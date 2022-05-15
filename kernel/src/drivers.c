typedef struct
{
    void* BaseAddress;
    long long buffer_size;
    unsigned int width;
    unsigned int height;
    unsigned int pixel_for_scanline;
} FrameBuffer;

typedef struct
{
    FrameBuffer* framebuffer;
} SystemInfo;

