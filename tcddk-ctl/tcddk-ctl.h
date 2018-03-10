#pragma once

void listDevices(libusb_context *ctx);
libusb_device_handle *openDevice(libusb_context *c, int vendorId, int productId);
void closeDevice(libusb_device_handle *dev_handle);
void monitorPedal(libusb_device_handle *dev_handle);
void resetPedal(libusb_device_handle *dev_handle);
void setDebugVars(libusb_device_handle *dev_handle, uint32_t *values);
void programDSP(libusb_device_handle *dev, const char *file, uint16_t startAddress);
