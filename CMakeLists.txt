cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent) 

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)
project(HandDexterity C CXX ASM)

pico_sdk_init()

# FreeRTOS ===========================================================================
FetchContent_Declare( freertos_kernel
  GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
  GIT_TAG        main
)

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM
INTERFACE
    include
)

set( FREERTOS_HEAP "4" CACHE STRING "" FORCE)
set( FREERTOS_PORT "GCC_RP2040" CACHE STRING "" FORCE)
if (CMAKE_CROSSCOMPILING)
  set(FREERTOS_PORT "GCC_RP2040" CACHE STRING "" FORCE)
endif()

FetchContent_MakeAvailable(freertos_kernel)
# ====================================================================================

add_executable(HandDexterity HandDexterity.c 
                src/leds.c
                src/display.c
                src/joystick.c
                src/learn.c
                src/ssd1306.c)

pico_set_program_name(HandDexterity "HandDexterity")
pico_set_program_version(HandDexterity "0.1")

pico_enable_stdio_uart(HandDexterity 0)
pico_enable_stdio_usb(HandDexterity 1)

target_link_libraries(HandDexterity
        pico_stdlib)

target_include_directories(HandDexterity PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(HandDexterity 
        hardware_adc
        hardware_i2c
        freertos_kernel
        freertos_kernel_include
        )

pico_add_extra_outputs(HandDexterity)

