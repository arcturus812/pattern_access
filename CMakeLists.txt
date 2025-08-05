cmake_minimum_required(VERSION 3.15)
project(pattern_access VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard to C++14 for std::make_unique support
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(nlohmann_json REQUIRED)

# Set CLI11 path explicitly
set(CLI11_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-linux/share/cli11")
find_package(CLI11 CONFIG REQUIRED)

# Add executable with all source files
add_executable(micro-access 
    src/main.cc
    src/access_wrapper.cc
    src/sequential_access.cc
    src/stride_access.cc
    src/dynamic_random_access.cc
)

# Set compiler options for the target
target_compile_options(micro-access PRIVATE -O0 -g -Wall)

# Include directories
target_include_directories(micro-access PRIVATE inc)

# Check for NUMA library in vcpkg packages
set(NUMA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/packages/numactl_x64-linux/include")
set(NUMA_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/packages/numactl_x64-linux/lib/libnuma.a")

if(EXISTS ${NUMA_INCLUDE_DIR}/numa.h AND EXISTS ${NUMA_LIBRARY})
    message(STATUS "Found NUMA library in vcpkg packages")
    target_include_directories(micro-access PRIVATE ${NUMA_INCLUDE_DIR})
    target_link_libraries(micro-access PRIVATE 
        nlohmann_json::nlohmann_json
        CLI11::CLI11
        ${NUMA_LIBRARY}
    )
    target_compile_definitions(micro-access PRIVATE HAVE_NUMA=1)
else()
    # Try to find system NUMA library
    find_library(NUMA_LIB numa)
    if(NUMA_LIB)
        message(STATUS "Found system NUMA library: ${NUMA_LIB}")
        target_link_libraries(micro-access PRIVATE 
            nlohmann_json::nlohmann_json
            CLI11::CLI11
            ${NUMA_LIB}
        )
        target_compile_definitions(micro-access PRIVATE HAVE_NUMA=1)
    else()
        message(WARNING "NUMA library not found. Building without NUMA support.")
        target_link_libraries(micro-access PRIVATE 
            nlohmann_json::nlohmann_json
            CLI11::CLI11
        )
        target_compile_definitions(micro-access PRIVATE HAVE_NUMA=0)
    endif()
endif()

# Set output directory
set_target_properties(micro-access PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
) 
