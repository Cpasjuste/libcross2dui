cmake_minimum_required(VERSION 3.0)
#set(CMAKE_VERBOSE_MAKEFILE ON)

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

if (OPTION_SSCRAP)
    list(APPEND FLAGS -D__SSCRAP__)
    list(APPEND LDFLAGS sscrap)
endif ()

if (OPTION_BUILTIN_MINIZIP)
    list(APPEND CROSS2DUI_INCLUDE source)
    list(APPEND CROSS2DUI_SOURCE
            source/minizip/ioapi.c
            source/minizip/miniunz.c
            source/minizip/minizip.c
            source/minizip/unzip.c)
endif ()

##########################
# libcross2dui
##########################
add_library(${PROJECT_NAME} STATIC ${CROSS2DUI_SOURCE})
target_include_directories(${PROJECT_NAME} PUBLIC ${CROSS2DUI_INCLUDE})
target_compile_options(${PROJECT_NAME} PUBLIC ${FLAGS})
target_link_libraries(${PROJECT_NAME} cross2d ${LDFLAGS})
