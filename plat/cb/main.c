/*++

Copyright (c) 2014 Minoca Corp.

    This file is licensed under the terms of the GNU General Public License
    version 3. Alternative licensing terms are available. Contact
    info@minocacorp.com for details. See the LICENSE file at the root of this
    project for complete licensing information.

Module Name:

    main.c

Abstract:

    This module implements the entry point for the UEFI firmware running on top
    of a legacy PC/AT BIOS.

Author:

    Evan Green 27-Feb-2014

Environment:

    Firmware

--*/

//
// ------------------------------------------------------------------- Includes
//

#include <uefifw.h>
#include <libpayload.h>
#include <assert.h>

//
// ---------------------------------------------------------------- Definitions
//

#define FIRMWARE_IMAGE_NAME "biosfw"

//
// ------------------------------------------------------ Data Type Definitions
//

//
// ----------------------------------------------- Internal Function Prototypes
//

//
// -------------------------------------------------------------------- Globals
//

//
// Variables defined in the linker script that mark the start and end of the
// image.
//

extern INT8 _end;
extern INT8 __executable_start;
extern VOID *EfiRsdpPointer;

//
// ------------------------------------------------------------------ Functions
//

int main()
{
    // all arguments are dummy
    EfiCoreMain(
            CONFIG_LP_BASE_ADDRESS,
            CONFIG_LP_BASE_ADDRESS,
            4096,
            FIRMWARE_IMAGE_NAME,
            4096,
            CONFIG_LP_STACK_SIZE
            );

    // unreachable
    halt();

    return 0;
}

EFI_STATUS
EfiPlatformInitialize (
    UINT32 Phase
    )

/*++

Routine Description:

    This routine performs platform-specific firmware initialization.

Arguments:

    Phase - Supplies the iteration number this routine is being called on.
        Phase zero occurs very early, just after the debugger comes up.
        Phase one occurs a bit later, after timer and interrupt services are
        initialized. Phase two happens right before boot, after all platform
        devices have been enumerated.

Return Value:

    EFI status code.

--*/

{

    UINT32 i;
    EFI_STATUS Status;
    VOID *Address;
    UINT64 Length;

    lib_get_sysinfo();

    Address = 0x00;
    Length = 0x00;

    for (i=0; i<lib_sysinfo.n_memranges; i++) {
        if (lib_sysinfo.memrange[i].type == 16 &&
                lib_sysinfo.memrange[i].base != 0x0) {
            Address = lib_sysinfo.memrange[i].base;
            Length = lib_sysinfo.memrange[i].size;
            break;
        }
    }

    assert(Address != 0x00 && Length != 0x00);

    Status = EFI_SUCCESS;
    if (Phase == 0) {
        EfiRsdpPointer = EfipPcatFindRsdp(Address, Length);
        if (EfiRsdpPointer == NULL) {
            return EFI_NOT_FOUND;
        }

    } else if (Phase == 1) {
        Status = EfipPcatInstallRsdp(Address, Length);
        if (EFI_ERROR(Status)) {
            return Status;
        }

        Status = EfipPcatInstallSmbios(Address, Length);
        if (EFI_ERROR(Status)) {
            return Status;
        }
    }

    return Status;
}

EFI_STATUS
EfiPlatformEnumerateDevices (
    VOID
    )

/*++

Routine Description:

    This routine enumerates and connects any builtin devices the platform
    contains.

Arguments:

    None.

Return Value:

    EFI status code.

--*/

{

    EFI_STATUS Status;

    //Status = EfipPcatEnumerateDisks();
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //EfipPcatEnumerateVideo();
    return EFI_SUCCESS;
}

//
// --------------------------------------------------------- Internal Functions
//

