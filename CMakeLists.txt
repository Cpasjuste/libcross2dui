cmake_minimum_required(VERSION 3.0)
#set(CMAKE_VERBOSE_MAKEFILE ON)

# delete cmake cache folder before changing this options
option(BUILD_LINUX "Build with Linux support" OFF)
option(BUILD_WINDOWS "Build with Windows support" OFF)
option(BUILD_VITA "Build with PSP2 support" OFF)
option(BUILD_3DS "Build with 3DS support" OFF)
option(BUILD_SWITCH "Build with Nintendo Switch support OFF")

option(UPDATE_CONFIG "Force config file updating" OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_DEBUG true CACHE BOOL "Debug build")
endif ()

add_subdirectory(libcross2d)

project(cross2dui)

#################
# libcross2dui
#################
set(CROSS2DUI_INCLUDE include)
file(GLOB CROSS2DUI_SOURCE source/*.c*)

if (UPDATE_CONFIG)
    list(APPEND FLAGS -D__UPDATE_CONFIG__)
endif ()

if (BUILD_VITA)
    #################
    # PSP2 (ps vita)
    #################
    list(APPEND FLAGS -DNO_KEYBOARD)
elseif (BUILD_3DS)
    ##############
    # 3DS
    ##############
elseif (BUILD_SWITCH)
    ##############
    # SWITCH
    ##############
    list(APPEND FLAGS -DNO_KEYBOARD)
endif (BUILD_VITA)

##########################
# libcross2dui
##########################
add_library(${PROJECT_NAME} ${CROSS2DUI_SOURCE})
target_include_directories(${PROJECT_NAME} PUBLIC ${CROSS2DUI_INCLUDE})
target_compile_options(${PROJECT_NAME} PUBLIC ${FLAGS})
target_link_libraries(${PROJECT_NAME} cross2d)
