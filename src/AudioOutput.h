/*
 * Copyright © 2019-2023 Synthstrom Audible Limited
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

#ifndef AUDIOOUTPUT_H_
#define AUDIOOUTPUT_H_

#include <GlobalEffectableForClip.h>
#include "Output.h"
#include "envelope.h"

class ModelStackWithTimelineCounter;

class AudioOutput final : public Output, public GlobalEffectableForClip {
public:
	AudioOutput();
	virtual ~AudioOutput();
	void cloneFrom(ModControllableAudio* other);

	void renderOutput(ModelStack* modelStack, StereoSample *startPos, StereoSample *endPos, int numSamples, int32_t* reverbBuffer, int32_t reverbAmountAdjust, int32_t sideChainHitPending, bool shouldLimitDelayFeedback, bool isClipActive);

    void renderGlobalEffectableForClip(ModelStackWithTimelineCounter* modelStack, StereoSample* globalEffectableBuffer, int32_t* bufferToTransferTo, int numSamples, int32_t* reverbBuffer, int32_t reverbAmountAdjust,
        		int32_t sideChainHitPending, bool shouldLimitDelayFeedback, bool isClipActive, int32_t pitchAdjust, int32_t amplitudeAtStart, int32_t amplitudeAtEnd);

    void resetEnvelope();

	ModControllable* toModControllable() { return this; }
	uint8_t* getModKnobMode() { return &modKnobMode; }

	void cutAllSound();
	void getThingWithMostReverb(Sound** soundWithMostReverb, ParamManagerForTimeline** paramManagerWithMostReverb, Kit** kitWithMostReverb,
				int32_t* highestReverbAmountFound);

	int readFromFile(Song* song, Clip* clip, int32_t readAutomationUpToPos);
    bool writeDataToFile(Clip* clipForSavingOutputOnly, Song* song);
    void deleteBackedUpParamManagers(Song* song);
    bool setActiveClip(ModelStackWithTimelineCounter* modelStack, int maySendMIDIPGMs = PGM_CHANGE_SEND_ONCE);
    bool isSkippingRendering();
    Output* toOutput() { return this; }
    void getThingWithMostReverb(Sound** soundWithMostReverb, ParamManager** paramManagerWithMostReverb, GlobalEffectableForClip** globalEffectableWithMostReverb, int32_t* highestReverbAmountFound);

    // A TimelineCounter is required
    void offerReceivedCCToLearnedParams(MIDIDevice* fromDevice, uint8_t channel, uint8_t ccNumber, uint8_t value, ModelStackWithTimelineCounter* modelStack) {
        ModControllableAudio::offerReceivedCCToLearnedParams(fromDevice, channel, ccNumber, value, modelStack);
    }
    bool offerReceivedPitchBendToLearnedParams(MIDIDevice* fromDevice, uint8_t channel, uint8_t data1, uint8_t data2, ModelStackWithTimelineCounter* modelStack) {
    	return ModControllableAudio::offerReceivedPitchBendToLearnedParams(fromDevice, channel, data1, data2, modelStack);
    }

    char const* getXMLTag() { return "audioTrack"; }

    Envelope envelope;

    int32_t amplitudeLastTime;

    int32_t overrideAmplitudeEnvelopeReleaseRate;

    int8_t inputChannel;
    bool echoing; // Doesn't get cloned - we wouldn't want that!

protected:
    Clip* createNewClipForArrangementRecording(ModelStack* modelStack);
    bool wantsToBeginArrangementRecording();
    bool willRenderAsOneChannelOnlyWhichWillNeedCopying();
};

#endif /* AUDIOOUTPUT_H_ */
