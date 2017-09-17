/*
 * Copyright (C) Freescale Semiconductor, Inc. 2006.
 * Author: Jason Jin<Jason.jin@freescale.com>
 *         Zhang Wei<wei.zhang@freescale.com>
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
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * with the reference on libata and ahci drvier in kernel
 *
 */

#include <endian.h>
#include <libpayload.h>
#include <stdint.h>

#include "dev/ahci.h"
#include "dev/ata.h"
#include "blockdev/blockdev.h"
#include "blockdev/list.h"

typedef struct SataDrive {
	BlockDev dev;

	AhciCtrlr *ctrlr;
	AhciIoPort *port;
} SataDrive;

#define writel_with_flush(a,b)	do { writel(a, b); readl(b); } while (0)

/* Maximum timeouts for each event */
static const int wait_ms_spinup = 10000;
static const int wait_ms_flush  = 5000;
static const int wait_ms_dataio = 5000;
static const int wait_ms_linkup = 4;

static void *ahci_port_base(void *base, int port)
{
	return (uint8_t *)base + 0x100 + (port * 0x80);
}

static void ahci_setup_port(AhciIoPort *port, void *base, int idx)
{
	base = ahci_port_base(base, idx);

	port->cmd_addr = base;
	port->scr_addr = (uint8_t *)base + PORT_SCR;
}

#define WAIT_UNTIL(expr, timeout)				\
	({							\
		typeof(timeout) __counter = timeout * 1000;	\
		typeof(expr) __expr_val;			\
		while (!(__expr_val = (expr)) && __counter--)	\
			udelay(1);				\
		__expr_val;					\
	})

#define WAIT_WHILE(expr, timeout) !WAIT_UNTIL(!(expr), (timeout))

static const char *ahci_decode_speed(uint32_t speed)
{
	switch (speed) {
	case 1: return "1.5";
	case 2: return "3";
	case 3: return "6";
	default: return "?";
	}
}

static const char *ahci_decode_mode(uint32_t mode)
{
	switch (mode) {
	case 0x0101: return "IDE";
	case 0x0104: return "RAID";
	case 0x0106: return "SATA";
	default: return "unknown";
	}
}

static void ahci_print_info(AhciCtrlr *ctrlr)
{
	pcidev_t pdev = ctrlr->dev;
	void *mmio = ctrlr->mmio_base;
	uint32_t cap, cap2;

	cap = ctrlr->cap;
	cap2 = readl(mmio + HOST_CAP2);

	uint8_t *vers = (uint8_t *)mmio + HOST_VERSION;
	printf("AHCI %02x%02x.%02x%02x", vers[3], vers[2], vers[1], vers[0]);
	printf(" %u slots", ((cap >> 8) & 0x1f) + 1);
	printf(" %u ports", (cap & 0x1f) + 1);
	uint32_t speed = (cap >> 20) & 0xf;
	printf(" %s Gbps", ahci_decode_speed(speed));
	printf(" %#x impl", ctrlr->port_map);
	uint32_t mode = pci_read_config16(pdev, 0xa);
	printf(" %s mode\n", ahci_decode_mode(mode));

	printf("flags: ");
	if (cap & (1 << 31)) printf("64bit ");
	if (cap & (1 << 30)) printf("ncq ");
	if (cap & (1 << 28)) printf("ilck ");
	if (cap & (1 << 27)) printf("stag ");
	if (cap & (1 << 26)) printf("pm ");
	if (cap & (1 << 25)) printf("led ");
	if (cap & (1 << 24)) printf("clo ");
	if (cap & (1 << 19)) printf("nz ");
	if (cap & (1 << 18)) printf("only ");
	if (cap & (1 << 17)) printf("pmp ");
	if (cap & (1 << 16)) printf("fbss ");
	if (cap & (1 << 15)) printf("pio ");
	if (cap & (1 << 14)) printf("slum ");
	if (cap & (1 << 13)) printf("part ");
	if (cap & (1 << 7)) printf("ccc ");
	if (cap & (1 << 6)) printf("ems ");
	if (cap & (1 << 5)) printf("sxs ");
	if (cap2 & (1 << 2)) printf("apst ");
	if (cap2 & (1 << 1)) printf("nvmp ");
	if (cap2 & (1 << 0)) printf("boh ");
	putchar('\n');
}

#define MAX_DATA_BYTE_COUNT  (4 * 1024 * 1024)

