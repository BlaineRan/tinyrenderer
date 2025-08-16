cmake_minimum_required(VERSION 3.10)   # 够新就行，3.10 支持广泛
project(TgaViewer LANGUAGES CXX)

# C++17/20/23 随你选；这里示范 17
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)          # 不用 gnu 扩展，保持可移植

# 如果想要把 tgaimage 拆成库，可用 add_library；简单起见直接一起编
add_executable(tgademo
    main.cpp
    tgaimage.cpp
    model.cpp
)

# 头文件就在同一目录，无需额外 include；如果以后换目录可这样写：
# target_include_directories(tgademo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 可选：常见编译警告开关
if (MSVC)
    target_compile_options(tgademo PRIVATE /W4 /permissive-)
else()
    target_compile_options(tgademo PRIVATE -Wall)
endif()
