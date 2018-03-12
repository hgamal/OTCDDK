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
#include "tcddk-ctl.h"

void debug(const char *fmt, ...)
{
#ifdef _DEBUG
	va_list va;
	va_start(va, fmt);

	printf("DEBUG: ");

	vprintf(fmt, va);
	va_end(va);

	putchar('\n');
#endif
}

void error(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	printf("Error: ");

	vfprintf(stderr, fmt, va);
	va_end(va);

	fprintf(stderr, "\n");
}

void usage(const char *pgm)
{
	printf("usage: %s [list|monitor|set_debug|upload_dsp]\n", pgm);
	exit(0);
}

void printdev(libusb_device *dev) 
{
	libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0) {
		error("failed to get device descriptor");
		return;
	}

	printf("Configurations: %d - ", (int)desc.bNumConfigurations);
	printf("Class: %04x - ", desc.bDeviceClass);
	printf("VendorID: %04x - ", desc.idVendor);
	printf("ProductID: %04x - ", desc.idProduct);

	libusb_config_descriptor *config;
	libusb_get_config_descriptor(dev, 0, &config);
	printf("Interfaces: %d\n", config->bNumInterfaces);

/*
	libusb_device_handle *dev_handle;

	r = libusb_open(dev, &dev_handle);
	if (r < 0)
		printf("\n");
	else {
		unsigned char product[200];

		r = libusb_get_string_descriptor_ascii(dev_handle, desc.idProduct, product, sizeof(product)); 	
		if (r >= 0)
			printf("-????\n");
		libusb_close(dev_handle);
	}

	const libusb_interface *inter;
	const libusb_interface_descriptor *interdesc;
	const libusb_endpoint_descriptor *epdesc;
	for(int i=0; i<(int)config->bNumInterfaces; i++) {
		inter = &config->interface[i];
		printf("\tNumber of alternate settings: %d\n", inter->num_altsetting);
		for(int j=0; j<inter->num_altsetting; j++) {
			interdesc = &inter->altsetting[j];
			printf("\t\tInterface Number: %d\n", interdesc->bInterfaceNumber);
			printf("\t\tNumber of endpoints: %d\n", interdesc->bNumEndpoints);;
			for(int k=0; k<(int)interdesc->bNumEndpoints; k++) {
				epdesc = &interdesc->endpoint[k];
				printf("\t\t\tDescriptor Type: %x\n", epdesc->bDescriptorType);
				printf("\t\t\tEP Address: %x\n", epdesc->bEndpointAddress);
			}
		}
	}
*/
	libusb_free_config_descriptor(config);
}

void listDevices(libusb_context *ctx)
{
	libusb_device **devs;
	ssize_t cnt;

	libusb_set_debug(ctx, 3);

	cnt = libusb_get_device_list(ctx, &devs);
	if (cnt < 0)
		debug("Get Device Error");

	debug("%d Devices in list", cnt);
	
	for(ssize_t i = 0; i < cnt; i++) 
		printdev(devs[i]);
	
	libusb_free_device_list(devs, 1);
}


int main(int ac, char **av)
{
	if (ac == 0)
		usage(av[0]);

	libusb_context *ctx = NULL;
	int rc;

	rc = libusb_init(&ctx);
	if (rc < 0) {
		error( "Init Error %d",  rc);
		return 1;
	}

	libusb_device_handle *dev_handle = openDevice(ctx, 0x0e41, 0x4154);
	if (!dev_handle) {
		error( "Open Device Error");
		return 2;
	}

	rc = 0;
	if (ac > 1 && strcmp(av[1], "list") == 0)
		listDevices(ctx);
	else if (ac > 1 && strcmp(av[1], "monitor") == 0)
		monitorPedal(dev_handle);
	else if (ac > 5 && strcmp(av[1], "set_debug") == 0) {
		uint32_t values[4];
		for (int i=0; i<4; i++)
			values[i] = strtol(av[2+i], NULL, 16);

		setDebugVars(dev_handle, values);
	} else if (ac > 2	 && strcmp(av[1], "upload_dsp") == 0) {
		programDSP(dev_handle, av[2], 0xc000);
	} else
		usage(av[0]);

	closeDevice(dev_handle);

	libusb_exit(ctx);

	return rc;

}
