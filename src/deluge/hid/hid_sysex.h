#include "definitions.h"

namespace HIDSysex {
	void requestOLEDDisplay(int ip, int d, int cable, uint8_t* data, int len);
	void sysexReceived(int ip, int d, int cable, uint8_t* data, int len);
	void sendOLEDData(int ip, int d, int cable, int blk);
	void sendOLEDDataAll(int ip, int d, int cable, bool rle);

}
