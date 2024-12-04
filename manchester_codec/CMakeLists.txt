cmake_minimum_required(VERSION 3.13)

# Initialize pico-sdk
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Create the library
add_library(manchester_codec
    sender.cpp
    receiver.cpp
)

# Add include directories
target_include_directories(manchester_codec PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against pico standard library
target_link_libraries(manchester_codec
    pico_stdlib
)

# Enable USB output, disable UART output
pico_enable_stdio_usb(manchester_codec 1)
pico_enable_stdio_uart(manchester_codec 0)