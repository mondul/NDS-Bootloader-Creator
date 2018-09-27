#include "create-bootloader.h"


int main(int argc, char const *argv[])
{
	// Check if all arguments were entered. If not, show message
	if (argc != 11)
	{
		printf("Nintendo DSi Bootloader Creator for HiyaCFW Helper\nv1.0 based off devkitPro's ndstool v1.27\n\nParameter        Syntax\n---------        ------\nCreate           -c bootloader.nds\nARM7 executable  -7 arm7.bin\nARM9 executable  -9 arm9.bin\nHeader template  -h header.bin\nBanner binary    -t banner.bin\n\n");
		return 0;
	}

	// Fill parameters
	unsigned char i = 1;
	do
	{
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
				case 'c':
					ndsfilename = (char *)argv[i + 1];
					break;

				case '7':
					arm7filename = (char *)argv[i + 1];
					break;

				case '9':
					arm9filename = (char *)argv[i + 1];
					break;

				case 'h':
					headerfilename = (char *)argv[i + 1];
					break;

				case 't':
					bannerfilename = (char *)argv[i + 1];
					break;
			}

			i += 2;
		}

		else {
			printf("ERROR: Misplaced argument\n");
			return 1;
		}
	}
	while (i < 11);

	// Check parameters
	if ((ndsfilename == NULL) || (ndsfilename[0] == '\0'))
	{
		printf("ERROR: Output file name missing\n");
		return 1;
	}

	else if ((arm7filename == NULL) || (arm7filename[0] == '\0'))
	{
		printf("ERROR: ARM7 executable file name missing\n");
		return 1;
	}

	else if ((arm9filename == NULL) || (arm9filename[0] == '\0'))
	{
		printf("ERROR: ARM9 executable file name missing\n");
		return 1;
	}

	else if ((headerfilename == NULL) || (headerfilename[0] == '\0'))
	{
		printf("ERROR: Header template file name missing\n");
		return 1;
	}

	else if ((bannerfilename == NULL) || (bannerfilename[0] == '\0'))
	{
		printf("ERROR: Banner binary file name missing\n");
		return 1;
	}

	printf("Creating bootloader...");

	return createBootloader();
}
