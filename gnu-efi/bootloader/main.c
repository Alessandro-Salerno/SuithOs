#include <efi.h>
#include <efilib.h>
#include <ssb_files.c>
#include <elf.h>
#include <ssb_graphics.c>


// TODO: Add support for posix-efi

int memcmp(const void *aptr, const void *bptr, long long n)
{
	const unsigned char *a = aptr, *b = bptr;
	for (long long i = 0; i < n; i++)
	{
		if (a[i] < b[i])
			return -1;
		else if (a[i] > b[i])
			return 1;
	}
	return 0;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{

	InitializeLib(ImageHandle, SystemTable); // Initialize EFI library

	EFI_FILE *kernel_file = ssb_load_file(NULL, L"kernel.elf", ImageHandle, SystemTable);

	if (kernel_file == NULL)
	{
		Print(L"Could not load kernel.elf\n");
		return EFI_LOAD_ERROR;
	}
	else
	{
		Print(L"Loaded kernel.elf\n");
	}

	Elf64_Ehdr header;
	{
		UINTN size;
		EFI_FILE_INFO file_info;

		kernel_file->GetInfo(kernel_file, &gEfiFileInfoGuid, &size, NULL);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void **)&file_info);
		kernel_file->GetInfo(kernel_file, &gEfiFileInfoGuid, &size, (void **)&file_info);

		UINTN _size = sizeof(header);
		kernel_file->Read(kernel_file, &_size, &header);
	}

	if (
		memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT)
	{
		Print(L"kernel format is bad\r\n");
	}
	else
	{
		Print(L"kernel header successfully verified\r\n");
	}

	Elf64_Phdr *phdrs;
	{
		kernel_file->SetPosition(kernel_file, header.e_phoff);
		UINTN size = header.e_phnum * header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void **)&phdrs);
		kernel_file->Read(kernel_file, &size, phdrs);
	}

	for (
		Elf64_Phdr *phdr = phdrs;
		(char *)phdr < (char *)phdrs + header.e_phnum * header.e_phentsize;
		phdr = (Elf64_Phdr *)((char *)phdr + header.e_phentsize))
	{
		switch (phdr->p_type)
		{
		case PT_LOAD:
		{
			int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
			Elf64_Addr segment = phdr->p_paddr;
			SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

			kernel_file->SetPosition(kernel_file, phdr->p_offset);
			UINTN size = phdr->p_filesz;
			kernel_file->Read(kernel_file, &size, (void *)segment);
			break;
		}
		}
	}

	Print(L"Kernel Loaded!\r\n");
	Print(L"Loading graphics...\r\n");
	FrameBuffer* framebuffer =  ssb_init_graphics_protocol();
	Print(L"Graphics loaded!\r\n");
	Print(L"Base: %d \n\r", framebuffer->BaseAddress);
	Print(L"Size: %d \n\r", framebuffer->buffer_size);
	Print(L"Width: %d \n\r", framebuffer->width);
	Print(L"Height: %d \n\r", framebuffer->height);
	Print(L"Pixel for scanline: %d \n\r", framebuffer->pixel_for_scanline);
	void (*kernel_start)() = ((__attribute__((sysv_abi)) void (*)() ) header.e_entry);

	unsigned int y = 50;
	unsigned int bbp = 4;


	

	Print(L"Calling kernel start\r\n");
	ssb_clear_screen(framebuffer, 0x004A49);
	
	kernel_start();
	return EFI_SUCCESS; // Exit the UEFI appalication
}
