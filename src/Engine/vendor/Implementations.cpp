//
// Created by alexl on 15.07.2023.
//
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#if defined(WINDOWS)
#define STBI_WINDOWS_UTF8
#endif
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#if defined(WINDOWS)
STBIDEF stbi_uc* stbi_load(const wchar_t* filename, int* x, int* y, int* comp, int req_comp)
{
    FILE* f;
    _wfopen_s(&f, filename, L"rb");
    unsigned char* result;
    if (!f)
        return stbi__errpuc("can't fopen", "Unable to open file");
    result = stbi_load_from_file(f, x, y, comp, req_comp);
    fclose(f);
    return result;
}
int stbi_write_png(const wchar_t* filename, int x, int y, int comp, const void* data, int stride_bytes)
{
    FILE* f;
    int len;
    unsigned char* png = stbi_write_png_to_mem((const unsigned char*)data, stride_bytes, x, y, comp, &len);
    if (png == NULL)
        return 0;

    _wfopen_s(&f, filename, L"wb");
    if (!f)
    {
        STBIW_FREE(png);
        return 0;
    }
    fwrite(png, 1, len, f);
    fclose(f);
    STBIW_FREE(png);
    return 1;
}
#endif
#if defined(BEE_COMPILE_VULKAN)
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
#endif
