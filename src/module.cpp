// data format:
// F0 67 CMD extra F7
// write buffer:
// F0 67 0 pos1 pos2 [byte0-7low] [byte8-15low] byte0_6high byte8_14high flags F7 == 5+8+8+2+2 = 25 bytes
// where byteXlow = byteX % 0x7F
// and high bytes are collated in LSB order, ie byte0 = byte0low+0x80*(1 & byte0_6high), byte1 = byte1low+0x80+nonzero(2 & byte0_6high)
// then pos = 8*(pos1+0x80*pos2), buffer[pos:pos+8] = byte[0:8] (end exclusive)
//
// flags[0] = byte7 high
// flags[1] = byte15 high
// then pos = 16*(pos1+0x80*pos2), buffer[pos:pos+16] = byte[0:16] (end exclusive)

#include "definitions.h"
#include "util/functions.h"
#include "hid/display/oled.h"
#include "hid/display/numeric_driver.h"
#include "gui/ui_timer_manager.h"

__attribute__ ((aligned (CACHE_LINE_SIZE))) uint8_t module_data[64*1024];  // gimme some bss

static inline uint32_t __ac_X31_hash_string(const unsigned char *s, int len)
{
	if (!len) return 0;
	uint32_t h = (uint32_t)*s;
	for (int i = 1 ; i < len; ++i) {
		h = (h << 5) - h + (uint32_t)s[i];
	}
	return h;
}

void handle_sysex_module(uint8_t *s, int size) {
	if (size < 7) return; // F0 67 cmd pos1 pos2 x F7
	if (s[0] != 0xf0 or s[1] != 0x67) return;

	int pos = 16*(s[3]+0x80*s[4]);
	int x = s[5]; // not used with bytes!!
	int f = 0; // (1<<0) and (1<<1) reserved for bytes

	__attribute__ ((aligned (16))) char bytes[16];
	bool has_bytes = false;
	if (size >= 25) {
		has_bytes = true;
		memcpy(bytes,s+5,16);
		for (int i = 0; i < 7; i++) {
			bytes[i] += 0x80*((s[21] & (1 << i))?1:0);
			bytes[i+8] += 0x80*((s[22] & (1 << i))?1:0);
		}
		f = s[23];
		bytes[7] += 0x80*((f&1) ? 1 : 0);
		bytes[15] += 0x80*((f&2) ? 1 : 0);
	}
	int c = s[2];

	if (c == 0) {
		// write
		if (not has_bytes) return;

		if (pos+16 > sizeof(module_data)) return;  // just checking..

		memcpy(module_data+pos, bytes, 16);
		memcpy(module_data+UNCACHED_MIRROR_OFFSET+pos, bytes, 16);
	} else if (c <16) { // execute
		if ((!has_bytes) and size >= 8) f=s[6]; // F0 c pos1 pos2 x moff F7
		int moff = (f&12); // otherwise 0,4,8,12 offset within the 16-byte block
		auto ptr = (int (*)(uint8_t *, char *))(module_data+pos+moff);
		if (!has_bytes) bytes[0] = x; // as a treat
		int result = ptr(module_data, bytes);

		if (c == 2) {
			char buffer[16] = "the: ";
			intToString(result, buffer+5);
			numericDriver.displayPopup(buffer, 0);
		} else if (c == 3 or c == 4) {
			char *text = (char *)result;
			numericDriver.displayPopup(text, (c == 3) ? 0 : 3);
		}
	} else if (c == 17) { // pique
		char buffer[16] = "peek: ";
		intToString(module_data[pos+x], buffer+6);
		numericDriver.displayPopup(buffer, 0);
	} else if (c == 18) { // pos of piqee
		char buffer[16] = "ppos: ";
		intToString(pos+x, buffer+6);
		numericDriver.displayPopup(buffer, 0);
	} else if (c == 24) {
		if (!has_bytes) return;
		int *data = (int *)bytes;
		int pos = data[0], size = data[1];
		unsigned int hash = __ac_X31_hash_string(module_data+pos,size);
		char buffer[16] = "hash: ";
		intToString(hash, buffer+6);
		numericDriver.displayPopup(buffer, 0);
	} else if (c == 25) {
		uiTimerManager.unsetTimer(TIMER_MODULE);
		timer_module_cb = NULL;
	} else {
		numericDriver.displayPopup("u w0t m8", 0);
	}

	// sysexBuffer[sysexPos-1] = 0;
	// OLED::popupText((char *)(sysexBuffer+1), true);
}
