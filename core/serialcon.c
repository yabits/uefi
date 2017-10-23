#include "ueficore.h"
#include <libpayload.h>
#include <stdio.h>

EFI_STATUS
EFIAPI
EfiSimpleTextInputExReset (
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    BOOLEAN ExtendedVerification
    )
{
    printf("EFiSimpleTextInputExReset is not supported.\n");

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EfiSimpleTextInputExReadKeyStrokeEx (
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    EFI_KEY_DATA *KeyData
    )
{
    printf("EfiSimpleTextInputExReadKeyStrokeEx is not supported.\n");

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EfiSimpleTextInputExSetState (
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    EFI_KEY_TOGGLE_STATE *KeyToggleState
    )
{
    printf("EfiSimpleTextInputExSetState is not supported.\n");

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EfiSimpleTextInputExRegisterKeyNotify (
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    EFI_KEY_DATA *KeyData,
    EFI_KEY_NOTIFY_FUNCTION KeyNotificationFunction,
    VOID **NotifyHandle
    )
{
    printf("EfiSimpleTextInputExRegisterKeyNotify is not supported.\n");

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EfiSimpleTextInputExUnregisterKeyNotify (
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    VOID *NotificationHandle
    )
{
    printf("EfiSimpleTextInputExUnregisterKeyNotify is not supported.\n");

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EfiSimpleTextInputReset (
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    BOOLEAN ExtendedVerification
    )
{
    printf("EfiSimpleTextReset is not suppoted.\n");

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EfiSimpleTextInputReadKeyStroke (
        EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
        EFI_INPUT_KEY *Key
        )
{
    CHAR16 Char;

    Char = (CHAR16)getchar();

    Key->ScanCode = 0x00;
    Key->UnicodeChar = Char;

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EfiSimpleTextOutputReset (
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    BOOLEAN ExtendedVerification
    )
{
    printf("EfiSimpleTextReset is not supported.\n");

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EfiSimpleTextOutputOutputString (
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    CHAR16 *String
    )
{
    UINTN Idx;

    for(Idx = 0; String[Idx] != L'\0'; Idx += 1) {
        putchar((unsigned int)String[Idx]);
    }

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EfiSimpleTextOutputTestString (
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    CHAR *String
    )
{
    printf("EfiSimpleTextTestString is not supported.\n");

    return EFI_SUCCESS;

}

EFI_STATUS
EFIAPI
EfiSimpleTextOutputQueryMode (
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    UINTN ModeNumber,
    UINTN *Columns,
    UINTN *Rows
    )
{
    video_get_rows_cols((unsigned int *)Rows, (unsigned int *)Columns);

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EfiSimpleTextOutputSetMode (
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    UINTN ModeNumber
    )
{
    printf("EfiSimpleTextSetMode is not supported.\n");

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EfiSimpleTextOutputSetAttribute (
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    UINTN Attribute
    )
{
    printf("EfiSimpleTextSetAttribute is not supported.\n");

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EfiSimpleTextOutputClearScreen (
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This
    )
{
    video_console_clear();

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EfiSimpleTextOutputSetCursorPosition (
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    UINTN Column,
    UINTN Row
    )
{
    video_console_set_cursor((unsigned int)Column, (unsigned int) Row);

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EfiSimpleTextOutputEnableCursor (
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    BOOLEAN Visible
    )
{
    video_console_cursor_enable((int)Visible);

    return EFI_SUCCESS;
}
