# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(security-monitor C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(security-monitor 
    src/security-monitor.c
    src/utils/buttons/button.c
    src/utils/buzzer/buzzer.c
    src/utils/server_connection/server_connection.c
    src/utils/ssd1306/ssd1306.c    
    src/utils/display/display.c
    src/utils/joystick/joystick.c
    src/utils/led/led.c
    src/utils/sensor_pir/sensor_pir.c
    src/utils/wifi/wifi.c
    src/utils/parson/parson.c
)

pico_set_program_name(security-monitor "security-monitor")
pico_set_program_version(security-monitor "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(security-monitor 0)
pico_enable_stdio_usb(security-monitor 1)

# Add the standard library to the build
target_link_libraries(security-monitor
    hardware_timer
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip
    pico_stdlib
    hardware_pwm
    hardware_adc
    hardware_i2c
)

# Add the standard include files to the build
target_include_directories(security-monitor PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/src/utils/buttons
    ${CMAKE_CURRENT_LIST_DIR}/src/utils/buzzer
    ${CMAKE_CURRENT_LIST_DIR}/src/utils/display
    ${CMAKE_CURRENT_LIST_DIR}/src/utils/joystick
    ${CMAKE_CURRENT_LIST_DIR}/src/utils/led
    ${CMAKE_CURRENT_LIST_DIR}/src/utils/sensor_pir
    ${CMAKE_CURRENT_LIST_DIR}/src/utils/server_connection
    ${CMAKE_CURRENT_LIST_DIR}/src/utils/ssd1306
    ${CMAKE_CURRENT_LIST_DIR}/src/utils/wifi
    ${CMAKE_CURRENT_LIST_DIR}/src/utils/parson
)

# Add any user requested libraries
target_link_libraries(security-monitor 
    pico_cyw43_arch_lwip_threadsafe_background
)

pico_add_extra_outputs(security-monitor)

