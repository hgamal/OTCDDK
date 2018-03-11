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

void listDevices(libusb_context *ctx);
libusb_device_handle *openDevice(libusb_context *c, int vendorId, int productId);
void closeDevice(libusb_device_handle *dev_handle);
void monitorPedal(libusb_device_handle *dev_handle);
void resetPedal(libusb_device_handle *dev_handle);
void setDebugVars(libusb_device_handle *dev_handle, uint32_t *values);
void programDSP(libusb_device_handle *dev, const char *file, uint16_t startAddress);
