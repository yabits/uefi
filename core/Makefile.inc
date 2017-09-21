#
# Copyright (C) 2008 by coresystems GmbH
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

include core/rtlib/Makefile.inc
include core/runtime/Makefile.inc
include core/x86/Makefile.inc

TARGETS-y += core/acpi.o core/acpitabs.o
TARGETS-y += core/basepe.o core/bdsboot.o
TARGETS-y += core/bdscon.o core/bdsentry.o
TARGETS-y += core/bdsutil.o core/cfgtable.o
TARGETS-y += core/crc32.o
#TARGETS-y += core/dbgser.o core/devpathu.o
TARGETS-y += core/devpathu.o
TARGETS-y += core/diskio.o core/dispatch.o
TARGETS-y += core/div.o core/drvsup.o
TARGETS-y += core/event.o core/fatdev.o
TARGETS-y += core/fatfs.o core/fsvars.o
TARGETS-y += core/fvblock.o core/fvsect.o
TARGETS-y += core/fwvol.o core/fwvolio.o
TARGETS-y += core/handle.o core/image.o
TARGETS-y += core/init.o core/intr.o
TARGETS-y += core/locate.o core/lock.o
TARGETS-y += core/memory.o core/part.o
TARGETS-y += core/partelto.o core/partgpt.o
TARGETS-y += core/partmbr.o core/pool.o
TARGETS-y += core/ramdisk.o core/smbios.o
# TARGETS-y += core/stubs.o core/tpl.o
TARGETS-y += core/tpl.o
TARGETS-y += core/timer.o core/util.o
TARGETS-y += core/version.o core/vidcon.o
#TARGETS-y += core/main.o
TARGETS-y += core/blockdev.o

