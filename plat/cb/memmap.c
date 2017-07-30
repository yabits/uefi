
#include "ueficore.h"
#include <libpayload.h>

#define LB_MEM_RAM		 1	/* Memory anyone can use */
#define LB_MEM_RESERVED		 2	/* Don't use this memory region */
#define LB_MEM_ACPI		 3	/* ACPI Tables */
#define LB_MEM_NVS		 4	/* ACPI NVS Memory */
#define LB_MEM_UNUSABLE		 5	/* Unusable address space */
#define LB_MEM_VENDOR_RSVD	 6	/* Vendor Reserved */
#define LB_MEM_TABLE		16    /* Ram configuration tables are kept in */

EFI_MEMORY_DESCRIPTOR EfiCorebootMemoryMap[SYSINFO_MAX_MEM_RANGES];

EFI_STATUS
EfiPlatformGetInitialMemoryMap (
    EFI_MEMORY_DESCRIPTOR **Map,
    UINTN *MapSize
    )
{
    EFI_PHYSICAL_ADDRESS BaseAddress;
    EFI_MEMORY_DESCRIPTOR Descriptor;
    EFI_MEMORY_TYPE DescriptorType;
    UINT64 Length;
    UINTN Size;
    int ret;
    int i;

    ret = lib_get_sysinfo();
    if (ret)
        goto fail;

    Size = lib_sysinfo.n_memranges;
    for (i=0; i<lib_sysinfo.n_memranges; i++) {
        BaseAddress = lib_sysinfo.memrange[i].base;
        Length = lib_sysinfo.memrange[i].size;

        switch (lib_sysinfo.memrange[i].type) {
            case LB_MEM_RAM:
                DescriptorType = EfiConventionalMemory;
                break;

            case LB_MEM_RESERVED:
                DescriptorType = EfiRuntimeServicesData;
                break;

            case LB_MEM_ACPI:
                DescriptorType = EfiACPIReclaimMemory;
                break;

            case LB_MEM_NVS:
                DescriptorType = EfiACPIMemoryNVS;
                break;

            case LB_MEM_UNUSABLE:
                DescriptorType = EfiUnusableMemory;
                break;

            case LB_MEM_VENDOR_RSVD:
                DescriptorType = EfiRuntimeServicesData;
                break;

            case LB_MEM_TABLE:
                DescriptorType = EfiRuntimeServicesData;
                break;
            default:
                continue;
        }

        Descriptor.Type = DescriptorType;
        Descriptor.Padding = 0;
        Descriptor.PhysicalStart = BaseAddress;
        Descriptor.VirtualStart = 0;
        Descriptor.NumberOfPages = EFI_SIZE_TO_PAGES(Length);
        Descriptor.Attribute = 0;
        EfiCorebootMemoryMap[i] = Descriptor;
    }

    *Map = EfiCorebootMemoryMap;
    *MapSize = Size;

    return EFI_SUCCESS;

fail:
    printf("Unable to find coreboot table!\n");
    return;
}

