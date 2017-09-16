/*++

Copyright (c) 2014 Minoca Corp.

    This file is licensed under the terms of the GNU General Public License
    version 3. Alternative licensing terms are available. Contact
    info@minocacorp.com for details. See the LICENSE file at the root of this
    project for complete licensing information.

Module Name:

    cbfw.h

Abstract:

    This header contains definitions for the UEFI firmware on top of a legacy
    PC/AT BIOS.

Author:

    Evan Green 27-Feb-2014

--*/

//
// ------------------------------------------------------------------- Includes
//

//
// --------------------------------------------------------------------- Macros
//

//
// ---------------------------------------------------------------- Definitions
//

//
// ------------------------------------------------------ Data Type Definitions
//

/*++

Structure Description:

    This structure defines a BIOS call context, including all code, data, and
    stack memory, and registers.

Members:

    CodePage - Stores the code page information of the real mode operation.

    DataPage - Stores the data page of the real mode operation.

    StackPage - Stores the stack page of the real mode operation.

    Registers - Stores the register state of the real mode context. Upon exit,
        these fields contain the final register values.

--*/

//
// -------------------------------------------------------------------- Globals
//

//
// Save a pointer to the RSDP.
//

extern VOID *EfiRsdpPointer;

//
// -------------------------------------------------------- Function Prototypes
//

VOID *
EfipPcatFindRsdp (
    VOID
    );

/*++

Routine Description:

    This routine attempts to find the ACPI RSDP table pointer on a PC-AT
    compatible system. It looks in the first 1k of the EBDA (Extended BIOS Data
    Area), as well as between the ranges 0xE0000 and 0xFFFFF. This routine
    must be run in physical mode.

Arguments:

    None.

Return Value:

    Returns a pointer to the RSDP table on success.

    NULL on failure.

--*/

EFI_STATUS
EfipPcatInstallRsdp (
    VOID
    );

/*++

Routine Description:

    This routine installs the RSDP pointer as a configuration table in EFI.

Arguments:

    None.

Return Value:

    EFI status.

--*/

EFI_STATUS
EfipPcatInstallSmbios (
    VOID
    );

/*++

Routine Description:

    This routine installs the SMBIOS entry point structure as a configuration
    table in EFI.

Arguments:

    None.

Return Value:

    EFI status.

--*/

EFI_STATUS
EfipPcatEnumerateDisks (
    VOID
    );

/*++

Routine Description:

    This routine enumerates all the disks it can find on a BIOS machine.

Arguments:

    None.

Return Value:

    EFI Status code.

--*/

EFI_STATUS
EfipPcatEnumerateVideo (
    VOID
    );

/*++

Routine Description:

    This routine enumerates the video display on a BIOS machine.

Arguments:

    None.

Return Value:

    EFI Status code.

--*/

//
// Runtime functions
//

EFIAPI
VOID
EfipPcatResetSystem (
    EFI_RESET_TYPE ResetType,
    EFI_STATUS ResetStatus,
    UINTN DataSize,
    VOID *ResetData
    );

/*++

Routine Description:

    This routine resets the entire platform.

Arguments:

    ResetType - Supplies the type of reset to perform.

    ResetStatus - Supplies the status code for this reset.

    DataSize - Supplies the size of the reset data.

    ResetData - Supplies an optional pointer for reset types of cold, warm, or
        shutdown to a null-terminated string, optionally followed by additional
        binary data.

Return Value:

    None. This routine does not return.

--*/

VOID
EfipPcatInitializeReset (
    VOID
    );

/*++

Routine Description:

    This routine initializes support for reset system. This routine must run
    with boot services.

Arguments:

    None.

Return Value:

    None.

--*/

