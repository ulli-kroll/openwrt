/*
 * Copyright (C) 2016 Felix Fietkau <nbd@nbd.name>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __FWIMAGE_H
#define __FWIMAGE_H

#include <stdint.h>

#define FWIMAGE_MAGIC		0x46577830 /* FWx0 */
#define UIMAGE_HEADER_MAGIC 0x27051956

typedef struct image_header {
	uint32_t ih_magic; /* Image Header Magic Number*/
	uint32_t ih_hcrc; /* Image Header CRC Checksum*/
	uint32_t ih_time; /* Image Creation Timestamp*/
	uint32_t ih_size; /* Image Data sizeof*/
	uint32_t ih_load; /* Data Load  Address*/
	uint32_t ih_ep; /* Entry Point Address*/
	uint32_t ih_dcrc; /* Image Data CRC Checksum*/
	uint8_t ih_os; /* Operating system.*/
	uint8_t ih_arch; /* CPU architecture*/
	uint8_t ih_type; /* Image type.*/
	uint8_t ih_comp; /* Compression type.*/
	uint8_t ih_name[32]; /* Image Name*/
} image_header_t;

struct fwimage_header {
	uint32_t version;
	uint32_t flags;
	char data[];
};

struct fwimage_trailer {
	uint32_t magic;
	uint32_t crc32;
	uint8_t type;
	uint8_t __pad[3];
	uint32_t size;
};

enum fwimage_type {
	FWIMAGE_SIGNATURE,
	FWIMAGE_INFO,
};

#endif
