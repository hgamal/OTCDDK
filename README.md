# OTCDDK

# udev configuration unde linux
```
	# cat /etc/udev/rules.d/27-line6.rules
	#TCDDK
	SUBSYSTEM=="usb", ACTION=="add", ATTR{idVendor}=="0e41", ATTR{idProduct}=="4154", SYMLINK+="tcddk-%k", GROUP="line6"

	# udevadm control --reload-rules && udevadm trigger
```
# tcddk-ctl

# libusb windows
Download libraries and sources from
	http://libusb.info/

## help
```
[hgamal@ygamal tcddk-ctl]$ ./tcddk-ctl 
usage: ./tcddk-ctl [list|monitor|reset|set_debug|upload_dsp]
```

## monitor
```
[hgamal@ygamal tcddk-ctl]$ ./tcddk-ctl monitor
0024r: p0=000, p1=000, p2=000, p3=000, p4=000, p5=000, s0=0, s1=0, tb=0, bb=0, gl=0, rl=0, dbg0=000000, dbg1=000000, dbg2=000000, dbg3=000000
```

- p0-p6: Potentiometer positions - values from 0 to 255

- s0-s1: Switches - values from 0, 1 and 2 (left, mid rigth)

- tb-bb: Top and Botton Switches - values 0 and 1

- gl-rl: Leds (green and red) - values 0 and 1

- dbg0 - dbg1: debug variables 24 bit resolution


# USB Vendor calls


## Version ???
	bmRequestType: 0xc0, bRequest: 67, wValue: 0x0000, wIndex: 0 (0x0000), wLength: 6
	
	c0:00:cf:ff:06:00

## Set debug variables to 800001800002800003800004
	bmRequestType: 0x40, bRequest: 65, wValue: 0x0000, wIndex: 0 (0x0000), wLength: 12, Data Fragment: 800001800002800003800004

## Enable Write - Erase ???
	bmRequestType: 0x40, bRequest: 99, wValue: 0xc000, wIndex: (c000, c200, c400, c600, c800, ca00, cc00, ce00) wLength: 1, Data Fragment: 00
		01

		??? Program Status after ???

## Status ???
	bmRequestType: 0xc0, bRequest: 105, wValue: 0x0000, wIndex: 0 (0x0000), wLength: 1
		01

## Program bytes
	bmRequestType: 0x40, bRequest: 97, wValue: start, wIndex: end, wLength: len, Data Fragment: bytes to program
		01

## Reset - Reboot DSP
	bmRequestType: 0xc0, bRequest: 66, wValue: 0xdbdb, wIndex: 0 (0x0000), wLength: 1
		01

## Read Status from DSP
	bmRequestType: 0xc0, bRequest: 64, wValue: 0x0000, wIndex: 0 (0x0000), wLength: 24

	00 00 00 00 00 00 00 00 00 00 00 00 01 07 89 01 07 89 7F FF FF 7F FF

	00: Pot 0
	01: Pot 1
	02: Por 2

# Upload life cycle

Inicio:
	Frame 11211: 65 bytes on wire (520 bits), 65 bytes captured (520 bits) on interface 0	- 0xc000
	Frame 11215: 65 bytes on wire (520 bits), 65 bytes captured (520 bits) on interface 0	- 0xc200
	Frame 11219: 65 bytes on wire (520 bits), 65 bytes captured (520 bits) on interface 0	- 0xc400
	Frame 11223: 65 bytes on wire (520 bits), 65 bytes captured (520 bits) on interface 0	- 0xc600
	Frame 11227: 65 bytes on wire (520 bits), 65 bytes captured (520 bits) on interface 0	- 0xc800
	Frame 11231: 65 bytes on wire (520 bits), 65 bytes captured (520 bits) on interface 0	- 0xca00
	Frame 11235: 65 bytes on wire (520 bits), 65 bytes captured (520 bits) on interface 0	- 0xcc00
	Frame 11239: 65 bytes on wire (520 bits), 65 bytes captured (520 bits) on interface 0	- 0xce00

	Frame 11243: 72 bytes on wire (576 bits), 72 bytes captured (576 bits) on interface 0	- Write 0xc400 - 0xc408
		00:01:60:00:00:00:0a:f0

	Frame 11247: 72 bytes on wire (576 bits), 72 bytes captured (576 bits) on interface 0	- Write 0xc408 - 0xc410
		80:00:00:4e:0c:00:02:00

	Frame 11251: 72 bytes on wire (576 bits), 72 bytes captured (576 bits) on interface 0	- Write 0xc410 - 0xc418
		00:00:0c:00:04:00:00:00

	Frame 11255: 72 bytes on wire (576 bits), 72 bytes captured (576 bits) on interface 0	- Write 0xc420 - 0xc428
		0c:00:06:00:00:00:0c:00

	... 132 transferências de 8 bytes => tomam 4 frames cada uma assim 11243 + 132 * 4 = 11771

	Frame 11771: 70 bytes on wire (560 bits), 70 bytes captured (560 bits) on interface 0	- Write 0xc420 - 0xc426 
		56:1f:00:00:00:0c

	Frame 11775: 64 bytes on wire (512 bits), 64 bytes captured (512 bits) on interface 0	- Reset

