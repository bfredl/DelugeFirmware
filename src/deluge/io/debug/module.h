#include "io/midi/midi_device.h"

class LoadableModule {
	public:
	virtual void activate() = 0;
	virtual void unload() {};
	virtual void sysex(MIDIDevice* device, uint8_t* data, int32_t len) {};
};

extern LoadableModule *loadable_module;
