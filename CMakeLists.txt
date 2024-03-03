cmake_minimum_required(VERSION 3.10)

project(rpi5-rp1-spi)

set(CMAKE_C_STANDARD 11)
set(SOURCE_DIR "src")

add_executable(${PROJECT_NAME} 
    ${SOURCE_DIR}/rpi5-rp1-spi.c 
    ${SOURCE_DIR}/rp1-spi.c
    ${SOURCE_DIR}/rp1-spi-util.c)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)