static int ahci_fill_sg(AhciSg *sg, void *buf, int len)
{
	uint32_t sg_count = ((len - 1) / MAX_DATA_BYTE_COUNT) + 1;
	if (sg_count > AHCI_MAX_SG) {
		printf("Error: Too much sg!\n");
		return -1;
	}

	for (int i = 0; i < sg_count; i++) {
		sg->addr = htolel((uintptr_t)buf + i * MAX_DATA_BYTE_COUNT);
		sg->addr_hi = 0;
		uint32_t bytes = MIN(len, MAX_DATA_BYTE_COUNT);
		sg->flags_size = htolel((bytes - 1) & 0x3fffff);
		sg++;
		len -= MAX_DATA_BYTE_COUNT;
	}

	return sg_count;
}


static void ahci_fill_cmd_slot(AhciIoPort *pp, uint32_t opts)
{
	pp->cmd_slot->opts = htolel(opts);
	pp->cmd_slot->status = 0;
	pp->cmd_slot->tbl_addr = htolel((uint32_t)(uintptr_t)pp->cmd_tbl);
	pp->cmd_slot->tbl_addr_hi = 0;
}


static int ahci_port_start(AhciIoPort *port, int index)
{
	uint8_t *port_mmio = port->port_mmio;

	port->index = index;

	uint32_t status = readl(port_mmio + PORT_SCR_STAT);
	printf("Port %d status: %x\n", index, status);
	if ((status & 0xf) != 0x3) {
		printf("No link on this port!\n");
		return -1;
	}

	uint8_t *mem = memalign(2048, AHCI_PORT_PRIV_DMA_SZ);
	if (!mem) {
		printf("No mem for table!\n");
		return -1;
	}
	memset(mem, 0, AHCI_PORT_PRIV_DMA_SZ);

	/*
	 * First item in chunk of DMA memory: 32-slot command table,
	 * 32 bytes each in size
	 */
	port->cmd_slot = (AhciCommandHeader *)mem;
	mem += (AHCI_CMD_SLOT_SZ + 224);

	/*
	 * Second item: Received-FIS area
	 */
	port->rx_fis = mem;
	mem += AHCI_RX_FIS_SZ;

	/*
	 * Third item: data area for storing a single command
	 * and its scatter-gather table
	 */
	port->cmd_tbl = mem;
	mem += AHCI_CMD_TBL_HDR;

	port->cmd_tbl_sg = (AhciSg *)mem;

	writel_with_flush((uintptr_t)port->cmd_slot, port_mmio + PORT_LST_ADDR);

	writel_with_flush((uintptr_t)port->rx_fis, port_mmio + PORT_FIS_ADDR);

	writel_with_flush(PORT_CMD_ICC_ACTIVE | PORT_CMD_FIS_RX |
			  PORT_CMD_POWER_ON | PORT_CMD_SPIN_UP |
			  PORT_CMD_START, port_mmio + PORT_CMD);

	return 0;
}


static int ahci_device_data_io(AhciIoPort *port, void *fis, int fis_len,
			       void *buf, int buf_len, int is_write, int wait)
{

	uint8_t *port_mmio = port->port_mmio;

	uint32_t port_status = readl(port_mmio + PORT_SCR_STAT);
	if ((port_status & 0xf) != 0x3) {
		printf("No link on port %d!\n", port->index);
		return -1;
	}

	memcpy(port->cmd_tbl, fis, fis_len);

	int sg_count = 0;
	if (buf && buf_len)
		sg_count = ahci_fill_sg(port->cmd_tbl_sg, buf, buf_len);
	uint32_t opts = (fis_len >> 2) | (sg_count << 16) | (is_write << 6);
	ahci_fill_cmd_slot(port, opts);

	writel_with_flush(1, port_mmio + PORT_CMD_ISSUE);

	// Wait for the command to complete.
	if (WAIT_WHILE((readl(port_mmio + PORT_CMD_ISSUE) & 0x1), wait)) {
		printf("AHCI: I/O timeout!\n");
		return -1;
	}

	return 0;
}

/*
 * In the general case of generic rotating media it makes sense to have a
 * flush capability. It probably even makes sense in the case of SSDs because
 * one cannot always know for sure what kind of internal cache/flush mechanism
 * is embodied therein.  Because writing to the disk in u-boot is very rare,
 * this flush command will be invoked after every block write.
 */
static int ahci_io_flush(AhciIoPort *port)
{
	uint8_t fis[20];

	// Set up the FIS.
	memset(fis, 0, 20);
	fis[0] = 0x27;		 // Host to device FIS.
	fis[1] = 1 << 7;	 // Command FIS.
	fis[2] = ATA_CMD_FLUSH_CACHE_EXT;

	if (ahci_device_data_io(port, fis, 20, NULL, 0, 1,
				wait_ms_flush) < 0) {
		printf("AHCI: Flush command failed.\n");
		return -1;
	}

	return 0;
}

