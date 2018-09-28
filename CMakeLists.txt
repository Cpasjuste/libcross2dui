cmake_minimum_required(VERSION 3.0)
#set(CMAKE_VERBOSE_MAKEFILE ON)

# delete cmake cache folder before changing this options
option(BUILD_SFML "Build with SFML support (hardware rendering, fast)" OFF)
option(BUILD_SDL1 "Build with SDL1 support (software, osmesa) support" OFF)
option(BUILD_SDL1_GL "Build with SDL1 support (hardware rendering, fast, needs BUILD_SDL1=ON)" OFF)
option(BUILD_SDL2 "Build with SDL2 support (software)" OFF)
option(BUILD_SDL2_GL "Build with SDL2 support (hardware rendering, fast, needs BUILD_SDL2=ON)" OFF)
option(BUILD_PSP2 "Build with PSP2 support (hardware rendering, fast)" OFF)
option(BUILD_PS3 "Build with PS3 support (hardware rendering, fast)" OFF)
option(BUILD_3DS "Build with 3DS support" OFF)
option(BUILD_NX "Build with NX (SWITCH) support" OFF)

option(UPDATE_CONFIG "Force config file updating" OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_DEBUG true CACHE BOOL "Debug build")
endif ()

# add cross2d library. If you want to port libcross2dui to another platform,
# you should (may) only need to port this library.
add_subdirectory(libcross2d)

project(cross2dui)

#################
# libcross2dui
#################
set(CROSS2DUI_INCLUDE include)
file(GLOB CROSS2DUI_SOURCE source/*.c*)

##############
# libconfig
##############
set(LIBCONFIG_INCLUDE libconfig)
file(GLOB LIBCONFIG_SOURCE
        libconfig/grammar.c
        libconfig/libconfig.c
        libconfig/scanctx.c
        libconfig/scanner.c
        libconfig/strbuf.c
        )

if (UPDATE_CONFIG)
    list(APPEND FLAGS -D__UPDATE_CONFIG__)
endif ()

if (BUILD_PSP2)
    #################
    # PSP2 (ps vita)
    #################
    list(APPEND FLAGS -DNO_KEYBOARD)
elseif (BUILD_PS3)
    ##############
    # PS3
    ##############
elseif (BUILD_3DS)
    ##############
    # 3DS
    ##############
elseif (BUILD_NX)
    ##############
    # NX (SWITCH)
    ##############
    list(APPEND FLAGS -DNO_KEYBOARD)
elseif (BUILD_SDL2)
    ##############
    # SDL2
    ##############
elseif (BUILD_SDL1)
    ##############
    # SDL1
    ##############
elseif (BUILD_SFML)
    ##############
    # SFML
    ##############
endif (BUILD_PSP2)

##########################
# libcross2dui
##########################
add_library(${PROJECT_NAME} ${CROSS2DUI_SOURCE} ${LIBCONFIG_SOURCE})
target_include_directories(${PROJECT_NAME} PUBLIC ${CROSS2DUI_INCLUDE} ${LIBCONFIG_INCLUDE})
target_compile_options(${PROJECT_NAME} PUBLIC ${FLAGS})
#target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++11 -fpermissive>)
target_link_libraries(${PROJECT_NAME} cross2d)
