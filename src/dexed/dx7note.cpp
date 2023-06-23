/*
 * Copyright 2016-2017 Pascal Gauthier.
 * Copyright 2012 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <math.h>
#include <stdlib.h>
#include "synth.h"
#include "freqlut.h"
#include "exp2.h"
#include "controllers.h"
#include "dx7note.h"
#include <iostream>
#include <cmath>

const int FEEDBACK_BITDEPTH = 8;

const int32_t coarsemul[] = {
    -16777216, 0, 16777216, 26591258, 33554432, 38955489, 43368474, 47099600,
    50331648, 53182516, 55732705, 58039632, 60145690, 62083076, 63876816,
    65546747, 67108864, 68576247, 69959732, 71268397, 72509921, 73690858,
    74816848, 75892776, 76922906, 77910978, 78860292, 79773775, 80654032,
    81503396, 82323963, 83117622
};

// 0, 66, 109, 255
static const uint32_t ampmodsenstab[] = {
    0, 4342338, 7171437, 16777216
};

static const uint8_t pitchmodsenstab[] = {
    0, 10, 20, 33, 55, 92, 153, 255
};


void Dx7Patch::update(const uint8_t patch[156]) {
    memcpy(currentPatch, patch, sizeof currentPatch);
}

int32_t Dx7Note::osc_freq(int logFreq_in, int mode, int coarse, int fine, int detune) {

    // TODO: pitch randomization
    int32_t logfreq;
    if (mode == 0) {
        logfreq = logFreq_in;

        // could use more precision, closer enough for now. those numbers comes from my DX7
        double detuneRatio = 0.0209 * exp(-0.396 * (((float)logfreq)/(1<<24))) / 7;
        logfreq += detuneRatio * logfreq * (detune - 7);
        
        logfreq += coarsemul[coarse & 31];
        if (fine) {
            // (1 << 24) / log(2)
            logfreq += (int32_t)floor(24204406.323123 * log(1 + 0.01 * fine) + 0.5);
        }
        
        // // This was measured at 7.213Hz per count at 9600Hz, but the exact
        // // value is somewhat dependent on midinote. Close enough for now.
        // //logfreq += 12606 * (detune -7);
    } else {
        // ((1 << 24) * log(10) / log(2) * .01) << 3
        logfreq = (4458616 * ((coarse & 3) * 100 + fine)) >> 3;
        logfreq += detune > 7 ? 13457 * (detune - 7) : 0;
    }
    return logfreq;
}

const uint8_t velocity_data[64] = {
    0, 70, 86, 97, 106, 114, 121, 126, 132, 138, 142, 148, 152, 156, 160, 163,
    166, 170, 173, 174, 178, 181, 184, 186, 189, 190, 194, 196, 198, 200, 202,
    205, 206, 209, 211, 214, 216, 218, 220, 222, 224, 225, 227, 229, 230, 232,
    233, 235, 237, 238, 240, 241, 242, 243, 244, 246, 246, 248, 249, 250, 251,
    252, 253, 254
};

// See "velocity" section of notes. Returns velocity delta in microsteps.
int ScaleVelocity(int velocity, int sensitivity) {
    int clamped_vel = max(0, min(127, velocity));
    int vel_value = velocity_data[clamped_vel >> 1] - 239;
    int scaled_vel = ((sensitivity * vel_value + 7) >> 3) << 4;
    return scaled_vel;
}

int ScaleRate(int midinote, int sensitivity) {
    int x = min(31, max(0, midinote / 3 - 7));
    int qratedelta = (sensitivity * x) >> 3;
#ifdef SUPER_PRECISE
    int rem = x & 7;
    if (sensitivity == 3 && rem == 3) {
        qratedelta -= 1;
    } else if (sensitivity == 7 && rem > 0 && rem < 4) {
        qratedelta += 1;
    }
#endif
    return qratedelta;
}

const uint8_t exp_scale_data[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 14, 16, 19, 23, 27, 33, 39, 47, 56, 66,
    80, 94, 110, 126, 142, 158, 174, 190, 206, 222, 238, 250
};

int ScaleCurve(int group, int depth, int curve) {
    int scale;
    if (curve == 0 || curve == 3) {
        // linear
        scale = (group * depth * 329) >> 12;
    } else {
        // exponential
        int n_scale_data = sizeof(exp_scale_data);
        int raw_exp = exp_scale_data[min(group, n_scale_data - 1)];
        scale = (raw_exp * depth * 329) >> 15;
    }
    if (curve < 2) {
        scale = -scale;
    }
    return scale;
}

int ScaleLevel(int midinote, int break_pt, int left_depth, int right_depth,
               int left_curve, int right_curve) {
    int offset = midinote - break_pt - 17;
    if (offset >= 0) {
        return ScaleCurve((offset+1) / 3, right_depth, right_curve);
    } else {
        return ScaleCurve(-(offset-1) / 3, left_depth, left_curve);
    }
}


Dx7Note::Dx7Note() {
    for(int op=0;op<6;op++) {
        phase[op] = 0;
        gain_out[op] = 0;
    }
}

// TODO: recalculate Scale() using logfreq
void Dx7Note::init(Dx7Patch &newp, int midinote, int logfreq, int velocity) {
    patch = newp.currentPatch;
    const uint8_t *patch = newp.currentPatch;
    
    for (int op = 0; op < 6; op++) {
        int off = op * 21;
        int outlevel = patch[off + 16];
        outlevel = Env::scaleoutlevel(outlevel);
        int level_scaling = ScaleLevel(midinote, patch[off + 8], patch[off + 9],
                                       patch[off + 10], patch[off + 11], patch[off + 12]);
        outlevel += level_scaling;
        outlevel = min(127, outlevel);
        outlevel = outlevel << 5;
        outlevel += ScaleVelocity(velocity, patch[off + 15]);
        outlevel = max(0, outlevel);
        int rate_scaling = ScaleRate(midinote, patch[off + 13]);
        env_[op].init(env_p(op), outlevel, rate_scaling);
        
		int mode = patch[op*21 + 17];
        int coarse = patch[off + 18];
        int fine = patch[off + 19];
        int detune = patch[off + 20];
        int32_t freq = osc_freq(logfreq, mode, coarse, fine, detune);
        basepitch_[op] = freq;
    }
    pitchenv_.set(pitchenv_p());
}

void Dx7Note::compute(int32_t *buf, int n, int32_t lfo_val, int32_t lfo_delay, int pb, const Controllers *ctrls) {
    // assert(n <= MAX_N);
    // ==== PITCH ====
	int pitchmoddepth = (patch[139] * 165) >> 6;
	int pitchmodsens = pitchmodsenstab[patch[143] & 7];
    uint32_t pmd = pitchmoddepth * lfo_delay;  // Q32
    int32_t senslfo = pitchmodsens * (lfo_val - (1 << 23));
    int32_t pmod_1 = (((int64_t) pmd) * (int64_t) senslfo) >> 39;
    pmod_1 = abs(pmod_1);
    int32_t pmod_2 = (int32_t)(((int64_t)ctrls->pitch_mod * (int64_t)senslfo) >> 14);
    pmod_2 = abs(pmod_2);
    int32_t pitch_mod = max(pmod_1, pmod_2);
    pitch_mod = pitchenv_.getsample(pitchenv_p(), n) + (pitch_mod * (senslfo < 0 ? -1 : 1));
    
    
    int32_t pitch_base = pb + ctrls->masterTune;
    pitch_mod += pitch_base;
    
    // ==== AMP MOD ====
    lfo_val = (1<<24) - lfo_val;
	int ampmoddepth = (patch[140] * 165) >> 6;
    uint32_t amod_1 = (uint32_t)(((int64_t) ampmoddepth * (int64_t) lfo_delay) >> 8); // Q24 :D
    amod_1 = (uint32_t)(((int64_t) amod_1 * (int64_t) lfo_val) >> 24);
    uint32_t amod_2 = (uint32_t)(((int64_t) ctrls->amp_mod * (int64_t) lfo_val) >> 7); // Q?? :|
    uint32_t amd_mod = max(amod_1, amod_2);
    
    // ==== EG AMP MOD ====
    uint32_t amod_3 = (ctrls->eg_mod+1) << 17;
    amd_mod = max((1<<24) - amod_3, amd_mod);
    
    FmOpParams params[6];
    // ==== OP RENDER ====
    for (int op = 0; op < 6; op++) {
        params[op].phase = phase[op];
        params[op].gain_out = gain_out[op];
        if ( ctrls->opSwitch[op] == '0' )  {
            env_[op].getsample(env_p(op), n); // advance the envelop even if it is not playing
            params[op].level_in = 0;
            params[op].freq = 0;
        } else {
			int off = op * 21;
            //int32_t gain = pow(2, 10 + level * (1.0 / (1 << 24)));
            
			int mode = patch[off + 17];
            if ( mode )
                params[op].freq = Freqlut::lookup(basepitch_[op] + pitch_base);
            else
                params[op].freq = Freqlut::lookup(basepitch_[op] + pitch_mod);
            
            int32_t level = env_[op].getsample(env_p(op), n);
			int ampmodsens = ampmodsenstab[patch[off + 14] & 3];
            if (ampmodsens != 0) {
                uint32_t sensamp = (uint32_t)(((uint64_t) amd_mod) * ((uint64_t) ampmodsens) >> 24);
                
                // TODO: mehhh.. this needs some real tuning.
                uint32_t pt = exp(((float)sensamp)/262144 * 0.07 + 12.2);
                uint32_t ldiff = (uint32_t)(((uint64_t)level) * (((uint64_t)pt<<4)) >> 28);
                level -= ldiff;
            }
            params[op].level_in = level;
        }
    }
	int algorithm = patch[134];
    int feedback = patch[135];
    int fb_shift = feedback != 0 ? FEEDBACK_BITDEPTH - feedback : 16;
    ctrls->core->render(buf, n, params, algorithm, fb_buf_, fb_shift);
    for (int op = 0; op < 6; op++) {
        phase[op] = params[op].phase;
        gain_out[op] = params[op].gain_out;
    }
}

void Dx7Note::keyup() {
    for (int op = 0; op < 6; op++) {
        env_[op].keydown(env_p(op), false);
    }
    pitchenv_.keydown(pitchenv_p(), false);
}

void Dx7Note::updateBasePitches(int logFreq)
{
    for (int op = 0; op < 6; op++)
    {
        int off = op * 21;
        int mode = patch[off + 17];
        int coarse = patch[off + 18];
        int fine = patch[off + 19];
        int detune = patch[off + 20];
        basepitch_[op] = osc_freq(logFreq, mode, coarse, fine, detune);
    }
}

// TODO: can share yet more codes with ::init()
void Dx7Note::update(Dx7Patch &newp, int midinote, int logFreq, int velocity) {
    patch = newp.currentPatch;

    for (int op = 0; op < 6; op++) {
        int off = op * 21;
        int mode = patch[off + 17];
        int coarse = patch[off + 18];
        int fine = patch[off + 19];
        int detune = patch[off + 20];
        basepitch_[op] = osc_freq(logFreq, mode, coarse, fine, detune);
        
        int outlevel = patch[off + 16];
        outlevel = Env::scaleoutlevel(outlevel);
        int level_scaling = ScaleLevel(midinote, patch[off + 8], patch[off + 9],
                                       patch[off + 10], patch[off + 11], patch[off + 12]);
        outlevel += level_scaling;
        outlevel = min(127, outlevel);
        outlevel = outlevel << 5;
        outlevel += ScaleVelocity(velocity, patch[off + 15]);
        outlevel = max(0, outlevel);
        int rate_scaling = ScaleRate(midinote, patch[off + 13]);
        env_[op].update(env_p(op), outlevel, rate_scaling);
    }

    // TODO: did we the pitchenv?
}

/**
 * Used in monophonic mode to transfer voice state from different notes
 */
void Dx7Note::transferState(Dx7Note &src) {
    for (int i=0;i<6;i++) {
        env_[i].transfer(src.env_[i]);
        gain_out[i] = src.gain_out[i];
        phase[i] = src.phase[i];
    }
}

void Dx7Note::transferSignal(Dx7Note &src) {
    for (int i=0;i<6;i++) {
        gain_out[i] = src.gain_out[i];
        phase[i] = src.phase[i];
    }
}

void Dx7Note::oscSync() {
    for (int i=0;i<6;i++) {
        gain_out[i] = 0;
        phase[i] = 0;
    }
}
