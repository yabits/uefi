#include "ueficore.h"
#include <stdio.h>

EFI_STATUS
EFIAPI
EfiSimpleTextOutputString (
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    CHAR16 *String
    )
{
    printf("%s", String);

    return EFI_SUCCESS;
}
