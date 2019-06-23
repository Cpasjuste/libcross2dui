cmake_minimum_required(VERSION 3.0)
#set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(libcross2d)

if (PLATFORM_LINUX)
    message(FATAL_ERROR "libcross2dui cmake: LINUX")
endif ()

project(cross2dui)

#################
# libcross2dui
#################
set(CROSS2DUI_INCLUDE include)
file(GLOB CROSS2DUI_SOURCE source/*.c*)

if (UPDATE_CONFIG)
    list(APPEND FLAGS -D__UPDATE_CONFIG__)
endif ()

if (OPTION_SSCRAP)
    list(APPEND FLAGS -D__SSCRAP__)
    list(APPEND LDFLAGS ssscrap)
endif ()

if (OPTION_BUILTIN_MINIZIP)
    list(APPEND CROSS2DUI_INCLUDE source)
    list(APPEND CROSS2DUI_SOURCE
            source/minizip/ioapi.c
            source/minizip/miniunz.c
            source/minizip/minizip.c
            source/minizip/unzip.c)
endif ()

if (PLATFORM_VITA)
    #################
    # PSP2 (ps vita)
    #################
    list(APPEND FLAGS -DNO_KEYBOARD)
elseif (PLATFORM_3DS)
    ##############
    # 3DS
    ##############
elseif (PLATFORM_SWITCH)
    ##############
    # SWITCH
    ##############
    list(APPEND FLAGS -DNO_KEYBOARD)
endif ()

##########################
# libcross2dui
##########################
add_library(${PROJECT_NAME} STATIC ${CROSS2DUI_SOURCE})
target_include_directories(${PROJECT_NAME} PUBLIC ${CROSS2DUI_INCLUDE})
target_compile_options(${PROJECT_NAME} PUBLIC ${FLAGS})
target_link_libraries(${PROJECT_NAME} cross2d ${LDFLAGS})
