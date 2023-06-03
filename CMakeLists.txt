cmake_minimum_required(VERSION 3.0)

############################################################
# Project
############################################################

project(bqqenv
    VERSION 0.1.0
    DESCRIPTION "BetterQQNT Launcher"
    HOMEPAGE_URL "https://github.com/koishi-nt/BetterQQNT-Launcher"
    LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE)

############################################################
# App
############################################################

add_executable(bqqenv
    WIN32
        src/main.cpp
)

target_include_directories(bqqenv
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

############################################################
# Compile Flags
############################################################

target_compile_definitions(bqqenv
    PRIVATE
        UNICODE
        _UNICODE
        WIN32
        _WIN32
        WINDOWS
        _WINDOWS
)

if (MSVC)
    target_compile_options(bqqenv
        PRIVATE
            /utf-8
    )
else()
    target_compile_options(bqqenv
        PRIVATE
            -fexec-charset=UTF-8
            -finput-charset=UTF-8
    )
endif()

############################################################
# Support Visual Studio "Edit and Continue"
############################################################

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    target_compile_options(bqqenv
        PRIVATE
            /ZI
    )

    target_link_options(bqqenv
        PRIVATE
            /INCREMENTAL
    )
endif()
