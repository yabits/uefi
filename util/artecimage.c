#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PROG_NAME "artecimage"
#define VERSION "1.1"

#define BUFSIZE 512

// Artecboot header, gives information to loader

#define ARTECBOOT_HEADER_MAGIC	0x10ADFACE
#define CURRENT_VERSION		0x0102

#define OS_UNKNOWN		0x00
#define OS_LINUX		0x01
#define OS_WINCE		0x02

#define FLAG_INITRD		0x0001	// if set, the loader will provide initrd to kernel
#define FLAG_FILESYSTEM		0x0002	// if set, the loader will use specified file names
#define FLAG_CMDLINE		0x0004	// if set, the loader will pass the new command line

#define ABOOT_FILE_KERNEL	"/kernel"
#define ABOOT_FILE_INITRD	"/initrd"
#define ABOOT_FILE_HEADER	"/header"

#define DEV_SECTOR_SIZE		512
#define DEV_SECTOR_BITS		9

typedef struct __attribute__ ((packed))
{
	unsigned long	magicHeader;
	unsigned short	bootVersion;
	unsigned short	headerSize;		// also kernel image start
	unsigned long	imageSize;		// NB! since 1.02 is the total image/partition size
	unsigned long	bitFlags;
	unsigned short	osType;
	char		cmdLine[256];
	unsigned long	kernelStart;		// used with Artecboot VFS / NULLFS
	unsigned long	kernelSize;		// used with Artecboot VFS / NULLFS
	unsigned long	initrdStart;		// used with Artecboot VFS / NULLFS
	unsigned long	initrdSize;		// used with Artecboot VFS / NULLFS
	char		kernelFile[100];	// valid only with FLAG_FILESYSTEM
	char		initrdFile[100];	// valid only with FLAG_FILESYSTEM

} ARTECBOOT_HEADER;

void usage(void);

int main(int argc, char **argv){
	ARTECBOOT_HEADER bootHdr;
	FILE *BINFILE = 0;
	FILE *KERNFILE = 0;
	FILE *INITRDFILE = 0;
	char buf[BUFSIZE];
	int c;
	int i;
	int verbose = 0;

	/* clear header */
	memset(&bootHdr, 0, sizeof(ARTECBOOT_HEADER));

	while ((c = getopt(argc, argv, "c:hi:k:o:v")) != -1) {
		switch(c) {
			case 'c':
				if (strlen(optarg) < sizeof(bootHdr.cmdLine)) {
					strcpy(bootHdr.cmdLine, optarg);
					bootHdr.bitFlags |= FLAG_CMDLINE;
				} else {
					usage();
					printf("\nCommand line exceeds %d char limit\n", (sizeof(bootHdr.cmdLine) - 1));
					return 1;
				}
				break;
			case 'h':
				usage();
				return 0;
				break;
			case 'i':
				INITRDFILE = fopen(optarg,"r");
			        if(INITRDFILE == NULL){
					usage();
			                printf("\nError: Failed to open file '%s'\n\n", optarg);
			                return 1;
				}
				break;
			case 'k':
				KERNFILE = fopen(optarg,"r");
			        if(KERNFILE == NULL){
					usage();
			                printf("\nError: Failed to open file '%s'\n\n", optarg);
			                return 1;
				}
				break;
			case 'o':
				BINFILE = fopen(optarg,"w");
			        if(BINFILE == NULL){
					usage();
			                printf("\nError: Failed to open file '%s'\n\n", optarg);
			                return 1;
				}
				break;
			case 'v':
				verbose = 1;
				break;
		}
	}
	if(!BINFILE){
		usage();
		printf("\nError: No output file specified\n\n");
		return 1;
	}

	if(!KERNFILE){
		usage();
		printf("\nError: No kernel image specified\n\n");
		return 1;
	}

	bootHdr.magicHeader = ARTECBOOT_HEADER_MAGIC;
	bootHdr.bootVersion = CURRENT_VERSION;
	bootHdr.headerSize = sizeof(ARTECBOOT_HEADER);

	/* Get kernel image size */
	fseek(KERNFILE, 0L, SEEK_END);
	bootHdr.imageSize = ftell(KERNFILE);

	bootHdr.kernelSize = bootHdr.imageSize;
	bootHdr.kernelStart = DEV_SECTOR_SIZE;

	bootHdr.osType = OS_LINUX;

	if(INITRDFILE){
		/* Get initrd image size */
		fseek(INITRDFILE, 0L, SEEK_END);
		bootHdr.initrdSize = ftell(INITRDFILE);

		/* Set INITRD flag */
		bootHdr.bitFlags |= FLAG_INITRD;

		/* Calculate start address (512 boundary) */
		bootHdr.initrdStart = (bootHdr.kernelStart + bootHdr.imageSize);
		bootHdr.initrdStart = (bootHdr.initrdStart >> DEV_SECTOR_BITS) + 1;
		bootHdr.initrdStart <<= DEV_SECTOR_BITS;
	}

	fwrite(&bootHdr, 1, sizeof(ARTECBOOT_HEADER), BINFILE);
	fseek(BINFILE, bootHdr.kernelStart, SEEK_SET);

	/* Copy kernel into output file */
	fseek(KERNFILE, 0, SEEK_SET);
	while((i = fread(&buf, 1, BUFSIZE, KERNFILE)) != 0){
		fwrite(&buf, 1, i, BINFILE);
	}
	fclose(KERNFILE);

	if(INITRDFILE){
		fseek(BINFILE, bootHdr.initrdStart, SEEK_SET);

		/* Copy initrd into output file */
		fseek(INITRDFILE, 0, SEEK_SET);
		while((i = fread(&buf, 1, BUFSIZE, INITRDFILE)) != 0){
			fwrite(&buf, 1, i, BINFILE);
		}
		fclose(INITRDFILE);
	}

	fclose(BINFILE);

	if(verbose){
		printf("magicHeader: 0x%04lx\n", bootHdr.magicHeader);
		printf("bootVersion: 0x%04x\n", bootHdr.bootVersion);
		printf("headerSize:  0x%08x\n", bootHdr.headerSize);
		printf("imageSize:   0x%08lx\n", bootHdr.imageSize);
		printf("bitFlags:    0x%08lx\n", bootHdr.bitFlags);
		printf("osType:      0x%04x\n", bootHdr.osType);
		printf("cmdLine:     %s\n", bootHdr.cmdLine);
		printf("kernelStart: 0x%08lx\n", bootHdr.kernelStart);
		printf("kernelSize:  0x%08lx\n", bootHdr.imageSize);
		printf("initrdStart: 0x%08lx\n", bootHdr.initrdStart);
		printf("initrdSize:  0x%08lx\n", bootHdr.initrdSize);
		printf("kernelFile:  %s\n", bootHdr.kernelFile);
		printf("initrdFile:  %s\n", bootHdr.initrdFile);
	}
	return 0;
}

void usage(void) {
	printf(PROG_NAME " v" VERSION "\n"
	       "Usage: " PROG_NAME " [-hv] [-c cmdline] [-i initrd] -k kernel -o outfile\n\n"
	       "Options:\n"
	       "  -c cmdline  Kernel command line (max 255 chars).\n"
	       "  -h          Display this help.\n"
	       "  -i initrd   initrd filename.\n"
	       "  -k kernel   Linux kernel filename.\n"
	       "  -o outfile  Write image to outfile.\n"
	       "  -v          Verbose.\n"
	);
	return;
}
