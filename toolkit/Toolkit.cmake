# Toolkit.cmake
#
# 2023

include_guard()

# Options
option(TOOLKIT_ENABLED "Enable toolkit which extends CryptoAuth library" OFF)

if (NOT TOOLKIT_ENABLED)
    return()
endif()

target_include_directories(cryptoauth
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src
)

target_sources(cryptoauth
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/toolkit/tk_api_log_int.c
        ${CMAKE_CURRENT_LIST_DIR}/src/toolkit/tk_atca_debug.c
        ${CMAKE_CURRENT_LIST_DIR}/src/toolkit/tk_atca_hid_device_factory.c
)

if (WIN32)
    target_sources(cryptoauth
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/src/toolkit/windows/tk_ctp_board.c
    )

    target_link_libraries(cryptoauth cfgmgr32.lib)
else()
    message(WARNING "Enumeration of CryptoAuth Trust Platform boards currently supported only on \{ WIN32 \} platform(s) !")
endif()

if (DEFAULT_INC_PATH)
    file(GLOB TOOLKIT_INC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "include/toolkit/*.h")
    install(FILES ${TOOLKIT_INC} DESTINATION ${DEFAULT_INC_PATH}/toolkit COMPONENT Development)
endif()
