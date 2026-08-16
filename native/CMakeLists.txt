cmake_minimum_required(VERSION 3.20)

project(LumenTerrainEngine)

set(CMAKE_CXX_STANDARD 20)


find_package(OpenCL REQUIRED)


find_package(Java REQUIRED)
find_package(JNI REQUIRED)

include_directories(
    ${JNI_INCLUDE_DIRS}
)

add_library(LumenTerrainEngine SHARED

    src/LTE.cpp
    src/Logger.cpp
    src/HardwareManager.cpp
    src/BenchmarkManager.cpp
    src/LTENative.cpp
    src/OpenCLManager.cpp
    src/OpenCLCompute.cpp
)

target_include_directories(
    LumenTerrainEngine
    PUBLIC
    include
)

target_link_libraries(
    LumenTerrainEngine
    OpenCL::OpenCL
)

add_executable(LTE_Test
    src/test.cpp
)

target_link_libraries(
    LTE_Test
    LumenTerrainEngine
)
