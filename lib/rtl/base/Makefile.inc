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

#TARGETS-y += lib/rtl/base/crc32.o lib/rtl/base/fp2int.o
#TARGETS-y += lib/rtl/base/heap.o lib/rtl/base/heapprof.o
TARGETS-y += lib/rtl/base/math.o lib/rtl/base/print.o
TARGETS-y += lib/rtl/base/rbtree.o lib/rtl/base/scan.o
#TARGETS-y += lib/rtl/base/softfp.o lib/rtl/base/string.o
TARGETS-y += lib/rtl/base/string.o
TARGETS-y += lib/rtl/base/time.o lib/rtl/base/timezone.o
#TARGETS-y += lib/rtl/base/version.o lib/rtl/base/wchar.o
TARGETS-y += lib/rtl/base/wchar.o
#TARGETS-y += lib/rtl/base/wprint.o lib/rtl/base/wscan.o
#TARGETS-y += lib/rtl/base/wstring.o lib/rtl/base/wstring.o
#TARGETS-y += lib/rtl/base/wtime.o
TARGETS-y += lib/rtl/base/x86/rtlmem.S.o lib/rtl/base/x86/rtlarch.S.o

