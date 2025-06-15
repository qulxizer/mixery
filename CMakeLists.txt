cmake_minimum_required(VERSION 3.13)


# Setup SDK
include("pico_sdk_import.cmake")
project(mixery)
pico_sdk_init()

# Executable
file(GLOB SRC_FILES "${PROJECT_SOURCE_DIR}/src/*.c")
add_executable(mixery)
target_sources(mixery PUBLIC ${SRC_FILES})

# Includes
target_include_directories(mixery PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(mixery PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board)

# Enable USB stdio
pico_enable_stdio_usb(mixery 1)

# Generate bin/hex/uf2
pico_add_extra_outputs(mixery)