/*
 * Some controllers limit number of blocks they can read/write at once.
 * Contemporary SSD devices work much faster if the read/write size is aligned
 * to a power of 2.  Let's set default to 128 and allowing to be overwritten if
 * needed.
 */
#ifndef MAX_SATA_BLOCKS_READ_WRITE
#define MAX_SATA_BLOCKS_READ_WRITE	0x80
#endif

static int ahci_read_write(SataDrive *drive, lba_t start, uint16_t count,
			   void *buf, int is_write)
{
	uint8_t fis[20];

	// Set up the FIS.
	memset(fis, 0, 20);
	fis[0] = 0x27;		 // Host to device FIS.
	fis[1] = 1 << 7;	 // Command FIS.
	// Command byte
	fis[2] = is_write ? ATA_CMD_WRITE_SECTORS_EXT :
		ATA_CMD_READ_SECTORS_EXT;

	while (count) {
		uint16_t tblocks = MIN(MAX_SATA_BLOCKS_READ_WRITE, count);
		uintptr_t tsize = tblocks * drive->dev.block_size;

		// LBA48 SATA command using 32bit address range.
		fis[3] = 0xe0; /* features */
		fis[4] = (start >> 0) & 0xff;
		fis[5] = (start >> 8) & 0xff;
		fis[6] = (start >> 16) & 0xff;
		fis[7] = 1 << 6; /* device reg: set LBA mode */
		fis[8] = ((start >> 24) & 0xff);

		// Block count.
		fis[12] = (tblocks >> 0) & 0xff;
		fis[13] = (tblocks >> 8) & 0xff;

		// Read/write from AHCI.
		if (ahci_device_data_io(drive->port, fis, sizeof(fis), buf,
					tsize, is_write, wait_ms_dataio)) {
			printf("AHCI: %s command failed.\n",
			      is_write ? "write" : "read");
			return -1;
		}

		// Flush writes.
		if (is_write) {
			if (ahci_io_flush(drive->port) < 0)
				return -1;
		}

		buf = (uint8_t *)buf + tsize;
		count -= tblocks;
		start += tblocks;
	}

	return 0;
}

static lba_t ahci_read(BlockDevOps *me, lba_t start, lba_t count, void *buffer)
{
	SataDrive *drive = container_of(me, SataDrive, dev.ops);
	if (ahci_read_write(drive, start, count, buffer, 0)) {
		printf("AHCI: Read failed.\n");
		return -1;
	}
	return count;
}

static lba_t ahci_write(BlockDevOps *me, lba_t start, lba_t count,
			const void *buffer)
{
	SataDrive *drive = container_of(me, SataDrive, dev.ops);
	if (ahci_read_write(drive, start, count, (void *)buffer, 1)) {
		printf("AHCI: Write failed.\n");
		return -1;
	}
	return count;
}

static inline int ata_implements_major(AtaIdentify *id, AtaMajorRevision rev)
{
	uint16_t major = le16toh(id->major_version);

	return major != 0xffff && (major & rev);
}

static int ata_identify_integrity(AtaIdentify *id)
{
	uint8_t sum = 0;
	int i;

	/*
	 * Target integrity check only for >= ATA8; it's supported on other
	 * standards, but we can't be bothered to detect all of them right now
	 */
	if (ata_implements_major(id, ATA_MAJOR_ATA8))
		return 0;

	/*
	 * Checksum is still optional; if we don't see the signature byte in
	 * the lower bits, it's not implemented
	 */
	if ((le16toh(id->integrity) & 0xff) != 0xA5)
		return 0;

	for (i = 0; i < sizeof(*id); i++)
		sum += ((uint8_t *)id)[i];

	return sum != 0;
}

static int ahci_identify(AhciIoPort *port, AtaIdentify *id)
{
	uint8_t fis[20];
	int ret;

	memset(fis, 0, 20);
	// Construct the FIS
	fis[0] = 0x27;		// Host to device FIS.
	fis[1] = 1 << 7;	// Command FIS.
	// Command byte.
	fis[2] = ATA_CMD_IDENTIFY_DEVICE;

	if (ahci_device_data_io(port, fis, 20, id, sizeof(AtaIdentify), 0,
				wait_ms_dataio)) {
		printf("AHCI: Identify command failed.\n");
		return -1;
	}
	printf("size of AtaIdentify is %zu.\n", sizeof(AtaIdentify));

	ret = ata_identify_integrity(id);
	if (ret)
		printf("ATA identify integrity failed on port %d\n",
				port->index);

	return ret;
}

