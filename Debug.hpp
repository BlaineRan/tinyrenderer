#pragma once
#include <iostream>
#include <string>
#include "Vec.h"

namespace Debug {

template <typename T>
void debug_var_endl(const char* name, const T& value) {
    std::cout << name << ": ";
    std::cout << value;
    std::cout << std::endl;
}
template <typename T>
void debug_var(const char* name, const T& value) {
    std::cout << name << ": ";
    std::cout << value;
    std::cout << " ";
}


void debug_var_endl(const char* name, const Vec2i& value) {
    std::cout << name << ": ";
    std::cout << value.x << " " << value.y;
    std::cout << std::endl;
}

void debug_var(const char* name, const Vec2i& value) {
    std::cout << name << ": ";
    std::cout << value.x << " " << value.y;
    std::cout << " ";
}
} 

// 宏放在 namespace 外部，方便直接使用
#define DEBUG_ENDL(x) Debug::debug_var_endl(#x, x)
#define DEBUG(x) Debug::debug_var(#x, x)