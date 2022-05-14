#include <efi.h>
#include <efilib.h>

EFI_FILE *ssb_load_file(EFI_FILE *directory, CHAR16 *path, EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table)
{
    EFI_FILE *file = NULL;
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image_prt;

    system_table->BootServices->HandleProtocol(image_handle, &gEfiLoadedImageProtocolGuid, (void **)&loaded_image_prt);

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs_prt;

    system_table->BootServices->HandleProtocol(loaded_image_prt->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void **)&fs_prt);

    if (directory == NULL)
    {
        fs_prt->OpenVolume(fs_prt, &directory);
    }

    EFI_STATUS status = directory->Open(directory, &file, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

    if (status != EFI_SUCCESS)
    {
        Print(L"Failed to open file: %s\n", path);
        return NULL;
    }

    return file;
}