# OTCDDK
OTCDDK means "Other Tone Core DSP Development Kit". It was enginered in order to permit develop DSP algorithms in other platforms such linux or Windows.

# Install and run on Windows
1. install git
2. if you want to compile dsp codes:
   1. download "make" from "https://sourceforge.net/projects/ezwinports/files/"
   2. install "make"  - downloaded above - into git mingw directory (mine is under C:\Program Files\Git\mingw32)
3. clone https://github.com/hgamal/OTCDDK.git
4. start git bash session under cloned directory


# tasks
## help
```
$ ./bin/tcddk-ctl
usage: ./tcddk-ctl [list|monitor|set_debug|upload_dsp]
```
## compile example dsp code: cd dsp-code/
```
$ cd dsp-code/TwoBandStereoEQ
$ make
```
## upload code to DSP
```
$ ./bin/cld2hex cld-exemples/TwoBandStereoEQ.cld
$ ./bin/tcddk-ctl upload_dsp cld-exemples/TwoBandStereoEQ.hex
Upload size=1101 (367 words)
Upload Checksum=e557
Upload size=1107
Upload Checksum=cba14
1107 bytes transfered
```
## monitor DSP running (pots and switches)
```
$ ./bin/tcddk-ctl monitor
0024: p0=000, p1=000, p2=000, p3=000, p4=000, p5=000, s0=0, s1=0, tb=0, bb=0, gl=0, rl=0, dbg0=000000, dbg1=000000, dbg2=000000, dbg3=000000
```

- p0-p6: Potentiometer positions - values from 0 to 255

- s0-s1: Switches - values from 0, 1 and 2 (left, mid rigth)

- tb-bb: Top and Botton Switches - values 0 and 1

- gl-rl: Leds (green and red) - values 0 and 1

- dbg0 - dbg1: debug variables 24 bit resolution

## to upload code to mcu
not available yet

# udev configuration on linux
```
	# cat /etc/udev/rules.d/27-line6.rules
	#TCDDK
	SUBSYSTEM=="usb", ACTION=="add", ATTR{idVendor}=="0e41", ATTR{idProduct}=="4154", SYMLINK+="tcddk-%k", GROUP="line6"

	# udevadm control --reload-rules && udevadm trigger
```
# libusb windows
It is needed in order to compile tcddk-ctl.

Download libraries and sources from
	http://libusb.info/

## Installing WinUSB
In order to make tcddk-ctl work, the Tone COre Developer Line6 Interface must to be associated to Microsoft Winusb.sys. 
The follow these instructions to load the driver:

1. Plug in your device to the host system
2. Open Device Manager and locate the device
3. Right-click the device and select "Update driver software"
4. from the context menu. In the wizard, select "Browse my computer" for driver software
5. Select "Let me pick from a list of device drivers on my computer"
6. From the list of device classes, select "Universal Serial Bus devices"
7. The wizard displays WinUsb Device. Select it to load the driver.
	
# USB Vendor calls

## Get Version ?
	bmRequestType: 0xc0, bRequest: 67, wValue: 0x0000, wIndex: 0 (0x0000), wLength: 6
	return 6 bytes: c0:00:cf:ff:06:00

## Set debug variables to 800001800002800003800004
	bmRequestType: 0x40, bRequest: 65, wValue: 0x0000, wIndex: 0 (0x0000), wLength: 12, Data Fragment: 800001800002800003800004

## Erase Block
	bmRequestType: 0x40, bRequest: 99, wValue: 0xc000, wIndex: (c000, c200, c400, c600, c800, ca00, cc00, ce00) wLength: 1, Data Fragment: 00

## Get status
	bmRequestType: 0xc0, bRequest: 105, wValue: 0x0000, wIndex: 0 (0x0000), wLength: 1
	return 1 byte: 01 == success

## Program bytes
	bmRequestType: 0x40, bRequest: 97, wValue: start, wIndex: end, wLength: len, Data Fragment: bytes to program
	return 1 byte: 01 == success

## Send Checksum
	bmRequestType: 0xc0, bRequest: 66, wValue: {sum}, wIndex: 0 (0x0000), wLength: 1
	return 1 byte: 01 == success

## Read Status from DSP
	bmRequestType: 0xc0, bRequest: 64, wValue: 0x0000, wIndex: 0 (0x0000), wLength: 24
	return 64 bytes: pots, switches, buttons and debug variables
