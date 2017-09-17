/*
 * Copyright 2014 Google Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __DRIVERS_STORAGE_STREAM_H__
#define __DRIVERS_STORAGE_STREAM_H__

#include <stdint.h>

/*
 * StreamOps is a single-use stream. You open it from a StreamCtrlr with a
 * particular offset and size, and do successive reads to it until it is
 * exhausted. The offset and size are in terms of physical parameters for
 * the underlying medium and the size found in practice may be smaller,
 * e.g., due to skipping bad blocks on NAND.
 */
typedef struct StreamOps {
	uint64_t (*read)(struct StreamOps *me, uint64_t count,
			 void *buffer);
	void (*close)(struct StreamOps *me);
} StreamOps;

/* Lazily initialized representation of a device; factory for streams to
 * read part of the device */
typedef struct StreamCtrlr {
	/* Create a new StreamOps */
	StreamOps *(*open)(struct StreamCtrlr *me, uint64_t offset,
			uint64_t size);
	/* Get the size of the underlying device in bytes, i.e., the upper
	 * bound for offset+size in open() calls */
	uint64_t (*size)(struct StreamCtrlr *me);
} StreamCtrlr;

#endif /* __DRIVERS_STORAGE_STREAM_H__ */

