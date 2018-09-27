#include "create-bootloader.h"
#include "header.h"
#include "crc16.h"


FILE *fNDS;


int copyFromBin(char *binFilename, unsigned int *size, unsigned int *size_without_footer)
{
	FILE *fi = fopen(binFilename, "rb");

	if (!fi)
	{
		fclose(fNDS);
		remove(ndsfilename);
		printf("\nERROR: Could not open binary file:\n%s\n", binFilename);
		return 1;
	}

	unsigned int _size = 0;

	while (1)
	{
		unsigned char buffer[1024];
		int bytesread = fread(buffer, 1, sizeof(buffer), fi);
		if (bytesread <= 0) break;
		fwrite(buffer, 1, bytesread, fNDS);
		_size += bytesread;
	}

	if (size) *size = _size;

	// check footer
	if (size_without_footer)
	{
		fseek(fi, _size - 3*4, SEEK_SET);

		unsigned int nitrocode;

		fread(&nitrocode, sizeof(nitrocode), 1, fi);

		if (nitrocode == 0xDEC00621)
			*size_without_footer = _size - 3*4;
		else
			*size_without_footer = _size;
	}

	fclose(fi);

	return 0;
}


int createBootloader()
{
	struct Header header;

	// initial header data
	// header template
	FILE *fi = fopen(headerfilename, "rb");

	if (!fi)
	{
		printf("\nERROR: Could not open header file:\n%s\n", headerfilename);
		return 1;
	}

	fread(&header, 1, 0x200, fi);

	fclose(fi);

	// header size
	unsigned int header_size = header.rom_header_size;

	if (!header_size)
	{
		header_size = 0x200;
		header.rom_header_size = header_size;
	}

	// Create bootloader file
	fNDS = fopen(ndsfilename, "wb");

	if (!fNDS)
	{
		printf("\nERROR: Could not create bootloader file:\n%s\n", ndsfilename);
		return 1;
	}

	fseek(fNDS, header_size, SEEK_SET);

	// ARM9 binary
	header.arm9_rom_offset = (ftell(fNDS) + 0x1FF) &~ 0x1FF;	// align to 512 bytes
	fseek(fNDS, header.arm9_rom_offset, SEEK_SET);

	unsigned int entry_address = (unsigned int)header.arm9_entry_address;		// template
	unsigned int ram_address = (unsigned int)header.arm9_ram_address;		// template
	if (!ram_address && entry_address) ram_address = entry_address;
	if (!entry_address && ram_address) entry_address = ram_address;
	if (!ram_address) { ram_address = entry_address = 0x02000000; }

	unsigned int size = 0;

	int failed = copyFromBin(arm9filename, 0, &size);

	if (failed) return failed;

	header.arm9_entry_address = entry_address;
	header.arm9_ram_address = ram_address;
	header.arm9_size = ((size + 3) &~ 3);

	// ARM7 binary
	header.arm7_rom_offset = (ftell(fNDS) + 0x1FF) &~ 0x1FF;	// align to 512 bytes
	fseek(fNDS, header.arm7_rom_offset, SEEK_SET);

	entry_address = (unsigned int)header.arm7_entry_address;		// template
	ram_address = (unsigned int)header.arm7_ram_address;		// template
	if (!ram_address && entry_address) ram_address = entry_address;
	if (!entry_address && ram_address) entry_address = ram_address;
	if (!ram_address) { ram_address = entry_address = 0x03800000; }

	size = 0;

	failed = copyFromBin(arm7filename, &size, 0);

	if (failed) return failed;

	header.arm7_entry_address = entry_address;
	header.arm7_ram_address = ram_address;
	header.arm7_size = ((size + 3) &~ 3);

	// banner
	header.banner_offset = (ftell(fNDS) + 0x1FF) &~ 0x1FF;	// align to 512 bytes
	fseek(fNDS, header.banner_offset, SEEK_SET);

	failed = copyFromBin(bannerfilename, 0, 0);

	if (failed) return failed;

	// application end offset
	int pad = ((ftell(fNDS) + 0x3) &~ 0x3) - ftell(fNDS);	// align to 4 bytes
	while (pad--) fputc(0, fNDS);
	header.application_end_offset = ftell(fNDS);

	// fix up header CRCs and write header
	header.logo_crc = calcCRC((unsigned char *)&header + 0xC0, 156);
	header.header_crc = calcCRC((unsigned char *)&header, 0x15E);
	fseek(fNDS, 0, SEEK_SET);
	fwrite(&header, 0x200, 1, fNDS);

	fclose(fNDS);

	printf(" Done.\n");

	return 0;
}
