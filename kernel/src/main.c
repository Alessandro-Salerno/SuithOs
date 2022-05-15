#include "drivers.c"


void kernel_graphics_clean_monitor(FrameBuffer* framebuffer, unsigned int color){
    unsigned int* framebuffer_ptr = (unsigned int*)framebuffer->BaseAddress;
    unsigned int* end_ptr = (unsigned int*)(framebuffer->BaseAddress + framebuffer->buffer_size);
    while(framebuffer_ptr < end_ptr){
        *framebuffer_ptr = color;
        framebuffer_ptr++;
    }
}
void kernel_start(SystemInfo sysinfo){

}

