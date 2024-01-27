#include "definitions_cxx.hpp"
#include "gui/l10n/language.h"
#include "gui/l10n/strings.h"

namespace deluge::l10n::built_in {
using enum String;
PLACE_SDRAM_DATA Language english{
    "English",
    {
        {EMPTY_STRING, ""},
        {STRING_FOR_ERROR_INSUFFICIENT_RAM, "Insufficient RAM"},
        {STRING_FOR_ERROR_INSUFFICIENT_RAM_FOR_FOLDER_CONTENTS_SIZE, "Too many files in folder"},
        {STRING_FOR_ERROR_SD_CARD, "SD card error"},
        {STRING_FOR_ERROR_SD_CARD_NOT_PRESENT, "No SD card present"},
        {STRING_FOR_ERROR_SD_CARD_NO_FILESYSTEM, "Please use FAT32-formatted card"},
        {STRING_FOR_ERROR_FILE_CORRUPTED, "File corrupted"},
        {STRING_FOR_ERROR_FILE_NOT_FOUND, "File not found"},
        {STRING_FOR_ERROR_FILE_UNREADABLE, "File unreadable"},
        {STRING_FOR_ERROR_FILE_UNSUPPORTED, "File unsupported"},
        {STRING_FOR_ERROR_FILE_FIRMWARE_VERSION_TOO_NEW, "Your firmware version is too old to open this file"},
        {STRING_FOR_ERROR_FOLDER_DOESNT_EXIST, "Folder not found"},
        {STRING_FOR_ERROR_BUG, "Bug encountered"},
        {STRING_FOR_ERROR_WRITE_FAIL, "SD card write error"},
        {STRING_FOR_ERROR_FILE_TOO_BIG, "File too large"},
        {STRING_FOR_ERROR_PRESET_IN_USE, "This preset is in-use elsewhere in your song"},
        {STRING_FOR_ERROR_NO_FURTHER_PRESETS, "This preset is in-use elsewhere in your song"},
        {STRING_FOR_ERROR_NO_FURTHER_FILES_THIS_DIRECTION, "No more presets found"},
        {STRING_FOR_ERROR_MAX_FILE_SIZE_REACHED, "Maximum file size reached"},
        {STRING_FOR_ERROR_SD_CARD_FULL, "SD card full"},
        {STRING_FOR_ERROR_FILE_NOT_LOADABLE_AS_WAVETABLE, "File does not contain wavetable data"},
        {STRING_FOR_ERROR_FILE_NOT_LOADABLE_AS_WAVETABLE_BECAUSE_STEREO, "Stereo files cannot be used as wavetables"},
        {STRING_FOR_ERROR_WRITE_PROTECTED, "Card is write-protected"},
        {STRING_FOR_ERROR_GENERIC, "You've encountered an unspecified error. Please report this bug."},

        // Patch sources
        {STRING_FOR_PATCH_SOURCE_LFO_GLOBAL, "LFO1"},
        {STRING_FOR_PATCH_SOURCE_LFO_LOCAL, "LFO2"},
        {STRING_FOR_PATCH_SOURCE_ENVELOPE_0, "Envelope 1"},
        {STRING_FOR_PATCH_SOURCE_ENVELOPE_1, "Envelope 2"},
        {STRING_FOR_PATCH_SOURCE_VELOCITY, "Velocity"},
        {STRING_FOR_PATCH_SOURCE_NOTE, "Note"},
        {STRING_FOR_PATCH_SOURCE_COMPRESSOR, "Sidechain"},
        {STRING_FOR_PATCH_SOURCE_RANDOM, "Random"},
        {STRING_FOR_PATCH_SOURCE_AFTERTOUCH, "Aftertouch"},
        {STRING_FOR_PATCH_SOURCE_X, "MPE X"},
        {STRING_FOR_PATCH_SOURCE_Y, "MPE Y"},

        /*
         * Start Parameter Strings
         */

        /*
         * Where used:
         * - automation_instrument_clip_view.cpp
         */

        // Patched Params (originally from functions.cpp)

        // Master Level, Pitch, Pan
        {STRING_FOR_PARAM_LOCAL_VOLUME, "Level"},
        {STRING_FOR_PARAM_GLOBAL_VOLUME_POST_FX, "Master Level"},
        {STRING_FOR_PARAM_LOCAL_PITCH_ADJUST, "Master Pitch"},
        {STRING_FOR_PARAM_LOCAL_PAN, "Master Pan"},

        // LPF Frequency, Resonance, Morph
        {STRING_FOR_PARAM_LOCAL_LPF_FREQ, "LPF frequency"},
        {STRING_FOR_PARAM_LOCAL_LPF_RESONANCE, "LPF resonance"},
        {STRING_FOR_PARAM_LOCAL_LPF_MORPH, "LPF morph"},

        // HPF Frequency, Resonance, Morph
        {STRING_FOR_PARAM_LOCAL_HPF_FREQ, "HPF frequency"},
        {STRING_FOR_PARAM_LOCAL_HPF_RESONANCE, "HPF resonance"},
        {STRING_FOR_PARAM_LOCAL_HPF_MORPH, "HPF morph"},

        // Reverb Amount
        {STRING_FOR_PARAM_GLOBAL_REVERB_AMOUNT, "Reverb amount"},

        // Delay Rate, Amount
        {STRING_FOR_PARAM_GLOBAL_DELAY_RATE, "Delay rate"},
        {STRING_FOR_PARAM_GLOBAL_DELAY_FEEDBACK, "Delay amount"},

        // Sidechain Level
        {STRING_FOR_PARAM_GLOBAL_VOLUME_POST_REVERB_SEND, "Sidechain Level"},

        // Wavefolder
        {STRING_FOR_WAVEFOLDER, "Wavefolder"},

        // OSC 1 Level, Pitch, Pulse Width, Carrier Feedback, Wave Position
        {STRING_FOR_PARAM_LOCAL_OSC_A_VOLUME, "Osc1 level"},
        {STRING_FOR_PARAM_LOCAL_OSC_A_PITCH_ADJUST, "Osc1 pitch"},
        {STRING_FOR_PARAM_LOCAL_OSC_A_PHASE_WIDTH, "Osc1 pulse width"},
        {STRING_FOR_PARAM_LOCAL_CARRIER_0_FEEDBACK, "Osc1 feedback"},
        {STRING_FOR_PARAM_LOCAL_OSC_A_WAVE_INDEX, "Osc1 wave position"},

        // OSC 2 Level, Pitch, Pulse Width, Carrier Feedback, Wave Position
        {STRING_FOR_PARAM_LOCAL_OSC_B_VOLUME, "Osc2 level"},
        {STRING_FOR_PARAM_LOCAL_OSC_B_PITCH_ADJUST, "Osc2 pitch"},
        {STRING_FOR_PARAM_LOCAL_OSC_B_PHASE_WIDTH, "Osc2 pulse width"},
        {STRING_FOR_PARAM_LOCAL_CARRIER_1_FEEDBACK, "Osc2 feedback"},
        {STRING_FOR_PARAM_LOCAL_OSC_B_WAVE_INDEX, "Osc2 wave position"},

        // FM Mod 1 Level, Pitch, Feedback
        {STRING_FOR_PARAM_LOCAL_MODULATOR_0_VOLUME, "FM mod1 level"},
        {STRING_FOR_PARAM_LOCAL_MODULATOR_0_PITCH_ADJUST, "FM mod1 pitch"},
        {STRING_FOR_PARAM_LOCAL_MODULATOR_0_FEEDBACK, "FM mod1 feedback"},

        // FM Mod 2 Level, Pitch, Feedback
        {STRING_FOR_PARAM_LOCAL_MODULATOR_1_VOLUME, "FM mod2 level"},
        {STRING_FOR_PARAM_LOCAL_MODULATOR_1_PITCH_ADJUST, "FM mod2 pitch"},
        {STRING_FOR_PARAM_LOCAL_MODULATOR_1_FEEDBACK, "FM mod2 feedback"},

        // Env 1 ADSR
        {STRING_FOR_PARAM_LOCAL_ENV_0_ATTACK, "Env1 attack"},
        {STRING_FOR_PARAM_LOCAL_ENV_0_DECAY, "Env1 decay"},
        {STRING_FOR_PARAM_LOCAL_ENV_0_SUSTAIN, "Env1 sustain"},
        {STRING_FOR_PARAM_LOCAL_ENV_0_RELEASE, "Env1 release"},

        // Env 2 ADSR
        {STRING_FOR_PARAM_LOCAL_ENV_1_ATTACK, "Env2 attack"},
        {STRING_FOR_PARAM_LOCAL_ENV_1_DECAY, "Env2 decay"},
        {STRING_FOR_PARAM_LOCAL_ENV_1_SUSTAIN, "Env2 sustain"},
        {STRING_FOR_PARAM_LOCAL_ENV_1_RELEASE, "Env2 release"},

        // LFO 1 Rate
        {STRING_FOR_PARAM_GLOBAL_LFO_FREQ, "LFO1 rate"},

        // LFO 2 Rate
        {STRING_FOR_PARAM_LOCAL_LFO_LOCAL_FREQ, "LFO2 rate"},

        // Mod FX Depth, Rate
        {STRING_FOR_PARAM_GLOBAL_MOD_FX_DEPTH, "Mod-FX depth"},
        {STRING_FOR_PARAM_GLOBAL_MOD_FX_RATE, "Mod-FX rate"},

        // Arp Rate
        {STRING_FOR_PARAM_GLOBAL_ARP_RATE, "Arp. rate"},

        // Noise
        {STRING_FOR_PARAM_LOCAL_NOISE_VOLUME, "Noise level"},

        // Unpatched Params (originally from functions.cpp)

        // Master Level, Pitch, Pan
        {STRING_FOR_MASTER_LEVEL, "Master level"},
        {STRING_FOR_MASTER_PITCH, "Master pitch"},
        {STRING_FOR_MASTER_PAN, "Master pan"},

        // LPF Frequency, Resonance
        {STRING_FOR_LPF_FREQUENCY, "LPF frequency"},
        {STRING_FOR_LPF_RESONANCE, "LPF resonance"},

        // HPF Frequency, Resonance
        {STRING_FOR_HPF_FREQUENCY, "HPF frequency"},
        {STRING_FOR_HPF_RESONANCE, "HPF resonance"},

        // Bass, Bass Freq
        {STRING_FOR_BASS, "BASS"},
        {STRING_FOR_BASS_FREQUENCY, "Bass frequency"},

        // Treble, Treble Freq
        {STRING_FOR_TREBLE, "TREBLE"},
        {STRING_FOR_TREBLE_FREQUENCY, "Treble frequency"},

        // Reverb Amount
        {STRING_FOR_REVERB_AMOUNT, "Reverb amount"},

        // Delay Rate, Amount
        {STRING_FOR_DELAY_RATE, "Delay rate"},
        {STRING_FOR_DELAY_AMOUNT, "Delay amount"},

        // Sidechain Level, Shape
        {STRING_FOR_SIDECHAIN_LEVEL, "Sidechain level"},
        {STRING_FOR_SIDECHAIN_SHAPE, "Sidechain shape"},

        // Decimation, Bitcrush
        {STRING_FOR_DISTORTION, "Distortion"},
        {STRING_FOR_DECIMATION, "DECIMATION"},
        {STRING_FOR_BITCRUSH, "Bitcrush"},

        // Mod FX Offset, Feedback, Depth, Rate
        {STRING_FOR_MODFX_OFFSET, "MOD-FX offset"},
        {STRING_FOR_MODFX_FEEDBACK, "MOD-FX feedback"},
        {STRING_FOR_MODFX_DEPTH, "MOD-FX depth"},
        {STRING_FOR_MODFX_RATE, "MOD-FX rate"},

        // Arp Gate
        {STRING_FOR_ARP_GATE_MENU_TITLE, "Arp. gate"},

        // Portamento
        {STRING_FOR_PORTAMENTO, "PORTAMENTO"},

        // Stutter Rate
        {STRING_FOR_STUTTER, "STUTTER"},
        {STRING_FOR_STUTTER_RATE, "Stutter Rate"},

        /*
         * End Parameter Strings
         */

        // General
        {STRING_FOR_DISABLED, "Disabled"},
        {STRING_FOR_ENABLED, "Enabled"},
        {STRING_FOR_OK, "OK"},
        {STRING_FOR_NEW, "New"},
        {STRING_FOR_DELETE, "Delete"},
        {STRING_FOR_SURE, "Sure"},
        {STRING_FOR_OVERWRITE, "Overwrite"},
        {STRING_FOR_OPTIONS, "Options"},

        // Menu Titles
        {STRING_FOR_AUDIO_SOURCE, "Audio source"},
        {STRING_FOR_ARE_YOU_SURE_QMARK, "Are you sure?"},
        {STRING_FOR_DELETE_QMARK, "Delete?"},
        {STRING_FOR_SAMPLES, "Sample(s)"},
        {STRING_FOR_LOAD_FILES, "Load file(s)"},
        {STRING_FOR_CLEAR_SONG_QMARK, "Clear song?"},
        {STRING_FOR_LOAD_PRESET, "Load preset"},
        {STRING_FOR_OVERWRITE_QMARK, "Overwrite?"},

        // gui/context_menu/audio_input_selector.cpp
        {STRING_FOR_LEFT_INPUT, "Left input"},
        {STRING_FOR_LEFT_INPUT_MONITORING, "Left input (monitoring)"},
        {STRING_FOR_RIGHT_INPUT, "Right input"},
        {STRING_FOR_RIGHT_INPUT_MONITORING, "Right input (monitoring)"},
        {STRING_FOR_STEREO_INPUT, "Stereo input"},
        {STRING_FOR_STEREO_INPUT_MONITORING, "Stereo input (monitoring)"},
        {STRING_FOR_BALANCED_INPUT, "Bal. input"},
        {STRING_FOR_BALANCED_INPUT_MONITORING, "Bal. input (monitoring)"},
        {STRING_FOR_MIX_PRE_FX, "Deluge mix (pre fx)"},
        {STRING_FOR_MIX_POST_FX, "Deluge output (post fx)"},

        // gui/context_menu/launch_style.cpp
        {STRING_FOR_DEFAULT_LAUNCH, "Default Clip"},
        {STRING_FOR_FILL_LAUNCH, "Fill Clip"},
        {STRING_FOR_ONCE_LAUNCH, "Once Clip"},

        // gui/context_menu/sample_browser/kit.cpp
        {STRING_FOR_LOAD_ALL, "Load all"},
        {STRING_FOR_SLICE, "Slice"},

        // gui/context_menu/sample_browser/synth.cpp
        {STRING_FOR_MULTISAMPLES, "Multisamples"},
        {STRING_FOR_BASIC, "Basic"},
        {STRING_FOR_SINGLE_CYCLE, "Single-cycle"},
        {STRING_FOR_WAVETABLE, "Wavetable"},

        // gui/context_menu/delete_file.cpp
        {STRING_FOR_ERROR_DELETING_FILE, "Error deleting file"},
        {STRING_FOR_FILE_DELETED, "File deleted"},

        // gui/context_menu/load_instrument_preset.cpp
        {STRING_FOR_CLONE, "Clone"},

        // gui/context_menu/save_song_or_instrument.cpp
        {STRING_FOR_COLLECT_MEDIA, "Collect media"},
        {STRING_FOR_CREATE_FOLDER, "Create folder"},

        // gui/menu_item/arpeggiator/mode.h
        {STRING_FOR_UP, "Up"},
        {STRING_FOR_DOWN, "Down"},
        {STRING_FOR_BOTH, "Both"},
        {STRING_FOR_RANDOM, "Random"},

        // gui/menu_item/cv/selection.h
        {STRING_FOR_CV_OUTPUT_N, "CV output *"},
        {STRING_FOR_CV_OUTPUT_1, "CV output 1"},
        {STRING_FOR_CV_OUTPUT_2, "CV output 2"},

        // gui/menu_item/delay/analog.h
        {STRING_FOR_DIGITAL, "Digital"},
        {STRING_FOR_ANALOG, "Analog"},

        // gui/menu_item/filter/lpf_mode.h
        {STRING_FOR_DRIVE, "Drive"},
        {STRING_FOR_SVF_BAND, "SVF Bandpass"},
        {STRING_FOR_SVF_NOTCH, "SVF Notch"},
        {STRING_FOR_HPLADDER, "HP Ladder"},
        {STRING_FOR_12DB_LADDER, "12dB Ladder"},
        {STRING_FOR_24DB_LADDER, "24dB Ladder"},
        // gui/menu_item/flash/status.h
        {STRING_FOR_FAST, "Fast"},
        {STRING_FOR_SLOW, "Slow"},
        {STRING_FOR_V_TRIGGER, "V-trig"},
        {STRING_FOR_S_TRIGGER, "S-trig"},
        {STRING_FOR_CLOCK, "Clock"},
        {STRING_FOR_RUN_SIGNAL, "Run signal"},

        // gui/menu_item/gate/selection.h
        {STRING_FOR_GATE_MODE_TITLE, "Gate out* mode"},
        {STRING_FOR_GATE_OUTPUT_1, "Gate output 1"},
        {STRING_FOR_GATE_OUTPUT_2, "Gate output 2"},
        {STRING_FOR_GATE_OUTPUT_3, "Gate output 3"},
        {STRING_FOR_GATE_OUTPUT_4, "Gate output 4"},
        {STRING_FOR_MINIMUM_OFF_TIME, "Minimum off-time"},

        // gui/menu_item/lfo/shape.h
        {STRING_FOR_SINE, "Sine"},
        {STRING_FOR_TRIANGLE, "Triangle"},
        {STRING_FOR_SQUARE, "Square"},
        {STRING_FOR_SAW, "Saw"},
        {STRING_FOR_SAMPLE_AND_HOLD, "S&H"},
        {STRING_FOR_RANDOM_WALK, "Random Walk"},

        // gui/menu_item/midi/takeover.h
        {STRING_FOR_JUMP, "Jump"},
        {STRING_FOR_PICK_UP, "Pickup"},
        {STRING_FOR_SCALE, "Scale"},

        // gui/menu_item/mod_fx/type.h
        {STRING_FOR_FLANGER, "Flanger"},
        {STRING_FOR_CHORUS, "Chorus"},
        {STRING_FOR_PHASER, "Phaser"},
        {STRING_FOR_STEREO_CHORUS, "Stereo Chorus"},

        // gui/menu_item/modulator/destination.h
        {STRING_FOR_CARRIERS, "Carriers"},
        {STRING_FOR_MODULATOR_1, "Modulator 1"},
        {STRING_FOR_MODULATOR_2, "Modulator 2"},

        // gui/menu_item/monitor/mode.h
        {STRING_FOR_CONDITIONAL, "Conditional"},
        {STRING_FOR_LOWER_ZONE, "Lower zone"},
        {STRING_FOR_UPPER_ZONE, "Upper zone"},
        {STRING_FOR_ANALOG_SQUARE, "Analog square"},
        {STRING_FOR_ANALOG_SAW, "Analog saw"},
        {STRING_FOR_SAMPLE, "Sample"},
        {STRING_FOR_SAMPLE, "DX7"},
        {STRING_FOR_INPUT, "Input"},
        {STRING_FOR_INPUT_LEFT, "Input (left)"},
        {STRING_FOR_INPUT_RIGHT, "Input (right)"},
        {STRING_FOR_INPUT_STEREO, "Input (stereo)"},
        {STRING_FOR_AUTOMATION_DELETED, "Automation deleted"},
        {STRING_FOR_RANGE_CONTAINS_ONE_NOTE, "Range contains only 1 note"},
        {STRING_FOR_LAST_RANGE_CANT_DELETE, "Only 1 range - can't delete"},
        {STRING_FOR_RANGE_DELETED, "Range deleted"},
        {STRING_FOR_BOTTOM, "Bottom"},
        {STRING_FOR_SHORTCUTS_VERSION_1, "1.0"},
        {STRING_FOR_SHORTCUTS_VERSION_3, "3.0"},
        {STRING_FOR_CANT_RECORD_AUDIO_FM_MODE, "Can't record audio into an FM synth"},

        // Autogen
        {STRING_FOR_FACTORY_RESET, "Factory reset"},
        {STRING_FOR_CLIPPING_OCCURRED, "Clipping occurred"},
        {STRING_FOR_NO_SAMPLE, "No sample"},
        {STRING_FOR_PARAMETER_NOT_APPLICABLE, "Parameter not applicable"},
        {STRING_FOR_SELECT_A_ROW_OR_AFFECT_ENTIRE, "Select row / affect-entire"},
        {STRING_FOR_UNIMPLEMENTED, "Feature not (yet?) implemented"},
        {STRING_FOR_CANT_LEARN, "Can't learn"},
        {STRING_FOR_CANT_LEARN_PC, "Can't learn Program Change"},
        {STRING_FOR_FOLDERS_CANNOT_BE_DELETED_ON_THE_DELUGE, "Folders cannot be deleted on the Deluge"},
        {STRING_FOR_ERROR_CREATING_MULTISAMPLED_INSTRUMENT, "Error creating multisampled instrument"},
        {STRING_FOR_CLIP_IS_RECORDING, "Clip is recording"},
        {STRING_FOR_AUDIO_FILE_IS_USED_IN_CURRENT_SONG, "Audio file is used in current song"},
        {STRING_FOR_CAN_ONLY_USE_SLICER_FOR_BRAND_NEW_KIT, "Can only user slicer for brand-new kit"},
        {STRING_FOR_TEMP_FOLDER_CANT_BE_BROWSED, "TEMP folder can't be browsed"},
        {STRING_FOR_UNLOADED_PARTS, "Can't return to current song, as parts have been unloaded"},
        {STRING_FOR_SD_CARD_ERROR, "SD card error"},
        {STRING_FOR_ERROR_LOADING_SONG, "Error loading song"},
        {STRING_FOR_DUPLICATE_NAMES, "Duplicate names"},
        {STRING_FOR_PRESET_SAVED, "Preset saved"},
        {STRING_FOR_SAME_NAME, "Another instrument in the song has the same name / number"},
        {STRING_FOR_SONG_SAVED, "Song saved"},
        {STRING_FOR_ERROR_MOVING_TEMP_FILES, "Song saved, but error moving temp files"},
        {STRING_FOR_OVERDUBS_PENDING, "Can't save while overdubs pending"},
        {STRING_FOR_INSTRUMENTS_WITH_CLIPS_CANT_BE_TURNED_INTO_AUDIO_TRACKS,
         "Instruments with clips can't be turned into audio tracks"},
        {STRING_FOR_NO_FREE_CHANNEL_SLOTS_AVAILABLE_IN_SONG, "No free channel slots available in song"},
        {STRING_FOR_MIDI_MUST_BE_LEARNED_TO_KIT_ITEMS_INDIVIDUALLY, "MIDI must be learned to kit items individually"},
        {STRING_FOR_AUDIO_TRACKS_WITH_CLIPS_CANT_BE_TURNED_INTO_AN_INSTRUMENT,
         "Audio tracks with clips can't be turned into an instrument"},
        {STRING_FOR_ARRANGEMENT_CLEARED, "Arrangement cleared"},
        {STRING_FOR_EMPTY_CLIP_INSTANCES_CANT_BE_MOVED_TO_THE_SESSION,
         "Empty clip instances can't be moved to the session"},
        {STRING_FOR_AUDIO_CLIP_CLEARED, "Audio clip cleared"},
        {STRING_FOR_CANT_EDIT_LENGTH, "Can't edit length"},
        {STRING_FOR_QUANTIZE_ALL_ROW, "QUANTIZE ALL ROW"},
        {STRING_FOR_QUANTIZE, "QUANTIZE"},
        {STRING_FOR_VELOCITY_DECREASED, "Velocity decreased"},
        {STRING_FOR_VELOCITY_INCREASED, "Velocity increased"},
        {STRING_FOR_RANDOMIZED, "Randomized"},
        {STRING_FOR_MAXIMUM_LENGTH_REACHED, "Maximum length reached"},
        {STRING_FOR_NOTES_PASTED, "Notes pasted"},
        {STRING_FOR_AUTOMATION_PASTED, "Automation pasted"},
        {STRING_FOR_CANT_PASTE_AUTOMATION, "Can't paste automation"},
        {STRING_FOR_NO_AUTOMATION_TO_PASTE, "No automation to paste"},
        {STRING_FOR_NOTES_COPIED, "Notes copied"},
        {STRING_FOR_NO_AUTOMATION_TO_COPY, "No automation to copy"},
        {STRING_FOR_AUTOMATION_COPIED, "Automation copied"},
        {STRING_FOR_DELETED_UNUSED_ROWS, "Deleted unused rows"},
        {STRING_FOR_AT_LEAST_ONE_ROW_NEEDS_TO_HAVE_NOTES, "At least one row needs to have notes"},
        {STRING_FOR_RECORDING_IN_PROGRESS, "Recording in progress"},
        {STRING_FOR_CANT_REMOVE_FINAL_CLIP, "Can't remove final clip"},
        {STRING_FOR_CLIP_NOT_EMPTY, "Clip not empty"},
        {STRING_FOR_RECORDING_TO_ARRANGEMENT, "Recording to arrangement"},
        {STRING_FOR_CANT_CREATE_OVERDUB_WHILE_CLIPS_SOLOING, "Can't create overdub while clips soloing"},
        {STRING_FOR_CLIP_WOULD_BREACH_MAX_ARRANGEMENT_LENGTH, "Clip would breach max arrangement length"},
        {STRING_FOR_NO_UNUSED_CHANNELS, "No unused channels"},
        {STRING_FOR_CLIP_HAS_INSTANCES_IN_ARRANGER, "Clip has instances in arranger"},
        {STRING_FOR_CANT_SAVE_WHILE_OVERDUBS_PENDING, "Can't save while overdubs pending"},
        {STRING_FOR_DIN_PORTS, "DIN ports"},
        {STRING_FOR_LOOPBACK, "Loopback"},
        {STRING_FOR_UPSTREAM_USB_PORT_3_SYSEX, "upstream USB port 3 (sysex)"},
        {STRING_FOR_UPSTREAM_USB_PORT_2, "upstream USB port 2"},
        {STRING_FOR_UPSTREAM_USB_PORT_1, "upstream USB port 1"},
        {STRING_FOR_HELLO_SYSEX, "hello sysex"},
        {STRING_FOR_OTHER_SCALE, "Other scale"},
        {STRING_FOR_CUSTOM_SCALE_WITH_MORE_THAN_7_NOTES_IN_USE, "Custom scale with more than 7 notes in use"},
        {STRING_FOR_CLIP_CLEARED, "Clip cleared"},
        {STRING_FOR_NO_FURTHER_UNUSED_INSTRUMENT_NUMBERS, "No further unused instrument numbers"},
        {STRING_FOR_CHANNEL_PRESSURE, "Channel pressure"},
        {STRING_FOR_PITCH_BEND, "Pitch bend"},
        {STRING_FOR_NO_PARAM, "No param"},
        {STRING_FOR_NO_FURTHER_UNUSED_MIDI_PARAMS, "No further unused MIDI params"},
        {STRING_FOR_ANALOG_DELAY, "Analog delay"},
        {STRING_FOR_NO_UNUSED_CHANNELS_AVAILABLE, "No unused channels available"},
        {STRING_FOR_NO_AVAILABLE_CHANNELS, "No available channels"},
        {STRING_FOR_PARAMETER_AUTOMATION_DELETED, "Parameter automation deleted"},
        {STRING_FOR_CREATE_OVERDUB_FROM_WHICH_CLIP, "Create overdub from which clip?"},
        {STRING_FOR_AUDIO_TRACK_HAS_NO_INPUT_CHANNEL, "Audio track has no input channel"},
        {STRING_FOR_CANT_GRAB_TEMPO_FROM_CLIP, "Can't grab tempo from clip"},
        {STRING_FOR_SYNC_NUDGED, "Sync nudged"},
        {STRING_FOR_FOLLOWING_EXTERNAL_CLOCK, "Following external clock"},
        {STRING_FOR_SLOW_SIDECHAIN_COMPRESSOR, "Slow sidechain compressor"},
        {STRING_FOR_FAST_SIDECHAIN_COMPRESSOR, "Fast sidechain compressor"},
        {STRING_FOR_REBOOT_TO_USE_THIS_SD_CARD, "Reboot to use this SD card"},
        {STRING_FOR_LARGE_ROOM_REVERB, "Large room reverb"},
        {STRING_FOR_MEDIUM_ROOM_REVERB, "Medium room reverb"},
        {STRING_FOR_SMALL_ROOM_REVERB, "Small room reverb"},
        {STRING_FOR_USB_DEVICES_MAX, "Maximum number of USB devices already hosted"},
        {STRING_FOR_USB_DEVICE_DETACHED, "USB device detached"},
        {STRING_FOR_USB_HUB_ATTACHED, "USB hub attached"},
        {STRING_FOR_USB_DEVICE_NOT_RECOGNIZED, "USB device not recognized"},

        {STRING_FOR_OUT, "Out"},
        {STRING_FOR_IN, "In"},

        {STRING_FOR_DEV_MENU_A, "Dev Menu A"},
        {STRING_FOR_DEV_MENU_B, "Dev Menu B"},
        {STRING_FOR_DEV_MENU_C, "Dev Menu C"},
        {STRING_FOR_DEV_MENU_D, "Dev Menu D"},
        {STRING_FOR_DEV_MENU_E, "Dev Menu E"},
        {STRING_FOR_DEV_MENU_F, "Dev Menu F"},
        {STRING_FOR_DEV_MENU_G, "Dev Menu G"},
        {STRING_FOR_ENVELOPE_1, "Envelope 1"},
        {STRING_FOR_ENVELOPE_2, "Envelope 2"},
        {STRING_FOR_VOLUME_LEVEL, "Volume"},
        {STRING_FOR_AMOUNT_LEVEL, "Level"},
        {STRING_FOR_REPEAT_MODE, "Repeat mode"},
        {STRING_FOR_PITCH_SPEED, "Pitch/speed"},
        {STRING_FOR_OSCILLATOR_SYNC, "Oscillator sync"},
        {STRING_FOR_OSCILLATOR_1, "Oscillator 1"},
        {STRING_FOR_OSCILLATOR_2, "Oscillator 2"},
        {STRING_FOR_UNISON_NUMBER, "Unison number"},
        {STRING_FOR_UNISON_DETUNE, "Unison detune"},
        {STRING_FOR_UNISON_STEREO_SPREAD, "Unison stereo spread"},
        {STRING_FOR_NUMBER_OF_OCTAVES, "Number of octaves"},
        {STRING_FOR_SHAPE, "SHAPE"},
        {STRING_FOR_MOD_FX, "Mod-fx"},
        {STRING_FOR_POLY_FINGER_MPE, "Poly / finger / MPE"},
        {STRING_FOR_REVERB_SIDECHAIN, "Reverb sidechain"},
        {STRING_FOR_ROOM_SIZE, "Room size"},
        {STRING_FOR_SUB_BANK, "Sub-bank"},
        {STRING_FOR_PLAY_DIRECTION, "Play direction"},
        {STRING_FOR_SWING_INTERVAL, "Swing interval"},
        {STRING_FOR_SHORTCUTS_VERSION, "Shortcuts version"},
        {STRING_FOR_KEYBOARD_FOR_TEXT, "Keyboard for text"},
        {STRING_FOR_LOOP_MARGINS, "Loop margins"},
        {STRING_FOR_SAMPLING_MONITORING, "Sampling monitoring"},
        {STRING_FOR_SAMPLE_PREVIEW, "Sample preview"},
        {STRING_FOR_PLAY_CURSOR, "Play-cursor"},
        {STRING_FOR_FIRMWARE_VERSION, "Firmware version"},
        {STRING_FOR_COMMUNITY_FTS, "Community features"},
        {STRING_FOR_MIDI_THRU, "MIDI-thru"},
        {STRING_FOR_TAKEOVER, "TAKEOVER"},
        {STRING_FOR_RECORD, "Record"},
        {STRING_FOR_COMMANDS, "Commands"},
        {STRING_FOR_OUTPUT, "Output"},
        {STRING_FOR_DEFAULT_UI, "UI"},
        {STRING_FOR_DEFAULT_UI_GRID, "Grid"},
        {STRING_FOR_DEFAULT_UI_DEFAULT_GRID_ACTIVE_MODE, "Default active mode"},
        {STRING_FOR_DEFAULT_UI_DEFAULT_GRID_ALLOW_GREEN_SELECTION, "Select in green mode"},
        {STRING_FOR_DEFAULT_UI_DEFAULT_GRID_ACTIVE_MODE_SELECTION, "Selection"},
        {STRING_FOR_DEFAULT_UI_DEFAULT_GRID_EMPTY_PADS, "Empty pads"},
        {STRING_FOR_DEFAULT_UI_DEFAULT_GRID_EMPTY_PADS_UNARM, "Unarm"},
        {STRING_FOR_DEFAULT_UI_DEFAULT_GRID_EMPTY_PADS_CREATE_REC, "Create + Record"},
        {STRING_FOR_DEFAULT_UI_LAYOUT, "Layout"},
        {STRING_FOR_DEFAULT_UI_KEYBOARD, "Keyboard"},
        {STRING_FOR_DEFAULT_UI_KEYBOARD_LAYOUT_ISOMORPHIC, "Isomorphic"},
        {STRING_FOR_DEFAULT_UI_KEYBOARD_LAYOUT_INKEY, "In-Key"},
        {STRING_FOR_DEFAULT_UI_SONG, "Song"},
        {STRING_FOR_DEFAULT_UI_SONG_LAYOUT_ROWS, "Rows"},
        {STRING_FOR_INPUT, "Input"},
        {STRING_FOR_TEMPO_MAGNITUDE_MATCHING, "Tempo magnitude matching"},
        {STRING_FOR_TRIGGER_CLOCK, "Trigger clock"},
        {STRING_FOR_FM_MODULATOR_1, "FM modulator 1"},
        {STRING_FOR_FM_MODULATOR_2, "FM modulator 2"},
        {STRING_FOR_DEXED_1, "DX7 source 1"},
        {STRING_FOR_DEXED_EDITOR, "DX7 patch editor"},
        {STRING_FOR_NOISE_LEVEL, "Noise level"},
        {STRING_FOR_MASTER_TRANSPOSE, "Master transpose"},
        {STRING_FOR_SYNTH_MODE, "Synth mode"},
        {STRING_FOR_FILTER_ROUTE, "Filter route"},

        {STRING_FOR_COMMUNITY_FEATURE_DRUM_RANDOMIZER, "Drum Randomizer"},
        {STRING_FOR_COMMUNITY_FEATURE_MASTER_COMPRESSOR, "Master Compressor"},
        {STRING_FOR_COMMUNITY_FEATURE_QUANTIZE, "Quantize"},
        {STRING_FOR_COMMUNITY_FEATURE_FINE_TEMPO_KNOB, "Fine Tempo Knob"},
        {STRING_FOR_COMMUNITY_FEATURE_MOD_DEPTH_DECIMALS, "Mod. Depth Decimals"},
        {STRING_FOR_COMMUNITY_FEATURE_CATCH_NOTES, "Catch Notes"},
        {STRING_FOR_COMMUNITY_FEATURE_DELETE_UNUSED_KIT_ROWS, "Delete Unused Kit Rows"},
        {STRING_FOR_COMMUNITY_FEATURE_ALT_DELAY_PARAMS, "Alternative Golden Knob Delay Params"},
        {STRING_FOR_COMMUNITY_FEATURE_QUANTIZED_STUTTER, "Stutter Rate Quantize"},
        {STRING_FOR_COMMUNITY_FEATURE_AUTOMATION, "Automation"},
        {STRING_FOR_COMMUNITY_FEATURE_AUTOMATION_INTERPOLATION, "Interpolation"},
        {STRING_FOR_COMMUNITY_FEATURE_AUTOMATION_CLEAR_CLIP, "Clear Clip"},
        {STRING_FOR_COMMUNITY_FEATURE_AUTOMATION_NUDGE_NOTE, "Nudge Note"},
        {STRING_FOR_COMMUNITY_FEATURE_AUTOMATION_SHIFT_CLIP, "Shift Note"},
        {STRING_FOR_COMMUNITY_FEATURE_AUTOMATION_DISABLE_AUDITION_PAD_SHORTCUTS, "Disable Audition Pad Shortcuts"},
        {STRING_FOR_COMMUNITY_FEATURE_DEV_SYSEX, "Allow Insecure Develop Sysex Messages"},
        {STRING_FOR_COMMUNITY_FEATURE_SYNC_SCALING_ACTION, "Sync Scaling Action"},
        {STRING_FOR_COMMUNITY_FEATURE_HIGHLIGHT_INCOMING_NOTES, "Highlight Incoming Notes"},
        {STRING_FOR_COMMUNITY_FEATURE_NORNS_LAYOUT, "Display Norns Layout"},
        {STRING_FOR_COMMUNITY_FEATURE_GRAIN_FX, "Enable Grain FX"},

        {STRING_FOR_TRACK_STILL_HAS_CLIPS_IN_SESSION, "Track still has clips in session"},
        {STRING_FOR_DELETE_ALL_TRACKS_CLIPS_FIRST, "Delete all track's clips first"},
        {STRING_FOR_CANT_DELETE_FINAL_CLIP, "Can't delete final Clip"},
        {STRING_FOR_NEW_SYNTH_CREATED, "New synth created"},
        {STRING_FOR_NEW_FM_SYNTH_CREATED, "New dx7 synth created"},
        {STRING_FOR_NEW_KIT_CREATED, "New kit created"},

        {STRING_FOR_CAN_ONLY_IMPORT_WHOLE_FOLDER_INTO_BRAND_NEW_KIT, "Can only import whole folder into brand-new kit"},
        {STRING_FOR_CANT_IMPORT_WHOLE_FOLDER_INTO_AUDIO_CLIP, "Can't import whole folder into audio clip"},

        {STRING_FOR_IMPOSSIBLE_FROM_GRID, "Impossible from Grid"},
        {STRING_FOR_SWITCHING_TO_TRACK_FAILED, "Switching to track failed"},
        {STRING_FOR_CANT_CLONE_AUDIO_IN_OTHER_TRACK, "Can't clone audio in other track"},
        {STRING_FOR_CANT_CONVERT_TYPE, "Can't convert audio/non-audio"},
        {STRING_FOR_TARGET_FULL, "Target full"},

        // Auto-extracted from menus.cpp
        {STRING_FOR_LPF_MORPH, "LPF morph"},
        {STRING_FOR_LPF_MODE, "LPF mode"},
        {STRING_FOR_MORPH, "Morph"},
        {STRING_FOR_HPF_MORPH, "HPF morph"},
        {STRING_FOR_HPF_MODE, "HPF mode"},
        {STRING_FOR_DECAY, "DECAY"},
        {STRING_FOR_SUSTAIN, "SUSTAIN"},
        {STRING_FOR_WAVE_INDEX, "Wave-index"},
        {STRING_FOR_RECORD_AUDIO, "Record audio"},
        {STRING_FOR_START_POINT, "Start-point"},
        {STRING_FOR_END_POINT, "End-point"},
        {STRING_FOR_SPEED, "SPEED"},
        {STRING_FOR_INTERPOLATION, "INTERPOLATION"},
        {STRING_FOR_PULSE_WIDTH, "PULSE WIDTH"},
        {STRING_FOR_UNISON, "UNISON"},
        {STRING_FOR_MODE, "MODE"},
        {STRING_FOR_ARPEGGIATOR, "ARPEGGIATOR"},
        {STRING_FOR_POLYPHONY, "POLYPHONY"},
        {STRING_FOR_PRIORITY, "PRIORITY"},
        {STRING_FOR_VOICE, "VOICE"},
        {STRING_FOR_DESTINATION, "Destination"},
        {STRING_FOR_RETRIGGER_PHASE, "Retrigger phase"},
        {STRING_FOR_LFO1_TYPE, "LFO1 type"},
        {STRING_FOR_LFO1_RATE, "LFO1 rate"},
        {STRING_FOR_LFO1_SYNC, "LFO1 sync"},
        {STRING_FOR_LFO1, "LFO1"},
        {STRING_FOR_LFO2_TYPE, "LFO2 type"},
        {STRING_FOR_LFO2_RATE, "LFO2 rate"},
        {STRING_FOR_LFO2, "LFO2"},
        {STRING_FOR_MODFX_TYPE, "MOD-FX type"},
        {STRING_FOR_FEEDBACK, "FEEDBACK"},
        {STRING_FOR_OFFSET, "OFFSET"},
        {STRING_FOR_EQ, "EQ"},
        {STRING_FOR_PINGPONG, "Pingpong"},
        {STRING_FOR_DELAY_PINGPONG, "Delay pingpong"},
        {STRING_FOR_DELAY_TYPE, "Delay type"},
        {STRING_FOR_DELAY_SYNC, "Delay sync"},
        {STRING_FOR_NORMAL, "Normal"},
        {STRING_FOR_SEND_TO_SIDECHAIN, "Send to sidechain"},
        {STRING_FOR_SYNC, "SYNC"},
        {STRING_FOR_SIDECHAIN_SYNC, "Sidechain sync"},
        {STRING_FOR_RELEASE, "RELEASE"},
        {STRING_FOR_DAMPENING, "DAMPENING"},
        {STRING_FOR_WIDTH, "WIDTH"},
        {STRING_FOR_REVERB_WIDTH, "Reverb width"},
        {STRING_FOR_REVERB_PAN, "Reverb pan"},
        {STRING_FOR_SATURATION, "SATURATION"},
        {STRING_FOR_BANK, "BANK"},
        {STRING_FOR_MIDI_BANK, "MIDI bank"},
        {STRING_FOR_MIDI_SUB_BANK, "MIDI sub-bank"},
        {STRING_FOR_PGM, "PGM"},
        {STRING_FOR_REVERSE, "REVERSE"},
        {STRING_FOR_WAVEFORM, "WAVEFORM"},
        {STRING_FOR_RESONANCE, "Resonance"},
        {STRING_FOR_LPF, "LPF"},
        {STRING_FOR_FREQUENCY, "Frequency"},
        {STRING_FOR_HPF, "HPF"},
        {STRING_FOR_TYPE, "TYPE"},
        {STRING_FOR_MOD_FX_TYPE, "MOD FX type"},
        {STRING_FOR_MOD_FX_RATE, "MOD FX rate"},
        {STRING_FOR_DEPTH, "DEPTH"},
        {STRING_FOR_MOD_FX_DEPTH, "MOD FX depth"},
        {STRING_FOR_RATE, "RATE"},
        {STRING_FOR_DELAY, "DELAY"},
        {STRING_FOR_AMOUNT, "AMOUNT"},
        {STRING_FOR_REVERB, "REVERB"},
        {STRING_FOR_VOLUME_DUCKING, "Volume ducking"},
        {STRING_FOR_SIDECHAIN_COMPRESSOR, "Sidechain compressor"},
        {STRING_FOR_ATTACK, "ATTACK"},
        {STRING_FOR_FX, "FX"},
        {STRING_FOR_VOLTS_PER_OCTAVE, "Volts per octave"},
        {STRING_FOR_TRANSPOSE, "TRANSPOSE"},
        {STRING_FOR_CV, "CV"},
        {STRING_FOR_CV_OUTPUTS, "CV outputs"},
        {STRING_FOR_GATE, "GATE"},
        {STRING_FOR_GATE_OUTPUTS, "Gate outputs"},
        {STRING_FOR_KEY_LAYOUT, "Key layout"},
        {STRING_FOR_COLOURS, "COLOURS"},
        {STRING_FOR_PADS, "PADS"},
        {STRING_FOR_QUANTIZATION, "Quantization"},
        {STRING_FOR_COUNT_IN, "Count-in"},
        {STRING_FOR_REC_COUNT_IN, "Rec count-in"},
        {STRING_FOR_MONITORING, "Monitoring"},
        {STRING_FOR_RECORDING, "Recording"},
        {STRING_FOR_RESTART, "Restart"},
        {STRING_FOR_PLAY, "PLAY"},
        {STRING_FOR_TAP_TEMPO, "Tap tempo"},
        {STRING_FOR_UNDO, "UNDO"},
        {STRING_FOR_REDO, "REDO"},
        {STRING_FOR_LOOP, "LOOP"},
        {STRING_FOR_LAYERING_LOOP, "LAYERING loop"},
        {STRING_FOR_MIDI_COMMANDS, "MIDI commands"},
        {STRING_FOR_DIFFERENTIATE_INPUTS, "Differentiate inputs"},
        {STRING_FOR_MIDI_CLOCK_OUT, "MIDI clock out"},
        {STRING_FOR_MIDI_CLOCK_IN, "MIDI clock in"},
        {STRING_FOR_DEVICES, "Devices"},
        {STRING_FOR_MIDI_DEVICES, "MIDI devices"},
        {STRING_FOR_MPE, "MPE"},
        {STRING_FOR_MPE_MONO, "Poly Expression to Mono"},
        {STRING_FOR_MIDI_CLOCK, "MIDI clock"},
        {STRING_FOR_MIDI, "MIDI"},
        {STRING_FOR_PPQN, "PPQN"},
        {STRING_FOR_INPUT_PPQN, "Input PPQN"},
        {STRING_FOR_AUTO_START, "Auto-start"},
        {STRING_FOR_OUTPUT_PPQN, "Output PPQN"},
        {STRING_FOR_TEMPO, "TEMPO"},
        {STRING_FOR_DEFAULT_TEMPO, "Default tempo"},
        {STRING_FOR_SWING, "SWING"},
        {STRING_FOR_DEFAULT_SWING, "Default swing"},
        {STRING_FOR_KEY, "KEY"},
        {STRING_FOR_DEFAULT_KEY, "Default key"},
        {STRING_FOR_DEFAULT_SCALE, "Default scale"},
        {STRING_FOR_VELOCITY, "VELOCITY"},
        {STRING_FOR_RESOLUTION, "RESOLUTION"},
        {STRING_FOR_DEFAULT_BEND_R, "Default bend r"},
        {STRING_FOR_DEFAULTS, "DEFAULTS"},
        {STRING_FOR_VIBRATO, "VIBRATO"},
        {STRING_FOR_NAME, "NAME"},
        {STRING_FOR_BEND_RANGE, "Bend range"},
        {STRING_FOR_PAN, "PAN"},
        {STRING_FOR_SOUND, "Sound"},
        {STRING_FOR_AUDIO_CLIP, "Audio clip"},
        {STRING_FOR_SONG, "Song"},
        {STRING_FOR_SETTINGS, "Settings"},
        {STRING_FOR_ENV_ATTACK_MENU_TITLE, "Env* attack"},
        {STRING_FOR_ENV_DECAY_MENU_TITLE, "Env* decay"},
        {STRING_FOR_ENV_SUSTAIN_MENU_TITLE, "Env* sustain"},
        {STRING_FOR_ENV_RELEASE_MENU_TITLE, "Env* release"},
        {STRING_FOR_OSC_TYPE_MENU_TITLE, "Osc* type"},
        {STRING_FOR_OSC_WAVE_IND_MENU_TITLE, "Osc* wave-ind."},
        {STRING_FOR_OSC_LEVEL_MENU_TITLE, "Osc* level"},
        {STRING_FOR_CARRIER_FEED_MENU_TITLE, "Carrier* feed."},
        {STRING_FOR_SAMP_REVERSE_MENU_TITLE, "Samp* reverse"},
        {STRING_FOR_SAMP_REPEAT_MENU_TITLE, "Samp* repeat"},
        {STRING_FOR_OSC_TRANSPOSE_MENU_TITLE, "Osc* transpose"},
        {STRING_FOR_SAMP_SPEED_MENU_TITLE, "Samp* speed"},
        {STRING_FOR_SAMP_INTERP_MENU_TITLE, "Samp* interp."},
        {STRING_FOR_OSC_P_WIDTH_MENU_TITLE, "Osc* p. width"},
        {STRING_FOR_OSC_R_PHASE_MENU_TITLE, "Osc* r. phase"},
        {STRING_FOR_ARP_MODE_MENU_TITLE, "Arp. mode"},
        {STRING_FOR_ARP_SYNC_MENU_TITLE, "Arp. sync"},
        {STRING_FOR_ARP_OCTAVES_MENU_TITLE, "Arp. octaves"},
        {STRING_FOR_ARP_RATE_MENU_TITLE, "Arp. rate"},
        {STRING_FOR_FM_MOD_TRAN_MENU_TITLE, "FM Mod* tran."},
        {STRING_FOR_FM_MOD_LEVEL_MENU_TITLE, "FM Mod* level"},
        {STRING_FOR_FM_MOD_FBACK_MENU_TITLE, "FM Mod* f.back"},
        {STRING_FOR_FM_MOD2_DEST_MENU_TITLE, "FM Mod2 dest."},
        {STRING_FOR_FM_MOD_RETRIG_MENU_TITLE, "FM Mod* retrig"},
        {STRING_FOR_SIDECH_ATTACK_MENU_TITLE, "Sidech. attack"},
        {STRING_FOR_SIDECH_SHAPE_MENU_TITLE, "Sidech. shape"},
        {STRING_FOR_REVERB_SIDECH_MENU_TITLE, "Reverb sidech."},
        {STRING_FOR_MIDI_PGM_NUMB_MENU_TITLE, "MIDI PGM numb."},
        {STRING_FOR_CV_V_PER_OCTAVE_MENU_TITLE, "CV* V/octave"},
        {STRING_FOR_CV_TRANSPOSE_MENU_TITLE, "CV* transpose"},
        {STRING_FOR_SHORTCUTS_VER_MENU_TITLE, "Shortcuts ver."},
        {STRING_FOR_FIRMWARE_VER_MENU_TITLE, "Firmware ver."},
        {STRING_FOR_COMMUNITY_FTS_MENU_TITLE, "Community fts."},
        {STRING_FOR_TEMPO_M_MATCH_MENU_TITLE, "Tempo m. match"},
        {STRING_FOR_T_CLOCK_INPUT_MENU_TITLE, "T. clock input"},
        {STRING_FOR_T_CLOCK_OUT_MENU_TITLE, "T. clock out"},
        {STRING_FOR_DEFAULT_VELOC_MENU_TITLE, "Default veloc."},
        {STRING_FOR_DEFAULT_RESOL_MENU_TITLE, "Default resol."},
        {STRING_FOR_MASTER_TRAN_MENU_TITLE, "Master tran."},
        {STRING_FOR_MIDI_INST_MENU_TITLE, "MIDI inst."},
        {STRING_FOR_SEND_TO_SIDECH_MENU_TITLE, "Send to sidech"},
        {STRING_FOR_SIDECH_RELEASE_MENU_TITLE, "Sidech release"},
        {STRING_FOR_SIDECHAIN_COMP_MENU_TITLE, "Sidechain comp"},
        {STRING_FOR_NUM_MEMBER_CH_MENU_TITLE, "Num member ch."},
        {STRING_FOR_METRONOME, "METRONOME"},
        {STRING_FOR_DEFAULT_METRO_MENU_TITLE, "Default metro."},

        {STRING_FOR_CV_INSTRUMENT, "CV instrument"},

        {STRING_FOR_ACTIVE, "ACTIVE"},
        {STRING_FOR_STOPPED, "STOPPED"},
        {STRING_FOR_MUTED, "MUTED"},
        {STRING_FOR_SOLOED, "SOLOED"},

        {STRING_FOR_FILE_BROWSER, "File browser"},

        // Auto-extracted from deluge/gui/menu_items/
        {STRING_FOR_LOW, "LOW"},
        {STRING_FOR_MEDIUM, "MEDIUM"},
        {STRING_FOR_HIGH, "HIGH"},
        {STRING_FOR_AUTO, "Auto"},
        {STRING_FOR_POLYPHONIC, "Polyphonic"},
        {STRING_FOR_MONOPHONIC, "Monophonic"},
        {STRING_FOR_LEGATO, "Legato"},
        {STRING_FOR_CHOKE, "Choke"},
        {STRING_FOR_MODULATE_WITH, "Modulate with"},
        {STRING_FOR_MODULATE_DEPTH, "Modulate depth"},
        {STRING_FOR_FORWARD, "FORWARD"},
        {STRING_FOR_REVERSED, "REVERSED"},
        {STRING_FOR_PING_PONG, "PING-PONG"},
        {STRING_FOR_NONE, "NONE"},
        {STRING_FOR_OFF, "Off"},
        {STRING_FOR_ON, "On"},
        {STRING_FOR_CUT, "CUT"},
        {STRING_FOR_ONCE, "ONCE"},
        {STRING_FOR_STRETCH, "STRETCH"},
        {STRING_FOR_LINKED, "Linked"},
        {STRING_FOR_INDEPENDENT, "Independent"},
        {STRING_FOR_LINEAR, "Linear"},
        {STRING_FOR_SINC, "Sinc"},
        {STRING_FOR_MPE_OUTPUT, "MPE output"},
        {STRING_FOR_MPE_INPUT, "MPE input"},
        {STRING_FOR_SUBTRACTIVE, "Subtractive"},
        {STRING_FOR_FM, "FM"},
        {STRING_FOR_RINGMOD, "Ringmod"},
        {STRING_FOR_NOTE_RANGE, "Note range"},
        {STRING_FOR_PARALLEL, "PARALLEL"},
        {STRING_FOR_RED, "RED"},
        {STRING_FOR_GREEN, "GREEN"},
        {STRING_FOR_BLUE, "BLUE"},
        {STRING_FOR_YELLOW, "YELLOW"},
        {STRING_FOR_CYAN, "CYAN"},
        {STRING_FOR_MAGENTA, "PURPLE"},
        {STRING_FOR_AMBER, "AMBER"},
        {STRING_FOR_WHITE, "WHITE"},
        {STRING_FOR_PINK, "PINK"},
        {STRING_FOR_COMMAND_UNASSIGNED, "Command unassigned"},
        {STRING_FOR_ANY_MIDI_DEVICE, "Any MIDI device"},
        {STRING_FOR_MPE_LOWER_ZONE, "MPE lower zone"},
        {STRING_FOR_MPE_UPPER_ZONE, "MPE upper zone"},
        {STRING_FOR_CHANNEL, "Channel"},
        {STRING_FOR_SET, "SET"},
        {STRING_FOR_UNLEARNED, "UNLEARNED"},
        {STRING_FOR_LEARNED, "LEARNED"},
        {STRING_FOR_RANGE_INSERTED, "Range inserted"},
        {STRING_FOR_INSERT, "INSERT"},

        {STRING_FOR_SVF_BAND, "SVF Bandpass"},
        {STRING_FOR_SVF_NOTCH, "SVF Notch"},
        {STRING_FOR_LOOP_TOO_SHORT, "Loop too short"},
        {STRING_FOR_LOOP_HALVED, "Loop halved"},
        {STRING_FOR_LOOP_TOO_LONG, "Loop too long"},
        {STRING_FOR_LOOP_DOUBLED, "Loop doubled"},
        {STRING_FOR_WAVEFOLD, "Wavefold"},
        {STRING_FOR_MOD_MATRIX, "Mod matrix"},

        {STRING_FOR_CHECKSUM_FAIL, "Checksum fail"},
        {STRING_FOR_WRONG_SIZE, "Wrong size?"},
        {STRING_FOR_NOTES_CLEARED, "Notes cleared"},
        {STRING_FOR_AUTOMATION_CLEARED, "Automation cleared"},

        {STRING_FOR_GRAIN, "Grain"},
        {STRING_FOR_FILL, "Fill"},

        /* Strings Specifically for Automation Instrument Clip View
         * automation_instrument_clip_view.cpp
         */

        {STRING_FOR_AUTOMATION, "AUTOMATION"},
        {STRING_FOR_AUTOMATION_OVERVIEW, "Automation Overview"},
        {STRING_FOR_AUTOMATION_OFF, "(Not Automated)"},
        {STRING_FOR_AUTOMATION_ON, "(Automated)"},
        {STRING_FOR_COMING_SOON, "Coming Soon"},
        {STRING_FOR_CANT_AUTOMATE_CV, "Can't Automate CV"},
        {STRING_FOR_SHIFT_LEFT, "Shift Left"},
        {STRING_FOR_SHIFT_RIGHT, "Shift Right"},
        {STRING_FOR_INTERPOLATION_DISABLED, "Interpolation Off"},
        {STRING_FOR_INTERPOLATION_ENABLED, "Interpolation On"},
        {STRING_FOR_PAD_SELECTION_OFF, "Pad Selection Off"},
        {STRING_FOR_PAD_SELECTION_ON, "Pad Selection On"},

        /* Strings Specifically for Performance View
         * performance_session_view.cpp
         */

        {STRING_FOR_PERFORM_VIEW, "Performance View"},
        {STRING_FOR_PERFORM_FX, "Perform FX"},
        {STRING_FOR_PERFORM_EDITOR, "Editing Mode"},
        {STRING_FOR_PERFORM_EDIT_PARAM, "Param"},
        {STRING_FOR_PERFORM_EDIT_VALUE, "Value"},
        {STRING_FOR_PERFORM_DEFAULTS_LOADED, "Defaults Loaded"},
        {STRING_FOR_PERFORM_DEFAULTS_SAVED, "Defaults Saved"},

        /* Strings Specifically for Song View
         * session_view.cpp
         */
        {STRING_FOR_MIDILOOPBACK, "MIDI Loopback"},

        {STRING_FOR_SONG_VIEW, "Song View"},

        /* Strings Specifically for Arranger View
         * arranger_view.cpp
         */

        {STRING_FOR_ARRANGER_VIEW, "Arranger View"},

        /* Strings for Kit Global FX Menu*/

        {STRING_FOR_KIT_GLOBAL_FX, "Kit FX"},
        {STRING_FOR_PITCH, "Pitch"},

        /* Strings Specifically for Midi Learning View
         * midi_follow.cpp
         */
        // strings for midi follow menu
        {STRING_FOR_FOLLOW_TITLE, "Midi-Follow"},
        {STRING_FOR_FOLLOW, "Follow"},
        {STRING_FOR_FOLLOW_CHANNEL_A, "Channel A"},
        {STRING_FOR_FOLLOW_CHANNEL_B, "Channel B"},
        {STRING_FOR_FOLLOW_CHANNEL_C, "Channel C"},
        {STRING_FOR_FOLLOW_KIT_ROOT_NOTE, "Kit Root Note"},
        {STRING_FOR_FOLLOW_DISPLAY_PARAM, "Display Param"},
        {STRING_FOR_FOLLOW_FEEDBACK, "Feedback"},
        {STRING_FOR_FOLLOW_FEEDBACK_AUTOMATION, "Automation Feedback"},
        {STRING_FOR_FOLLOW_FEEDBACK_FILTER, "Filter Responses"},
        {STRING_FOR_FOLLOW_DEVICE_UNASSIGNED, "Device unassigned"},
        {STRING_FOR_FOLLOW_CHANNEL_UNASSIGNED, "Channel unassigned"},
        {STRING_FOR_INPUT_DIFFERENTIATION_ON, "Use learned device:"},
        {STRING_FOR_INPUT_DIFFERENTIATION_OFF, "Use any device"},

        // String for Select Kit Row Menu

        {STRING_FOR_SELECT_KIT_ROW, "Select Kit Row"},

        {STRING_FOR_MODEL, "Model"},
        {STRING_FOR_FREEVERB, "Freeverb"},
        {STRING_FOR_MUTABLE, "Mutable"},
    },
};
} // namespace deluge::l10n::built_in
