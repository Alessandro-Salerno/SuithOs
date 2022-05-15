#include <efi.h>
#include <efilib.h>
FrameBuffer frame_buffer;
FrameBuffer* ssb_init_graphics_protocol(){
    EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_STATUS status;

    status = uefi_call_wrapper(BS->LocateProtocol, 3, &gop_guid, NULL, (void **)&gop);

	if(EFI_ERROR(status)){
		Print(L"unable to locate GOP\n\r");
        return NULL;
	}
	else
	{
		Print(L"GOP located\n\r");
	}
    frame_buffer.BaseAddress = (void*)gop->Mode->FrameBufferBase;
    frame_buffer.buffer_size = gop->Mode->FrameBufferSize;
    
    frame_buffer.width = gop->Mode->Info->HorizontalResolution;
    frame_buffer.height = gop->Mode->Info->VerticalResolution;

    frame_buffer.pixel_for_scanline = gop->Mode->Info->PixelsPerScanLine;
    
    return &frame_buffer;
}