```
Mar  6 23:41:53 ygamal kernel: usb 8-2: new full-speed USB device number 2 using xhci_hcd
Mar  6 23:41:53 ygamal kernel: usb 8-2: New USB device found, idVendor=0e41, idProduct=4154
Mar  6 23:41:53 ygamal kernel: usb 8-2: New USB device strings: Mfr=1, Product=1, SerialNumber=1
Mar  6 23:41:53 ygamal kernel: usb 8-2: Product: Line 6 ToneCore DSP Developer Kit
Mar  6 23:41:53 ygamal kernel: usb 8-2: Manufacturer: Line 6 ToneCore DSP Developer Kit
Mar  6 23:41:53 ygamal kernel: usb 8-2: SerialNumber: Line 6 ToneCore DSP Developer Kit
Mar  6 23:41:53 ygamal systemd-udevd[477]: Invalid rule /etc/udev/rules.d/25-pemicro.rules:1: unknown key 'BUS'
Mar  6 23:41:53 ygamal systemd-udevd[477]: Invalid rule /etc/udev/rules.d/25-pemicro.rules:3: unknown key 'SYSFS{idVendor}'
Mar  6 23:41:53 ygamal systemd-udevd[477]: Invalid rule /etc/udev/rules.d/25-pemicro.rules:4: unknown key 'SYSFS{idVendor}'
Mar  6 23:41:53 ygamal systemd-udevd[477]: Invalid rule /etc/udev/rules.d/25-pemicro.rules:5: unknown key 'SYSFS{idVendor}'
Mar  6 23:41:53 ygamal systemd-udevd[477]: Invalid rule /etc/udev/rules.d/25-pemicro.rules:6: unknown key 'SYSFS{idVendor}'
Mar  6 23:41:53 ygamal systemd-udevd[477]: Invalid rule /etc/udev/rules.d/25-pemicro.rules:7: unknown key 'SYSFS{idVendor}'
Mar  6 23:41:53 ygamal systemd-udevd[477]: Invalid rule /etc/udev/rules.d/25-pemicro.rules:8: unknown key 'SYSFS{idVendor}'
Mar  6 23:41:53 ygamal systemd-udevd[477]: Invalid rule /etc/udev/rules.d/25-pemicro.rules:9: unknown key 'SYSFS{idVendor}'
Mar  6 23:41:53 ygamal systemd-udevd[477]: Invalid rule /etc/udev/rules.d/25-pemicro.rules:10: unknown key 'SYSFS{idVendor}'
Mar  6 23:41:53 ygamal systemd-udevd[477]: Invalid rule /etc/udev/rules.d/45-jlink.rules:1: unknown key 'BUS'
Mar  6 23:41:53 ygamal systemd-udevd[477]: Invalid rule /etc/udev/rules.d/45-jlink.rules:3: unknown key 'SYSFS{idProduct}'
Mar  6 23:41:53 ygamal systemd-udevd[477]: Invalid rule /etc/udev/rules.d/45-jlink.rules:4: unknown key 'SYSFS{idProduct}'
Mar  6 23:41:53 ygamal systemd-udevd[477]: Invalid rule /etc/udev/rules.d/45-jlink.rules:5: unknown key 'SYSFS{idProduct}'
Mar  6 23:41:53 ygamal systemd-udevd[477]: Invalid rule /etc/udev/rules.d/45-jlink.rules:6: unknown key 'SYSFS{idProduct}'

Bus 008 Device 002: ID 0e41:4154 Line6, Inc. 
Device Descriptor:
  bLength                18
  bDescriptorType         1
  bcdUSB               1.01
  bDeviceClass            0 
  bDeviceSubClass         0 
  bDeviceProtocol         0 
  bMaxPacketSize0        16
  idVendor           0x0e41 Line6, Inc.
  idProduct          0x4154 
  bcdDevice            1.00
  iManufacturer           1 Line 6 ToneCore DSP Developer Kit
  iProduct                1 Line 6 ToneCore DSP Developer Kit
  iSerial                 1 Line 6 ToneCore DSP Developer Kit
  bNumConfigurations      1
  Configuration Descriptor:
    bLength                 9
    bDescriptorType         2
    wTotalLength           25
    bNumInterfaces          1
    bConfigurationValue     1
    iConfiguration          0 
    bmAttributes         0x80
      (Bus Powered)
    MaxPower                0mA
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        0
      bAlternateSetting       0
      bNumEndpoints           1
      bInterfaceClass       255 Vendor Specific Class
      bInterfaceSubClass      0 
      bInterfaceProtocol    255 
      iInterface              0 
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x81  EP 1 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0040  1x 64 bytes
        bInterval             255
Device Status:     0x0040
  (Bus Powered)
  Debug Mode


usb.addr == "8.2.0" && usb.bmRequestType != 0xc0
```
