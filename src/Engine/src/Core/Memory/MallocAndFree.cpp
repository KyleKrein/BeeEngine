//
// Created by Александр Лебедев on 16.05.2023.
//

#include <cstdlib>
#include "MallocAndFree.h"


//4 байта unsigned int - размер блока
//1 байт - bool false/true - занят/свободен
//4 байта unsigned int - размер блока до этого

void* bee_malloc(unsigned long long size)
{
    return malloc(size);
}

void bee_free(void* ptr)
{
    free(ptr);
}
