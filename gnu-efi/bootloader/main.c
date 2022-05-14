#include <efi.h>
#include <efilib.h>
#include <ssb_files.c>
#include <elf.h>

// TODO: Add support for posix-efi

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

	return EFI_SUCCESS; // Exit the UEFI application
}