static int ahci_read_capacity(AhciIoPort *port, lba_t *cap,
			      unsigned *block_size)
{
	AtaIdentify id;

	if (ahci_identify(port, &id))
		return -1;

	uint32_t cap32;
	memcpy(&cap32, &id.sectors28, sizeof(cap32));
	*cap = letohl(cap32);
	if (*cap == 0xfffffff) {
		memcpy(cap, id.sectors48, sizeof(*cap));
		*cap = letohll(*cap);
	}

	*block_size = 512;
	return 0;
}

static int ahci_ctrlr_init(BlockDevCtrlrOps *me)
{
	uint32_t host_impl_bitmap;

	AhciCtrlr *ctrlr = container_of(me, AhciCtrlr, ctrlr.ops);

	ctrlr->mmio_base = (void *)pci_read_resource(ctrlr->dev, 5);
	printf("AHCI MMIO base = %p\n", ctrlr->mmio_base);

	// JMicron-specific fixup taken from kernel:
	// make sure we're in AHCI mode
	if (pci_read_config16(ctrlr->dev, REG_VENDOR_ID) == 0x197b)
		pci_write_config8(ctrlr->dev, 0x41, 0xa1);

	/* initialize adapter */
	pcidev_t pdev = ctrlr->dev;
	void *mmio = ctrlr->mmio_base;

	uint32_t cap_save = readl(mmio + HOST_CAP);
	cap_save &= ((1 << 28) | (1 << 17));
	cap_save |= (1 << 27);

	// Global controller reset.
	uint32_t host_ctl = readl(mmio + HOST_CTL);
	if ((host_ctl & HOST_RESET) == 0)
		writel_with_flush(host_ctl | HOST_RESET,
			(uintptr_t)mmio + HOST_CTL);

	// Reset must complete within 1 second.
	if (WAIT_WHILE((readl(mmio + HOST_CTL) & HOST_RESET), 1000)) {
		printf("Controller reset failed.\n");
		return -1;
	}

	writel_with_flush(HOST_AHCI_EN, mmio + HOST_CTL);
	writel(cap_save, mmio + HOST_CAP);
	writel_with_flush(0xf, mmio + HOST_PORTS_IMPL);

	ctrlr->cap = readl(mmio + HOST_CAP);
	host_impl_bitmap = ctrlr->port_map = readl(mmio + HOST_PORTS_IMPL);
	/* ABAR+0x0 (GHC_CAP) reports number of SATA ports, its always read as
	 * +1 means if '0' which means number of enabled SATA port is 1.
	 * ABAR+0xC (GHC_PI) provides port bit map, hence relied on Port Map
	 * not number of SATA port
	 */
	ctrlr->n_ports = 0;
	while (host_impl_bitmap != 0) {
		ctrlr->n_ports++;
		host_impl_bitmap = host_impl_bitmap >> 1;
	}

	printf("cap %#x  port_map %#x  n_ports %d\n",
	      ctrlr->cap, ctrlr->port_map, ctrlr->n_ports);

	for (int i = 0; i < ctrlr->n_ports; i++) {
		/* Skip ports that are not enabled. */
		if (!(ctrlr->port_map & (1 << i)))
			continue;

		ctrlr->ports[i].port_mmio = ahci_port_base(mmio, i);
		uint8_t *port_mmio = (uint8_t *)ctrlr->ports[i].port_mmio;
		ahci_setup_port(&ctrlr->ports[i], mmio, i);

		/* make sure port is not active */
		uint32_t port_cmd = readl(port_mmio + PORT_CMD);
		uint32_t port_cmd_bits =
			PORT_CMD_LIST_ON | PORT_CMD_FIS_ON |
			PORT_CMD_FIS_RX | PORT_CMD_START;
		if (port_cmd & port_cmd_bits) {
			printf("Port %d is active. Deactivating.\n", i);
			port_cmd &= ~port_cmd_bits;
			writel_with_flush(port_cmd, port_mmio + PORT_CMD);

			/* spec says 500 msecs for each bit, so
			 * this is slightly incorrect.
			 */
			mdelay(500);
		}

		/* Bring up SATA link. */
		port_cmd = PORT_CMD_SPIN_UP | PORT_CMD_FIS_RX;
		writel_with_flush(port_cmd, port_mmio + PORT_CMD);

		int j;
		uint32_t tmp;
		for (j = 0; j < wait_ms_linkup; j++) {
			tmp = readl(port_mmio + PORT_SCR_STAT);
			if ((tmp & 0xf) == 0x3)
				break;
			mdelay(1);
		}
		if (j == wait_ms_linkup) {
			printf("SATA link %d timeout.\n", i);
			continue;
		} else {
			printf("SATA link %d ok.\n", i);
		}

		/* Clear error status */
		uint32_t port_scr_err = readl(port_mmio + PORT_SCR_ERR);
		if (port_scr_err)
			writel(port_scr_err, port_mmio + PORT_SCR_ERR);

		/* Wait for SATA device to complete spin-up. */
		printf("Waiting for device on port %d... ", i);

		for (j = 0; j < wait_ms_spinup; j++) {
			tmp = readl(port_mmio + PORT_TFDATA);
			if (!(tmp & (ATA_STAT_BUSY | ATA_STAT_DRQ)))
				break;
			mdelay(1);
		}
		if (j == wait_ms_spinup)
			printf("timeout.\n");
		else
			printf("ok. Target spinup took %d ms.\n", j);

		/* Clear error status */
		port_scr_err = readl(port_mmio + PORT_SCR_ERR);
		if (port_scr_err) {
			printf("PORT_SCR_ERR %#x\n", port_scr_err);
			writel(port_scr_err, port_mmio + PORT_SCR_ERR);
		}

		/* ack any pending irq events for this port */
		uint32_t port_irq_stat = readl(port_mmio + PORT_IRQ_STAT);
		if (port_irq_stat) {
			printf("PORT_IRQ_STAT 0x%x\n", port_irq_stat);
			writel(port_irq_stat, port_mmio + PORT_IRQ_STAT);
		}

		writel(1 << i, mmio + HOST_IRQ_STAT);

		/* set irq mask (enables interrupts) */
		writel(DEF_PORT_IRQ, port_mmio + PORT_IRQ_MASK);

		/* register linkup ports */
		uint32_t port_scr_stat = readl(port_mmio + PORT_SCR_STAT);
		printf("Port %d status: 0x%x\n", i, port_scr_stat);
		if ((port_scr_stat & 0xf) == 0x3)
			ctrlr->link_port_map |= (0x1 << i);
	}

	host_ctl = readl(mmio + HOST_CTL);
	writel(host_ctl | HOST_IRQ_EN, mmio + HOST_CTL);
	host_ctl = readl(mmio + HOST_CTL);
	printf("HOST_CTL 0x%x\n", host_ctl);

	pci_write_config16(pdev, REG_COMMAND,
		pci_read_config16(pdev, REG_COMMAND) | REG_COMMAND_BM);

	ahci_print_info(ctrlr);

	uint32_t linkmap = ctrlr->link_port_map;

	for (int i = 0; i < sizeof(linkmap) * 8; i++) {
		if (((linkmap >> i) & 0x1)) {
			AhciIoPort *port = &ctrlr->ports[i];
			if (ahci_port_start(port, i)) {
				printf("Can not start port %d\n", i);
				continue;
			}
			lba_t cap;
			unsigned block_size;
			if (ahci_read_capacity(port, &cap, &block_size)) {
				printf("Can't read port %d's capacity.\n", i);
				continue;
			}

			SataDrive *sata_drive = xzalloc(sizeof(*sata_drive));
			static const int name_size = 18;
			char *name = xmalloc(name_size);
			snprintf(name, name_size, "Sata port %d", i);
			sata_drive->dev.ops.read = &ahci_read;
			sata_drive->dev.ops.write = &ahci_write;
			sata_drive->dev.ops.new_stream = &new_simple_stream;
			sata_drive->dev.name = name;
			sata_drive->dev.removable = 0;
			sata_drive->dev.block_size = block_size;
			sata_drive->dev.block_count = cap;
			sata_drive->ctrlr = ctrlr;
			sata_drive->port = port;
			list_insert_after(&sata_drive->dev.list_node,
					  &fixed_block_devices);
		}
	}

	ctrlr->ctrlr.need_update = 0;

	return 0;
}

AhciCtrlr *new_ahci_ctrlr(pcidev_t dev)
{
	AhciCtrlr *ctrlr = xzalloc(sizeof(*ctrlr));
	ctrlr->ctrlr.ops.update = &ahci_ctrlr_init;
	ctrlr->ctrlr.need_update = 1;
	ctrlr->dev = dev;
	return ctrlr;
}
