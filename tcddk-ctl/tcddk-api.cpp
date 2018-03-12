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

#include <stdio.h>

#pragma warning(disable:4200)
#include <libusb-1.0/libusb.h>

#include "tcddk-api.h"

static unsigned char *copyWord24(unsigned long value, unsigned char *ptr)
{
	*ptr++ = (unsigned char) ((value & 0xff0000) >> 16);
	*ptr++ = (unsigned char) ((value & 0xff00) >> 8);
	*ptr++ = (unsigned char) ((value & 0xff));

	return ptr;
}

/** Line6 TCDDK USB API **/

int TCDDK_readStatus(libusb_device_handle *dev, uint8_t *buffer)
{
	int rc = libusb_control_transfer(dev, 0xc0, 64, 0, 0, buffer, 24, 0);
	return rc;
}

int TCDDK_setDebugVars(libusb_device_handle *dev, uint32_t values[4])
{
	unsigned char buffer[12];
	unsigned char *ptr = buffer;
	for (int i=0; i<4; i++)
		ptr = copyWord24(values[i], ptr);

	int rc = libusb_control_transfer(dev, 0x40, 65, 0x0000, 0, buffer, sizeof(buffer), 0);
	return rc;	
}

int TCDDK_uploadBuffer(libusb_device_handle *dev, uint8_t *buffer, int size, uint16_t startAddress)
{
	uint8_t *p = buffer;
	int count = size;
	int rc_count = 0;
	int rc;
	uint8_t status;
	uint16_t sum=0;

	// erase blocks
	for (uint16_t x=0xc000; x<0xcf00; x+=0x200) {
		uint8_t data = 0;
		rc = libusb_control_transfer(dev, 0x40, 99, x, 0, &data, sizeof(data), 0);
#ifdef _DEBUG
		printf("zero: addr=%04x, rc=%d\n", x, rc);
#endif
		if (rc != 1)
			return TCDDK_ERROR_ERASE_BLOCK;

		// Get Status
		rc = libusb_control_transfer(dev, 0xc0, 105, 0x0000, 0, &status, sizeof(status), 0);
#ifdef _DEBUG
		printf("zero: status: rc=%d, status=%d\n", rc, status);
#endif

		if (rc != 1 && status != 1)
			return TCDDK_ERROR_ERASE_BLOCK_STATUS;
	}

	// program data
	while (count) {
		int chunkSize = count > 8 ? 8 : count;
		uint16_t addrInit = startAddress;
		uint16_t addrEnd = addrInit + chunkSize; 

		// Calculate checksum
		for (int i=0; i<chunkSize; i++)
			sum += p[i];		

		// Send buffer
		rc = libusb_control_transfer(dev, 0x40, 97, addrInit, addrEnd, p, chunkSize, 0);
#ifdef _DEBUG
		printf("upload: addr=%04x, end=%04x, chunk size=%d, rc=%d,", addrInit, addrEnd, chunkSize, rc);
#endif
		if (rc != chunkSize)
			return TCDDK_ERROR_TRANSFER_BLOCK;

#ifdef _DEBUG
		for (int i=0; i<chunkSize; i++)
			printf(" %02x", p[i]);
#endif

		// Get Status
		rc = libusb_control_transfer(dev, 0xc0, 105, 0x0000, 0, &status, sizeof(status), 0);
#ifdef _DEBUG
		printf(", status: rc=%d, status=%d\n", rc, status);
#endif
		if (rc != 1 && status != 1)
			return TCDDK_ERROR_TRANSFER_BLOCK_STATUS;

		p += chunkSize;
		count -= chunkSize;
		rc_count += chunkSize;
		startAddress += chunkSize;
	}

#ifdef _DEBUG
	printf("Code sum=%d, %x\n", sum, sum);
#endif

	// verify checksum and reset
	rc = libusb_control_transfer(dev, 0xc0, 66, sum, 0, &status, sizeof(status), 0);
#ifdef _DEBUG
	printf("Upload status: rc=%d, status=%d\n", rc, status);
#endif

	if (rc != 1 && status != 1)
		return TCDDK_ERROR_CHKSUM;

	return rc_count;	
}
