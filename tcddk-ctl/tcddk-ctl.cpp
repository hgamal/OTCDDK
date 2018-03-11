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
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#pragma warning(disable:4200)
#include <libusb-1.0/libusb.h>

#include "main.h"
#include "tcddk-api.h"

/** USB operations **/
libusb_device_handle *openDevice(libusb_context *ctx, int vendorId, int productId)
{
	int rc;

	libusb_device_handle *dev_handle = libusb_open_device_with_vid_pid(ctx, vendorId, productId); //these are vendorID and productID I found for my usb device

	if (dev_handle == NULL) {
		error("Cannot open device");
		return NULL;
	}

	debug("Device Opened");
	

	if (libusb_kernel_driver_active(dev_handle, 0) == 1) {
		debug("Kernel Driver Active");
		if (libusb_detach_kernel_driver(dev_handle, 0) == 0)
			debug("Kernel Driver Detached!");
	}

	rc = libusb_claim_interface(dev_handle, 0);
	if (rc < 0) {
		error( "Cannot Claim Interface");
		return NULL;
	}
	debug("Claimed Interface");

	return dev_handle;
}

void closeDevice(libusb_device_handle *dev_handle)
{
	int rc = libusb_release_interface(dev_handle, 0); //release the claimed interface
	if (rc != 0)
		error("Cannot Release Interface");
}

void monitorPedal(libusb_device_handle *dev_handle)
{
		unsigned char buf[24];

		while (true) {
			int rc = TCDDK_readStatus(dev_handle, buf);
			printf("%04d: ", rc);

			TcddkStatus *s = (TcddkStatus *) buf;

			//for (int i=0; i<rc; i++)
			//	printf("%02X ", buf[i]);
			printf("p0=%03d, p1=%03d, p2=%03d, p3=%03d, p4=%03d, p5=%03d, "
				   "s0=%1d, s1=%1d, tb=%1d, bb=%1d, gl=%1d, rl=%1d, "
				   "dbg0=%06x, dbg1=%06x, dbg2=%06x, dbg3=%06x"
				   "\r",
				s->pot0, s->pot1, s->pot2, s->pot3, s->pot5, s->pot5,
				s->switch0, s->switch1, s->topButton, s->bottonButton,
				s->greeLed, s->redLed,
				(uint32_t) s->debug0, (uint32_t) s->debug1, 
				(uint32_t) s->debug2, (uint32_t)s->debug3
			);

			putchar('\r');
		}

		putchar('\n');
}

void resetPedal(libusb_device_handle *dev_handle)
{
		unsigned char status;

		int rc = TCDDK_resetPedal(dev_handle, &status);
		printf("reset rc=%d, ret_code=%x\n", rc, status);
}

void setDebugVars(libusb_device_handle *dev_handle, uint32_t *values)
{
	int rc = TCDDK_setDebugVars(dev_handle, values);
	printf("rc=%d\n", rc);
}

static uint8_t *readHexFile(const char *file, int *size, uint16_t addr)
{
	FILE *fp = fopen(file, "r");
	if (!fp) {
		error("can not open %s", file);
		return NULL;
	}

	char buffer[1024];
	static uint8_t outputBuffer[16000];	// Tone Core have no more than 2000 bytes of DSP memory  

	memset(outputBuffer, 0, sizeof(outputBuffer));

	uint8_t *o = outputBuffer + 6;			// save space for size

	while (char *p = fgets(buffer, sizeof(buffer), fp)) {
		char *q;
		q = strtok(p, "\t\r\n, ");
		while (q && *q) {
			if (q[0] == '0' && q[1] == 'x')
				q += 2;

			uint8_t value = (uint8_t) strtol(q, NULL, 16);

			*o++ = value;

			q = strtok(NULL, "\t\r\n, ");
			(*size)++;
		}
	}

	debug("Code size=%d, %d words", (*size), (*size)/3);

	uint32_t nwords = (*size)/3;

	outputBuffer[0] = (nwords >> 16) & 0xff;
	outputBuffer[1] = (nwords >> 8) & 0xff;
	outputBuffer[2] = nwords & 0xff;
	outputBuffer[3] = 0;
	outputBuffer[4] = 0;
	outputBuffer[5] = 0;

	(*size) += 6;

	debug("Upload size=%d", (*size));
/*
	for (int i=0; i<(*size); i++) {
		if (i % 16 == 0)
			printf("\nvalue [%04x] = %02x", addr, outputBuffer[i]);
		else
			printf(" %02x", outputBuffer[i]);
		addr++;
	}

	printf("\n");
*/
	fclose(fp);

	return outputBuffer;
}

void programDSP(libusb_device_handle *dev, const char *file, uint16_t startAddress)
{
	int size = 0;

	uint8_t *buffer = readHexFile(file, &size, startAddress);

	int rc = TCDDK_uploadBuffer(dev, buffer, size, startAddress);
	printf("upload rc=%d\n", rc);
}