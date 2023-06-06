cmake_minimum_required(VERSION 3.0)

############################################################
# Project
############################################################

project(bqqenv
    DESCRIPTION "BetterQQNT Launcher"
    HOMEPAGE_URL "https://github.com/BetterQQNT/BetterQQNT-Launcher"
    LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE)

############################################################
# Download BetterQQNT
############################################################

set(
    BETTERQQNT_VERSION
    "0.0.0"
    CACHE
    STRING
    "BetterQQNT Version"
)

set(
    BETTERQQNT_ARCH
    "x86"
    CACHE
    STRING
    "BetterQQNT Architecture"
)

if (BETTERQQNT_ARCH MATCHES "x64")
    set(
        BETTERQQNT_FILENAME
        "better-qqnt-no-hijack-x64.dll"
    )
else()
    set(
        BETTERQQNT_FILENAME
        "better-qqnt-no-hijack.dll"
    )
endif()

set(
    BETTERQQNT_URL
    "https://github.com/BetterQQNT/BetterQQNT/releases/download/${BETTERQQNT_VERSION}/${BETTERQQNT_FILENAME}"
)

set(
    BETTERQQNT_PATH
    "${PROJECT_SOURCE_DIR}/build/better-qqnt-no-hijack.dll"
)

if (NOT EXISTS BETTERQQNT_PATH)
    file(
        DOWNLOAD
            ${BETTERQQNT_URL}
            ${BETTERQQNT_PATH}
            SHOW_PROGRESS
    )
endif()

############################################################
# App
############################################################

add_executable(bqqinj
    WIN32
        src/common.cpp
        src/inj.cpp
        src/inj.rc
)

target_include_directories(bqqinj
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

add_executable(bqqenv
    WIN32
        src/common.cpp
        src/env.cpp
        src/env.rc
)

target_include_directories(bqqenv
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

############################################################
# Compile Flags
############################################################

target_compile_definitions(bqqinj
    PRIVATE
        UNICODE
        _UNICODE
        WIN32
        _WIN32
        WINDOWS
        _WINDOWS
)

target_compile_definitions(bqqenv
    PRIVATE
        UNICODE
        _UNICODE
        WIN32
        _WIN32
        WINDOWS
        _WINDOWS
)

target_compile_options(bqqinj
    PRIVATE
        /ZI
        /utf-8
)

target_compile_options(bqqenv
    PRIVATE
        /ZI
        /utf-8
)

target_link_options(bqqinj
    PRIVATE
        /INCREMENTAL
)

target_link_options(bqqenv
    PRIVATE
        /INCREMENTAL
)
