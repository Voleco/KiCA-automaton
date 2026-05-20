# 指定最低 CMake 版本要求
cmake_minimum_required(VERSION 3.14)

# 定义项目名称和使用的语言 (C++)
project(KiCA VERSION 1.0 LANGUAGES CXX)

# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==========================================
# 1. 编译核心引擎库 (kica_core)
# ==========================================
# 将 src 目录下的源文件编译为一个静态库 (STATIC) 或共享库 (SHARED)
add_library(kica_core STATIC
    core_engine/src/KicaEngine.cpp
    core_engine/src/RandomRewireStrategy.cpp
    core_engine/src/KicaUtils.cpp
)

# 为库指定头文件搜索路径
target_include_directories(kica_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core_engine/inc>
)

# ==========================================
# 2. 编译一个测试可执行文件
# ==========================================

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp")
    add_executable(kica_test test_main.cpp)
    
    # 将核心引擎库链接到测试程序
    target_link_libraries(kica_test PRIVATE kica_core)
endif()