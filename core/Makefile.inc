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

TARGETS-CORE-y += core/acpi.o core/acpitabs.o
TARGETS-CORE-y += core/basepe.o core/bdsboot.o
TARGETS-CORE-y += core/bdscon.o core/bdsentry.o
TARGETS-CORE-y += core/bdsutil.o core/cfgtable.o
TARGETS-CORE-y += core/crc32.o
#TARGETS-CORE-y += core/dbgser.o core/devpathu.o
TARGETS-CORE-y += core/devpathu.o
TARGETS-CORE-y += core/diskio.o core/dispatch.o
TARGETS-CORE-y += core/div.o core/drvsup.o
TARGETS-CORE-y += core/event.o core/fatdev.o
TARGETS-CORE-y += core/fatfs.o core/fsvars.o
TARGETS-CORE-y += core/fvblock.o core/fvsect.o
TARGETS-CORE-y += core/fwvol.o core/fwvolio.o
TARGETS-CORE-y += core/handle.o core/image.o
TARGETS-CORE-y += core/init.o core/intr.o
TARGETS-CORE-y += core/locate.o core/lock.o
TARGETS-CORE-y += core/memory.o core/part.o
TARGETS-CORE-y += core/partelto.o core/partgpt.o
TARGETS-CORE-y += core/partmbr.o core/pool.o
TARGETS-CORE-y += core/ramdisk.o core/smbios.o
# TARGETS-CORE-y += core/stubs.o core/tpl.o
TARGETS-CORE-y += core/tpl.o
TARGETS-CORE-y += core/timer.o core/util.o
TARGETS-CORE-y += core/version.o core/vidcon.o
#TARGETS-CORE-y += core/main.o
TARGETS-CORE-y += core/blockdev.o
TARGETS-CORE-y += core/serialcon.o
