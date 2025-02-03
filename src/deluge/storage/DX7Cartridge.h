/**
 *
 * Copyright (c) 2014-2016 Pascal Gauthier.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 */

#pragma once

#include "gui/l10n/strings.h"
#include "util/misc.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <span>

std::byte sysexChecksum(std::span<std::byte> sysex);
void exportSysexPgm(uint8_t* dest, uint8_t* src);

constexpr std::array kSysexHeader = {0xF0_b, 0x43_b, 0x00_b, 0x09_b, 0x20_b, 0x00_b};
constexpr std::size_t kSysexSize = 4104;
constexpr std::size_t kSmallSysexSize = 163;

class DX7Cartridge {
	std::array<std::byte, kSysexSize> voiceData;

	void setHeader() {
		std::ranges::copy(kSysexHeader, voiceData.begin());
		voiceData[4102] = sysexChecksum({&voiceData[6], 4096});
		voiceData[4103] = std::byte{0xF7};
	}

public:
	DX7Cartridge() = default;

	static void normalizePgmName(char buffer[11], const char* sysexName) {
		memcpy(buffer, sysexName, 10);

		for (int j = 0; j < 10; j++) {
			char c = (unsigned char)buffer[j];
			c &= 0x7F; // strip don't care most-significant bit from name
			switch (c) {
			case 92:
				c = 'Y';
				break; /* yen */
			case 126:
				c = '>';
				break; /* >> */
			case 127:
				c = '<';
				break; /* << */
			default:
				if (c < 32 || c > 127)
					c = 32;
				break;
			}
			buffer[j] = c;
		}
		buffer[10] = 0;

		// trim spaces at the end
		for (int j = 9; j >= 0; j--) {
			if (buffer[j] != 32) {
				break;
			}
			buffer[j] = 0;
		}
	}
	/**
	 * Loads sysex buffer
	 * Returns EMPTY_STRING if it was parsed successfully
	 * otherwise a string describing the error.
	 */
	deluge::l10n::String load(std::span<std::byte> stream) {
		using deluge::l10n::String;
		const std::byte* pos = stream.data();

		size_t minMsgSize = 163;

		if (stream.size() < minMsgSize) {
			std::ranges::copy(stream, &voiceData[6]);
			return String::STRING_FOR_DX_ERROR_FILE_TOO_SMALL;
		}

		if (pos[0] != std::byte{0xF0}) {
			// it is not, just copy the first 4096 bytes
			std::copy_n(pos, 4096, &voiceData[6]);
			return String::STRING_FOR_DX_ERROR_NO_SYSEX_START;
		}

		size_t i;
		// check if this is the size of a DX7 sysex cartridge
		for (i = 0; i < stream.size(); ++i) {
			if (pos[i] == std::byte{0xF7}) {
				break;
			}
		}
		if (i == stream.size()) {
			return String::STRING_FOR_DX_ERROR_NO_SYSEX_END;
		}

		int msgSize = i + 1;

		if (msgSize != kSysexSize && msgSize != kSmallSysexSize) {
			return String::STRING_FOR_DX_ERROR_INVALID_LEN;
		}

		std::copy_n(pos, msgSize, voiceData.begin());
		size_t dataSize = (msgSize == kSysexSize) ? 4096 : 155;
		if (sysexChecksum({&voiceData[6], dataSize}) != pos[msgSize - 2]) {
			return String::STRING_FOR_DX_ERROR_CHECKSUM_FAIL;
		}

		if (voiceData[1] != std::byte{67} || (voiceData[3] != std::byte{9} && voiceData[3] != std::byte{0})) {
			return String::STRING_FOR_DX_ERROR_SYSEX_ID;
		}

		return String::EMPTY_STRING;
	}

	bool isCartridge() { return voiceData[3] == std::byte{9}; }
	int numPatches() { return isCartridge() ? 32 : 1; }

	std::span<const std::byte> saveVoice() {
		setHeader();
		return voiceData;
	}

	std::byte* getRawVoice() { return &voiceData[6]; }

	void getProgramNames(char dest[32][11]) { // 10 chars + NUL
		for (int i = 0; i < numPatches(); i++) {
			getProgramName(i, dest[i]);
		}
	}

	void getProgramName(int32_t i, char dest[11]) {
		normalizePgmName(dest,
		                 reinterpret_cast<const char*>(getRawVoice() + ((i * 128) + (isCartridge() ? 118 : 145))));
	}

	void unpackProgram(std::span<std::uint8_t>, size_t idx);
	void packProgram(uint8_t* src, int idx, char* name, char* opSwitch);
};
