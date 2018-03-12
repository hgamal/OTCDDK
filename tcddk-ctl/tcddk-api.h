/*
    This file is part of OTCDDK.

	Copyright © 2008-2018 Haroldo Gamal <haroldo@gamal.com.br>
	
    OTCDDK is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OTCDDK is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OTCDDK.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#define TCDDK_ERROR_ERASE_BLOCK					-1
#define TCDDK_ERROR_ERASE_BLOCK_STATUS			-2
#define TCDDK_ERROR_TRANSFER_BLOCK				-3
#define TCDDK_ERROR_TRANSFER_BLOCK_STATUS		-4
#define TCDDK_ERROR_CHKSUM						-5


struct uint24 {
	uint8_t high;
	uint8_t mid;
	uint8_t low;

	operator uint32_t()	{ return (high << 16) + (mid << 8) + low; }

	uint24 &operator =(uint32_t value) {
		low  = (uint8_t) value & 0xff;
		mid  = (uint8_t) (value / 0x100) & 0xff;
		high = (uint8_t) (value / 0x10000) & 0xff;
		return *this;
	}

	uint24():high(0), mid(0), low(0) {}

	uint24(uint32_t value) {
		low  = (uint8_t) value & 0xff;
		mid  = (uint8_t) (value / 0x100) & 0xff;
		high = (uint8_t) (value / 0x10000) & 0xff;
	}
};

struct TcddkStatus {
	uint8_t pot0;
	uint8_t pot1;
	uint8_t pot2;
	uint8_t pot3;
	uint8_t pot4;
	uint8_t pot5;

	uint8_t switch0;
	uint8_t switch1;
	
	uint8_t topButton;
	uint8_t bottonButton;

	uint8_t redLed;
	uint8_t greeLed;

	uint24  debug0;
	uint24  debug1;
	uint24  debug2;
	uint24  debug3;
};

/** Line6 TCDDK USB API **/
int TCDDK_readStatus(libusb_device_handle *dev, uint8_t *buffer);
int TCDDK_setDebugVars(libusb_device_handle *dev, uint32_t *values);
int TCDDK_uploadBuffer(libusb_device_handle *dev, uint8_t *buffer, int size, uint16_t startAddress);