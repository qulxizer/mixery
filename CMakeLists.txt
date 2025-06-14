cmake_minimum_required(VERSION 4.0)

set(PICO_SDK_PATH "${CMAKE_SOURCE_DIR}/pico-sdk")
include(pico_sdk_import.cmake)
project(mixery)


pico_sdk_init()

add_executable(mixery
)


pico_enable_stdio_usb(mixery 1)


target_sources(mixery PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c)

target_include_directories(mixery PRIVATE lib/cJSON)
target_link_libraries(mixery 
PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board)
target_include_directories(mixery PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})
pico_enable_stdio_usb(mixery 1)


# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(mixery)

