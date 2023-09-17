//
// Created by alexl on 15.07.2023.
//
#define STB_IMAGE_IMPLEMENTATION
#if defined(WINDOWS)
#define STBI_WINDOWS_UTF8
#endif
#include "stb_image.h"
#if defined(WINDOWS)
STBIDEF stbi_uc *stbi_load(const wchar_t *filename, int *x, int *y, int *comp, int req_comp)
{
    FILE *f;
    _wfopen_s(&f, filename, L"rb");
    unsigned char *result;
    if (!f) return stbi__errpuc("can't fopen", "Unable to open file");
    result = stbi_load_from_file(f,x,y,comp,req_comp);
    fclose(f);
    return result;
}
#endif

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"