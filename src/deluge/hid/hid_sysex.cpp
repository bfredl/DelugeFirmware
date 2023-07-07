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

	if (data[4] == 0) {
		for (int blk = 0; blk < (OLED_MAIN_HEIGHT_PIXELS >> 3); blk++) {
			sendOLEDData(ip, d, cable, blk);
		}
	} else if (data[4] == 1 && len >= 7) {
		// only for testing: send a single block at a time.
		int row = data[5];
		if (row >= (OLED_MAIN_HEIGHT_PIXELS >> 3)) {
			return;
		}
		sendOLEDData(ip, d, cable, row);
	} else if (data[4] == 2 or data[4] == 3) {
		sendOLEDDataAll(ip, d, cable, (data[4] == 3));
	}

}

void HIDSysex::sendOLEDData(int ip, int d, int cable, int blk) {
	const int block_size = 128;
	const int packed_block_size = 147;

	uint8_t reply[7+packed_block_size+1] = {0xf0, 0x7e, 0x02, 0x40, 0x00};
	reply[5] = blk;

	int packed = pack_8bit_to_7bit(reply+6, packed_block_size,
								   &OLED::oledCurrentImage[blk][0], block_size);
	if (packed != packed_block_size) {
	  OLED::popupText("eror: fail");
	}
	reply[6+packed_block_size] = 0xf7; // end of transmission
	midiEngine.sendSysex(ip, d, cable, reply, packed_block_size+7);
}

static uint8_t big_buffer[1024];

void HIDSysex::sendOLEDDataAll(int ip, int d, int cable, bool rle) {
	const int block_size = 768;
	const int packed_block_size = 878;

	uint8_t reply_hdr[5] = {0xf0, 0x7e, 0x02, 0x40, rle ? 0x02 : 0x01};
	uint8_t *reply = big_buffer;
	memcpy(reply, reply_hdr, 5);
	reply[5] = 0;  // nominally 32*data[5] is start pos for a delta

	int packed = pack_8bit_to_7bit(reply+6, packed_block_size,
								   OLED::oledCurrentImage[0], block_size);
	if (packed != packed_block_size) {
	  OLED::popupText("eror: fail");
	}
	reply[6+packed_block_size] = 0xf7; // end of transmission
	midiEngine.sendSysex(ip, d, cable, reply, packed_block_size+7);
}
