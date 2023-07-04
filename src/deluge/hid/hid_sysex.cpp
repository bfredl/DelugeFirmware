#include "hid/hid_sysex.h"
#include "hid/display/oled.h"
#include "io/midi/midi_engine.h"
#include "util/functions.h"

void (*oled_override)(int ip, int d, int cable, uint8_t* data, int len) = NULL;


void HIDSysex::sysexReceived(int ip, int d, int cable, uint8_t* data, int len) {
	if (len < 6) {
		return;
	}
	// first three bytes are already used, next is command
	switch (data[3]) {
		case 0: // request OLED dispaly
			requestOLEDDisplay(ip, d, cable, data, len);
			break;

		default:
		break;

	}
}


void HIDSysex::requestOLEDDisplay(int ip, int d, int cable, uint8_t* data, int len) {
	if (oled_override) {
		return oled_override(ip, d, cable, data, len);
	}

	if (data[4] != 0 or len < 8) {
		return; // reserved
	}

	int row = data[5];
	int col = data[6];

	if (row >= (OLED_MAIN_HEIGHT_PIXELS >> 3)) {
		return;
	}

	if (col == 0x7f) {
		const int block_size = 128;
		const int packed_block_size = 147;

		uint8_t reply[7+packed_block_size+1] = {0xf0, 0x7e, 0x02, 0x40, 0x00};
		reply[5] = row;
		reply[6] = col;

		int packed = pack_8bit_to_7bit(reply+7, packed_block_size,
									  &OLED::oledCurrentImage[row][block_size], block_size);
		if (packed != packed_block_size) {
		  OLED::popupText("eror: fail");
		}
		reply[6+packed_block_size] = 0xf7; // end of transmission
		midiEngine.sendSysex(ip, d, cable, reply, packed_block_size+7);
	} else {
		if (col >= 4) return;
		const int block_size = 32;
		const int packed_block_size =37;

		uint8_t reply[7+packed_block_size+1] = {0xf0, 0x7e, 0x02, 0x40, 0x00};
		reply[5] = row;
		reply[6] = col;

		int packed = pack_8bit_to_7bit(reply+7, packed_block_size,
									  &OLED::oledCurrentImage[row][col*block_size], block_size);
		if (packed != packed_block_size) {
		  OLED::popupText("eror: vad i");
		}
		reply[6+packed_block_size] = 0xf7; // end of transmission
		midiEngine.sendSysex(ip, d, cable, reply, packed_block_size+7);
	}
}
