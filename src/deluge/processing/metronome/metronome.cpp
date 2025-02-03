/*
 * Copyright © 2015-2023 Synthstrom Audible Limited
 *
 * This file is part of The Synthstrom Audible Deluge Firmware.
 *
 * The Synthstrom Audible Deluge Firmware is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "processing/metronome/metronome.h"
#include "dsp/stereo_sample.h"
#include "model/song/song.h"
#include "modulation/params/param_set.h"
#include "processing/engines/audio_engine.h"
#include "storage/flash_storage.h"
#include "util/fixedpoint.h"

Metronome::Metronome() {
	sounding = false;
	setVolume(25);
}

void Metronome::trigger(uint32_t newPhaseIncrement) {
	sounding = true;
	phase = 0;
	phaseIncrement = newPhaseIncrement;
	timeSinceTrigger = 0;
}

void Metronome::render(std::span<StereoSample> buffer) {
	if (!sounding) {
		return;
	}
	int32_t volumePostFX;
	if (currentSong) {
		volumePostFX =
		    getFinalParameterValueVolume(
		        134217728, cableToLinearParamShortcut(currentSong->paramManager.getUnpatchedParamSet()->getValue(
		                       deluge::modulation::params::UNPATCHED_VOLUME)))
		    >> 1;
	}
	else {
		volumePostFX = ONE_Q31;
	}

	q31_t high = multiply_32x32_rshift32(metronomeVolume, volumePostFX);

	for (StereoSample& sample : buffer) {
		q31_t value = (phase <= ONE_Q31) ? high : -high;
		phase += phaseIncrement;

		sample.l += value;
		sample.r += value;
	}

	timeSinceTrigger += buffer.size();
	if (timeSinceTrigger > 1000) {
		sounding = false;
	}
}
