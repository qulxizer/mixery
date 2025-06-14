cmake_minimum_required(VERSION 4.0)

set(PICO_SDK_PATH "${CMAKE_SOURCE_DIR}/pico-sdk")
include(pico_sdk_import.cmake)
project(mixery)


pico_sdk_init()

add_executable(mixery
        main.c
)



# pull in common dependencies
target_link_libraries(mixery pico_stdlib)
pico_enable_stdio_usb(mixery 1)


# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(mixery)

