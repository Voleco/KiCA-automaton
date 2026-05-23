# 将 src 目录下的源文件编译为静态库
add_library(kica_core STATIC
    src/KicaEngine.cpp
    src/RewireStrategies.cpp
    src/KicaUtils.cpp
)

target_include_directories(kica_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/3rdparty>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)