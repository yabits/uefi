#include "ueficore.h"
#include <libpayload.h>

int main()
{
    // all arguments are dummy
    EfiCoreMain(
            CONFIG_LP_BASE_ADDRESS,
            CONFIG_LP_BASE_ADDRESS,
            4096,
            "UEFI",
            4096,
            CONFIG_LP_STACK_SIZE
            );

    // do not reach here
    halt();

    return 0;
}
