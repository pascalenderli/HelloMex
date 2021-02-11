// File:   hello.cpp
// Author: Pascal Enderli
// Date:   Zuerich, 06. Februar 2021

#include "hello.h"

Hello::Hello(int preset) : preset_(preset) {};

int Hello::Compute(int factor)
{
    return preset_ * factor;
}

int Hello::GetPreset()
{
    return preset_;
}
