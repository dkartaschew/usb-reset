/*
 *   Copyright (C) 2009-2020 Darran Kartaschew
 *
 *   This file is part of the usbreset package.
 *
 *   usbreset is free software; you can redistribute it and/or modify
 *   it under the terms of the BSD License as included within the
 *   file 'COPYING' located in the root directory
 *
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <libusb-1.0/libusb.h>

/**
* List all devices to console.
*/
void listDevices() {
	libusb_device** udev_list = NULL;
	libusb_context* ctx = NULL;
	// Initialise context and get list of devices.
	libusb_init(&ctx);
	size_t udev_cnt = libusb_get_device_list(ctx, &udev_list);
	printf("Number of attached USB devices = %zu\n", udev_cnt);
	// Print all
	for (size_t i = 0; i < udev_cnt; i++) {
		libusb_device* dev = udev_list[i];
		struct libusb_device_descriptor desc;
		memset(&desc, 0, sizeof(desc));
		libusb_get_device_descriptor(dev, &desc);
		uint8_t busID = libusb_get_bus_number(dev);
		uint8_t portID = libusb_get_port_number(dev);
		uint8_t addr = libusb_get_device_address(dev);
		uint16_t vendorID = desc.idVendor;
		uint16_t productID = desc.idProduct;
		uint8_t classID = desc.bDeviceClass;
		printf("Bus: %d Port: %d Address: %d VID:PID = %04x:%04x Class: %x\n",
			   busID, portID, addr, vendorID, productID, classID);
	}
	// clean up
	libusb_free_device_list(udev_list, 1);
	libusb_exit(ctx);
}

/**
* Reset a single device.
*
* @param dev The device to reset.
* @return TRUE for success, FALSE for failed.
*/
int resetDevice(libusb_device* dev) {
	if (dev == NULL) {
		return false;
	}
	struct libusb_device_descriptor desc;
	memset(&desc, 0, sizeof(desc));
	libusb_get_device_descriptor(dev, &desc);
	printf("Reset device %04x:%04x\n", desc.idVendor, desc.idProduct);
	libusb_device_handle* handle = NULL;
	int ret = libusb_open(dev, &handle);
	if (ret) {
		fprintf(stderr, "ERR: failed to open device ");
		switch (ret) {
		case LIBUSB_ERROR_NO_MEM:
			fprintf(stderr, "(memory allocation failed)\n");
			break;
		case LIBUSB_ERROR_ACCESS:
			fprintf(stderr, "(no permission)\n");
			break;
		case LIBUSB_ERROR_NO_DEVICE:
			fprintf(stderr, "(udev not found)\n");
		}
		return false;
	}
	ret = libusb_reset_device(handle);
	libusb_close(handle);
	switch (ret) {
	case LIBUSB_ERROR_NOT_FOUND:
		fprintf(stderr, "WARN: Device reset successfully, however device re-enumeration required.\n");
		return true;
	case 0:
		return true;
	default:
		fprintf(stderr, "ERR: Reset failed with code %x\n", ret);
		return false;
	}
}

/**
* Locate the first instance of a device based on VID:PID
*
* @param ctx libusb context
* @param vendorID Te vendor ID to match
* @param productID The product ID to match
* @return The first device insranc
*/
libusb_device* locateDevice(libusb_context* ctx, const uint16_t vendorID, const uint16_t productID) {
	libusb_device** udev_list = NULL;
	size_t udev_cnt = libusb_get_device_list(ctx, &udev_list);
	libusb_device* device = NULL;
	// Locate device
	for (size_t i = 0; i < udev_cnt; i++) {
		libusb_device* dev = udev_list[i];
		struct libusb_device_descriptor desc;
		memset(&desc, 0, sizeof(desc));
		libusb_get_device_descriptor(dev, &desc);
		if (vendorID == desc.idVendor && productID == desc.idProduct) {
			device = dev;
			libusb_ref_device(device);
			break;
		}
	}
	// clean up
	libusb_free_device_list(udev_list, 1);
	return device;
}

/**
* Reset all non hub devices
*/
void resetAllDevices() {
	libusb_device** udev_list = NULL;
	libusb_context* ctx = NULL;
	// Initialise context and get list of devices.
	libusb_init(&ctx);
	size_t udev_cnt = libusb_get_device_list(ctx, &udev_list);
	// Print all
	for (size_t i = 0; i < udev_cnt; i++) {
		libusb_device* dev = udev_list[i];
		struct libusb_device_descriptor desc;
		memset(&desc, 0, sizeof(desc));
		libusb_get_device_descriptor(dev, &desc);
		// Only reset if not a HUB
		if (desc.bDeviceClass != LIBUSB_CLASS_HUB) {
			resetDevice(dev);
		}
	}
	// clean up
	libusb_free_device_list(udev_list, 1);
	libusb_exit(ctx);
}

/**
* Print help
*/
void printHelp() {
	printf("usage: " PACKAGE_NAME " [-ald]\n");
	printf("  -a : Reset all non Hub devices\n");
	printf("  -l : List all devices\n");
	printf("  -d vendorID:productID : Reset device with VID:PID. values must be hex.\n");
}

/**
* Reset the device based on the param
*
* @param param the CLI argument.
*/
void resetDevice0(const char* param) {
	libusb_context* ctx = NULL;
	uint16_t vendorID = 0;
	uint16_t productID = 0;
	int ret = sscanf(param, "%04hx:%04hx", &vendorID, &productID);
	if (ret != 2) {
		fprintf(stderr, "ERR: invalid vendorID:productID pair.\n");
		return;
	}
	// Initialise context and locate device.
	libusb_init(&ctx);
	libusb_device* dev = locateDevice(ctx, vendorID, productID);
	if (dev != NULL) {
		resetDevice(dev);
		libusb_unref_device(dev);
	} else {
		fprintf(stderr, "ERR: Unable to locate device %04x:%04x.\n", vendorID, productID);
	}
	// Clear context.
	libusb_exit(ctx);
}

int main(int argc, char* argv[]) {
	char c;
	int ret = 0;
	while ((c = getopt (argc, argv, "lahd:")) != -1) {
		switch (c) {
		case 'l':
			listDevices();
			return ret;
		case 'a':
			resetAllDevices();
			return ret;
		case 'h':
			printHelp();
			return 0;
		case 'd':
			resetDevice0(optarg);
			return 0;
		case '?':
			if (optopt == 'd') {
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			} else if (isprint (optopt)) {
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			} else {
				fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			}
			printHelp();
			return 1;
		}
	}
	printHelp();
	return ret;
}
