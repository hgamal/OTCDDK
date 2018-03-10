#include <stdio.h>
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

int TCDDK_resetPedal(libusb_device_handle *dev, uint8_t *status)
{
	printf("sizeof=%ld\n", sizeof(*status));
	int rc = libusb_control_transfer(dev, 0xc0, 66, 0xdbdb, 0, status, sizeof(*status), 0);
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

	while (count) {
		int chunkSize = count > 8 ? 8 : count;
		uint16_t addrInit = startAddress;
		uint16_t addrEnd = addrInit + chunkSize; 

		// Calculate checksum
		for (int i=0; i<chunkSize; i++)
			sum += p[i];		

		// Send buffer
		rc = libusb_control_transfer(dev, 0x40, 97, addrInit, addrEnd, p, chunkSize, 0);
		printf("upload: addr=%04x, end=%04x, chunk size=%d, rc=%d,", addrInit, addrEnd, chunkSize, rc);
		for (int i=0; i<chunkSize; i++)
			printf(" %02x", p[i]);

		// Get Status
		rc = libusb_control_transfer(dev, 0xc0, 105, 0x0000, 0, &status, sizeof(status), 0);
		printf(", status: rc=%d, status=%d\n", rc, status);

		p += chunkSize;
		count -= chunkSize;
		rc_count += chunkSize;
		startAddress += chunkSize;
	}

	printf("Code sum=%d, %x\n", sum, sum);

	rc = libusb_control_transfer(dev, 0xc0, 66, sum, 0, &status, sizeof(status), 0);
	printf("Upload status: rc=%d, status=%d\n", rc, status);

	return rc_count;	
}
