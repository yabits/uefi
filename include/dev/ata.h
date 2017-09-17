/*
 * Copyright 2012 Google Inc.
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

#ifndef __DRIVERS_STORAGE_ATA_H__
#define __DRIVERS_STORAGE_ATA_H__

typedef enum AtaCommand {
	ATA_CMD_NOP = 0x00,
	ATA_CMD_CFA_REQUEST_EXTENDED_ERROR = 0x03,
	ATA_CMD_DEVICE_RESET = 0x08,
	ATA_CMD_READ_SECTORS = 0x20,
	ATA_CMD_READ_SECTORS_EXT = 0x24,
	ATA_CMD_READ_DMA_EXT = 0x25,
	ATA_CMD_READ_DMA_QUEUED_EXT = 0x26,
	ATA_CMD_READ_NATIVE_MAX_ADDRESS_EXT = 0x27,
	ATA_CMD_READ_MULTIPLE_EXT = 0x29,
	ATA_CMD_READ_STREAM_DMA_EXT = 0x2a,
	ATA_CMD_READ_STREAM_EXT = 0x2b,
	ATA_CMD_READ_LOG_EXT = 0x2f,
	ATA_CMD_WRITE_SECTORS = 0x30,
	ATA_CMD_WRITE_SECTORS_EXT = 0x34,
	ATA_CMD_WRITE_DMA_EXT = 0x35,
	ATA_CMD_WRITE_DMA_QUEUED_EXT = 0x36,
	ATA_CMD_SET_MAX_ADDRESS_EXT = 0x37,
	ATA_CMD_CFA_WRITE_SECTORS_WITHOUT_ERASE = 0x38,
	ATA_CMD_WRITE_MULTIPLE_EXT = 0x39,
	ATA_CMD_WRITE_STREAM_DMA_EXT = 0x3a,
	ATA_CMD_WRITE_STREAM_EXT = 0x3b,
	ATA_CMD_WRITE_DMA_FUA_EXT = 0x3d,
	ATA_CMD_WRITE_DMA_QUEUED_FUA_EXT = 0x3e,
	ATA_CMD_WRITE_LOG_EXT = 0x3f,
	ATA_CMD_READ_VERIFY_SECTORS = 0x40,
	ATA_CMD_READ_VERIFY_SECTORS_EXT = 0x42,
	ATA_CMD_WRITE_UNCORRECTABLE_EXT = 0x45,
	ATA_CMD_READ_LOG_DMA_EXT = 0x47,
	ATA_CMD_CONFIGURE_STREAM = 0x51,
	ATA_CMD_WRITE_LOG_DMA_EXT = 0x57,
	ATA_CMD_TRUSTED_RECEIVE = 0x5c,
	ATA_CMD_TRUSTED_RECEIVE_DMA = 0x5d,
	ATA_CMD_TRUSTED_SEND = 0x5e,
	ATA_CMD_TRUSTED_SEND_DMA = 0x5f,
	ATA_CMD_CFA_TRANSLATE_SECTOR = 0x87,
	ATA_CMD_EXECUTE_DEVICE_DIAGNOSTIC = 0x90,
	ATA_CMD_DOWNLOAD_MICROCODE = 0x92,
	ATA_CMD_PACKET = 0xa0,
	ATA_CMD_IDENTIFY_PACKET_DEVICE = 0xa1,
	ATA_CMD_SERVICE = 0xa2,
	ATA_CMD_SMART = 0xb0,
	ATA_CMD_DEVICE_CONFIGURATION_OVERLAY = 0xb1,
	ATA_CMD_NV_CACHE = 0xb6,
	ATA_CMD_CFA_ERASE_SECTORS = 0xc0,
	ATA_CMD_READ_MULTIPLE = 0xc4,
	ATA_CMD_WRITE_MULTIPLE = 0xc5,
	ATA_CMD_SET_MULTIPLE_MODE = 0xc6,
	ATA_CMD_READ_DMA_QUEUED = 0xc7,
	ATA_CMD_READ_DMA = 0xc8,
	ATA_CMD_WRITE_DMA = 0xca,
	ATA_CMD_WRITE_DMA_QUEUED = 0xcc,
	ATA_CMD_CFA_WRITE_MULTIPLE_WITHOUT_ERASE = 0xcd,
	ATA_CMD_WRITE_MULTIPLE_FUA_EXT = 0xce,
	ATA_CMD_CHECK_MEDIA_CARD_TYPE = 0xd1,
	ATA_CMD_STANDBY_IMMEDIATE = 0xe0,
	ATA_CMD_IDLE_IMMEDIATE = 0xe1,
	ATA_CMD_STANDBY = 0xe2,
	ATA_CMD_IDLE = 0xe3,
	ATA_CMD_READ_BUFFER = 0xe4,
	ATA_CMD_CHECK_POWER_MODE = 0xe5,
	ATA_CMD_SLEEP = 0xe6,
	ATA_CMD_FLUSH_CACHE = 0xe7,
	ATA_CMD_WRITE_BUFFER = 0xe8,
	ATA_CMD_FLUSH_CACHE_EXT = 0xea,
	ATA_CMD_IDENTIFY_DEVICE = 0xec,
	ATA_CMD_SET_FEATURES = 0xef,
	ATA_CMD_SECURITY_SET_PASSWORD = 0xf1,
	ATA_CMD_SECURITY_UNLOCK = 0xf2,
	ATA_CMD_SECURITY_EARASE_PREPARE = 0xf3,
	ATA_CMD_SECURITY_ERASE_UNIT = 0xf4,
	ATA_CMD_SECURITY_FREEZE_LOCK = 0xf5,
	ATA_CMD_SECURITY_DISABLE_PASSWORD = 0xf6,
	ATA_CMD_READ_NATIVE_MAX_ADDRESS = 0xf8,
	ATA_CMD_SET_MAX_ADDRESS = 0xf9
} AtaCommand;

typedef enum AtaStatus {
	ATA_STAT_BUSY = 0x80,
	ATA_STAT_READY = 0x40,
	ATA_STAT_FAULT = 0x20,
	ATA_STAT_SEEK = 0x10,
	ATA_STAT_DRQ = 0x08,
	ATA_STAT_CORR = 0x04,
	ATA_STAT_INDEX = 0x02,
	ATA_STAT_ERR = 0x01,
} AtaStatus;

#include <stdint.h>

typedef enum AtaMajorRevision {
	ATA_MAJOR_ATA4	= (1 << 4),
	ATA_MAJOR_ATA5	= (1 << 5),
	ATA_MAJOR_ATA6	= (1 << 6),
	ATA_MAJOR_ATA7	= (1 << 7),
	ATA_MAJOR_ATA8	= (1 << 8),
} AtaMajorRevision;

typedef struct AtaIdentify {
	uint16_t config;
	uint16_t word1;
	uint16_t specific_config;
	uint16_t word3_9[7];
	uint16_t serial[10];
	uint16_t word20_22[3];
	uint16_t firmware[4];
	uint16_t model[20];
	uint16_t max_multi_sectors;
	uint16_t trusted_features;
	uint16_t capabilities[2];
	uint16_t word51_52[2];
	uint16_t dma_valid;
	uint16_t word54_58[5];
	uint16_t cur_multi_sectors;
	uint16_t sectors28[2];
	uint16_t word62;
	uint16_t dma_mode;
	uint16_t pio_modes;
	uint16_t min_ns_per_dma;
	uint16_t rec_ns_per_dma;
	uint16_t min_ns_per_pio;
	uint16_t min_ns_per_iordy_pio;
	uint16_t word69_70[2];
	uint16_t word71_74[4];
	uint16_t queue_depth;
	uint16_t word76_79[4];
	uint16_t major_version;
	uint16_t minor_version;
	uint16_t command_sets[2];
	uint16_t features[4];
	uint16_t dma_modes;
	uint16_t sec_erase_time;
	uint16_t esec_erase_time;
	uint16_t cur_apm;
	uint16_t master_password_identifier;
	uint16_t reset_result;
	uint16_t acoustics;
	uint16_t stream_min_size;
	uint16_t stream_transfer_time_dma;
	uint16_t stream_access_latency;
	uint16_t stream_perf_gran[2];
	uint16_t sectors48[4];
	uint16_t stream_transfer_time_pio;
	uint16_t word105;
	uint16_t log_sects_per_phys;
	uint16_t inter_seek_delay;
	uint16_t naa_ieee_oui;
	uint16_t ieee_oui_uid;
	uint16_t uid[2];
	uint16_t word112_116[5];
	uint16_t words_per_sector[2];
	uint16_t more_features[2];
	uint16_t word121_127[7];
	uint16_t sec_status;
	uint16_t word129_159[31];
	uint16_t cfa_power_mode;
	uint16_t word161_175[15];
	uint16_t media_serial[30];
	uint16_t sct_command_transport;
	uint16_t word207_208[2];
	uint16_t log_to_phys_alignment;
	uint16_t wrv_sec_mode3[2];
	uint16_t vsc_mode2[2];
	uint16_t nv_cache_cap;
	uint16_t nv_cache_size[2];
	uint16_t nv_cache_read_mbs;
	uint16_t nv_cache_write_mbs;
	uint16_t nv_cache_spinup;
	uint16_t wrv_mode;
	uint16_t word221;
	uint16_t transport_major_ver;
	uint16_t transport_minor_ver;
	uint16_t word224_233[10];
	uint16_t min_512s_per_dnld_micro;
	uint16_t max_512s_per_dnld_micro;
	uint16_t word236_254[19];
	uint16_t integrity;
} AtaIdentify;

#endif /* __DRIVERS_STORAGE_ATA_H__ */
