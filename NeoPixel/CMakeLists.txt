cmake_minimum_required(VERSION 3.13)

# initialize the SDK directly
include(/home/saiffullah/Projects/RP_Pico/pico-sdk/pico_sdk_init.cmake)

project(my_project)
enable_language( C CXX ASM )

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(pio_ws2812)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)

# generate the header file into the source tree as it is included in the RP2040 datasheet
pico_generate_pio_header(pio_ws2812 ${CMAKE_CURRENT_LIST_DIR}/ws2812b.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

pico_enable_stdio_uart(pio_ws2812 1)
pico_enable_stdio_usb(pio_ws2812 0)

target_sources(pio_ws2812 PRIVATE ws2812b.c)

target_link_libraries(pio_ws2812 PRIVATE pico_stdlib hardware_pio)
pico_add_extra_outputs(pio_ws2812)
