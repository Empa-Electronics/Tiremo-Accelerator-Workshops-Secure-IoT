idf_component_register(
    INCLUDE_DIRS
        include
        ../micro-ecc

    REQUIRES
        esp_hw_support
        freertos
        esp_system
        esp_event
        esp_wifi
        esp_netif
        lwip
        nvs_flash
        mqtt
        bt
        esp_http_client
        esp_https_ota
        mbedtls
        esp-cryptoauthlib
        json
        app_update
)

# target_link_libraries(${COMPONENT_LIB}
#     INTERFACE
#         "-Wl,--whole-archive"
#         "${CMAKE_CURRENT_LIST_DIR}/libcyberwhiz-core-lib.a"
#         "-Wl,--no-whole-archive"
# )

# Prebuilt .a dosyasını component'e eklemenin DOĞRU yolu
# set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY
#     INTERFACE_LINK_LIBRARIES
#     "${CMAKE_CURRENT_LIST_DIR}/libcyberwhiz-core-lib.a"
# )

target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=calib_get_pubkey -u __wrap_calib_get_pubkey")
target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=calib_sign_ext -u __wrap_calib_sign_ext")