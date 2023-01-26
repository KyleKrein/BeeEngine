//
// Created by Александр Лебедев on 23.01.2023.
//

#pragma once

#include <iostream>

#define BeeAssert(condition, message) LogToConsole(condition, message)

void LogToConsole(bool condition, char* message)
{
    if(condition)
        return;
    std::cout<<message<<std::endl;
}
