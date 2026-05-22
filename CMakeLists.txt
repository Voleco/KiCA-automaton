# 指定最低 CMake 版本要求
cmake_minimum_required(VERSION 3.14)

project(KiCA VERSION 1.0 LANGUAGES CXX)

# 全局设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O2)
elseif(MSVC)
    add_compile_options(/O2)
endif()

# ==========================================
# 引入各个子模块
# ==========================================
add_subdirectory(core_engine)
add_subdirectory(graph_utils)

# ==========================================
# 编译测试可执行文件 (test_main.cpp 在根目录)
# ==========================================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp")
    add_executable(kica_test test_main.cpp)
    
    # 链接图工具库 (由于 PUBLIC 属性，kica_core 会被自动链接)
    target_link_libraries(kica_test PRIVATE graph_utils)
endif()