cmake_minimum_required(VERSION 3.16)

# set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

#include_directories(BEFORE "${CMAKE_SOURCE_DIR}/components/cyberwhiz-core-lib/include")

project(empa-cyberwhiz-app)

target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/certs/root_cert_auth.pem" TEXT)


set(CUSTOM_MBEDTLS_CONFIG_FILE "${CMAKE_SOURCE_DIR}/components/cyberwhiz-core-lib/include/custom_mbedtls_config.h")
add_definitions(-DMBEDTLS_USER_CONFIG_FILE=\"${CUSTOM_MBEDTLS_CONFIG_FILE}\")

target_link_libraries(${CMAKE_PROJECT_NAME}.elf PRIVATE
    "-Wl,--whole-archive"
    ${CMAKE_SOURCE_DIR}/components/cyberwhiz-core-lib/libcyberwhiz-core-lib.a
    "-Wl,--no-whole-archive"
)