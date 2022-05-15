#include <efi.h>
#include <efilib.h>
#include <ssb_files.c>
#include <elf.h>

// TODO: Add support for posix-efi

int memcmp(const void* aptr, const void* bptr, long long n){
	const unsigned char* a = aptr, *b = bptr;
	for (long long i = 0; i < n; i++){
		if (a[i] < b[i]) return -1;
		else if (a[i] > b[i]) return 1;
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
		kernel_file->GetInfo(kernel_file, &gEfiFileInfoGuid, &size, (void**)&file_info);

		UINTN _size = sizeof(header);
		kernel_file->Read(kernel_file, &_size, &header);


	}

	if (
		memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT
	)
	{
		Print(L"kernel format is bad\r\n");
	}
	else
	{
		Print(L"kernel header successfully verified\r\n");
	}


	return EFI_SUCCESS; // Exit the UEFI application
}
