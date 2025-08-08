
#pragma once
#include "printer.hpp"

namespace debug_utils {

template <typename T>
void debug_var(const char* name, const T& value) {
    std::cout << name << ": ";
    print_value(value);
    std::cout << std::endl;
}

} // namespace debug_utils

// 宏放在 namespace 外部，方便直接使用
#define DEBUG(x) debug_utils::debug_var(#x, x)
