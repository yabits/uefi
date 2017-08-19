/*++

Copyright (c) 2014 Minoca Corp.

    This file is licensed under the terms of the GNU General Public License
    version 3. Alternative licensing terms are available. Contact
    info@minocacorp.com for details. See the LICENSE file at the root of this
    project for complete licensing information.

Module Name:

    acpi.c

Abstract:

    This module implements ACPI table support for the UEFI firmware on PC/AT
    BIOS machines.

Author:

    Evan Green 26-Mar-2014

Environment:

    Firmware

--*/

//
// ------------------------------------------------------------------- Includes
//

#include <minoca/lib/types.h>
#include <minoca/fw/acpitabs.h>
#include <minoca/fw/smbios.h>
#include <uefifw.h>

//
// ---------------------------------------------------------------- Definitions
//

//
// Define the search parameters for the SMBIOS table.
//

#define SMBIOS_SEARCH_INCREMENT 0x10

//
// ------------------------------------------------------ Data Type Definitions
//

//
// ----------------------------------------------- Internal Function Prototypes
//

PVOID
EfipPcatSearchForRsdp (
    PVOID Address,
    ULONGLONG Length
    );

BOOLEAN
EfipPcatChecksumTable (
    VOID *Address,
    UINT32 Length
    );

PVOID
EfipPcatFindSmbiosTable (
    VOID *Address,
    UINT64 Length
    );

//
// -------------------------------------------------------------------- Globals
//

//
// Save a pointer to the RSDP.
//

VOID *EfiRsdpPointer;

extern EFI_GUID EfiSmbiosTableGuid;

//
// ------------------------------------------------------------------ Functions
//

VOID *
EfipPcatFindRsdp (
    VOID *Address,
    UINT64 Length
    )

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

{

    PVOID RsdpPointer;

    //
    // Search the given range
    //

    RsdpPointer = EfipPcatSearchForRsdp(Address, Length);

    if (RsdpPointer != NULL) {
        return RsdpPointer;
    }

    return NULL;
}

EFI_STATUS
EfipPcatInstallRsdp (
    VOID *Address,
    UINT64 Length
    )

/*++

Routine Description:

    This routine installs the RSDP pointer as a configuration table in EFI.

Arguments:

    None.

Return Value:

    EFI status.

--*/

{

    EFI_GUID *Guid;
    PRSDP Rsdp;
    EFI_STATUS Status;

    Rsdp = EfiRsdpPointer;
    if (Rsdp == NULL) {
        Rsdp = EfipPcatFindRsdp(Address, Length);
    }

    if (Rsdp == NULL) {
        return EFI_UNSUPPORTED;
    }

    if (Rsdp->Revision >= ACPI_20_RSDP_REVISION) {
        Guid = &EfiAcpiTableGuid;

    } else {
        Guid = &EfiAcpiTable1Guid;
    }

    Status = EfiInstallConfigurationTable(Guid, Rsdp);
    return Status;
}

EFI_STATUS
EfipPcatInstallSmbios (
    VOID *Address,
    UINT64 Length
    )

/*++

Routine Description:

    This routine installs the SMBIOS entry point structure as a configuration
    table in EFI.

Arguments:

    None.

Return Value:

    EFI status.

--*/

{

    PSMBIOS_ENTRY_POINT SmbiosTable;
    EFI_STATUS Status;

    SmbiosTable = EfipPcatFindSmbiosTable(Address, Length);
    if (SmbiosTable == NULL) {
        return EFI_SUCCESS;
    }

    Status = EfiInstallConfigurationTable(&EfiSmbiosTableGuid, SmbiosTable);
    return Status;
}

//
// --------------------------------------------------------- Internal Functions
//

PVOID
EfipPcatFindSmbiosTable (
    VOID *Address,
    UINT64 Length
    )

/*++

Routine Description:

    This routine attempts to find the SMBIOS table entry point structure.

Arguments:

    None.

Return Value:

    Returns a pointer to the SMBIOS entry point structure on success.

    NULL on failure.

--*/

{

    UINTN CompareIndex;
    PVOID Intermediate;
    UCHAR Size;
    BOOL Match;
    ULONG Offset;
    PSMBIOS_ENTRY_POINT Table;

    Table = Address;
    while (Table < Address+Size) {
        if (Table->AnchorString == SMBIOS_ANCHOR_STRING_VALUE) {
            Size = Table->EntryPointLength;

            //
            // Check the checksum.
            //

            if (EfipPcatChecksumTable(Table, Size) != FALSE) {

                //
                // Also verify and checksum the second part of the table.
                //

                Match = TRUE;
                for (CompareIndex = 0;
                     CompareIndex < SMBIOS_INTERMEDIATE_ANCHOR_SIZE;
                     CompareIndex += 1) {

                    if (Table->IntermediateAnchor[CompareIndex] !=
                        SMBIOS_INTERMEDIATE_ANCHOR[CompareIndex]) {

                        Match = FALSE;
                        break;
                    }
                }

                if (Match != FALSE) {
                    Offset = OFFSET_OF(SMBIOS_ENTRY_POINT, IntermediateAnchor);
                    Size = sizeof(SMBIOS_ENTRY_POINT) - Offset;
                    Intermediate = (((PVOID)Table) + Offset);

                    //
                    // If this also checksums, then the table really is here.
                    //

                    if (EfipPcatChecksumTable(Intermediate, Size) != FALSE) {
                        return Table;
                    }
                }
            }
        }

        //
        // Move up 16 bytes to the next candidate position.
        //

        Table = (PSMBIOS_ENTRY_POINT)(((PVOID)Table) + SMBIOS_SEARCH_INCREMENT);
    }

    return NULL;
}

VOID *
EfipPcatSearchForRsdp (
    VOID *Address,
    UINT64 Length
    )

/*++

Routine Description:

    This routine searches the given range for the RSDP table.

Arguments:

    Address - Supplies the starting address to search for the RSDP. This
        address must be 16 byte aligned.

    Length - Supplies the number of bytes to search.

Return Value:

    Returns a pointer to the RSDP table on success.

    NULL on failure.

--*/

{

    UINT64 *CurrentAddress;
    BOOLEAN GoodChecksum;

    CurrentAddress = Address;
    while (Length >= sizeof(UINT64)) {
        if (*CurrentAddress == RSDP_SIGNATURE) {
            GoodChecksum = EfipPcatChecksumTable(CurrentAddress,
                                                 RSDP_CHECKSUM_SIZE);

            /*
            if (GoodChecksum != FALSE) {
                return CurrentAddress;
            }
            */
            return CurrentAddress;
        }

        //
        // Advance by 16 bytes.
        //

        CurrentAddress = (UINT64 *)((UINT8 *)CurrentAddress + 16);
    }

    return NULL;
}

BOOLEAN
EfipPcatChecksumTable (
    VOID *Address,
    UINT32 Length
    )

/*++

Routine Description:

    This routine sums all of the bytes in a given table to determine if its
    checksum is correct. The checksum is set such that all the bytes in the
    table sum to a value of 0.

Arguments:

    Address - Supplies the address of the table to checksum.

    Length - Supplies the length of the table, in bytes.

Return Value:

    TRUE if all bytes in the table correctly sum to 0.

    FALSE if the bytes don't properly sum to 0.

--*/

{

    UINT8 *CurrentByte;
    UINT8 Sum;

    CurrentByte = Address;
    Sum = 0;
    while (Length != 0) {
        Sum += *CurrentByte;
        CurrentByte += 1;
        Length -= 1;
    }

    if (Sum == 0) {
        return TRUE;
    }

    return FALSE;
}

