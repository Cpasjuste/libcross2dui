//
// Created by cpasjuste on 28/05/18.
//

#ifndef C2DUI_H
#define C2DUI_H

#include "c2d.h"
#include "c2dui_option.h"
#include "c2dui_config.h"
#include "c2dui_romlist.h"
#include "c2dui_skin.h"
#include "c2dui_gui_main.h"

#ifdef __PSP2__
#include <psp2/power.h>
#include <psp2/io/dirent.h>

int _newlib_heap_size_user = 192 * 1024 * 1024;
#define SCR_W   960
#define SCR_H   544
#elif __PS3__
#define SCR_W   1280
#define SCR_H   720
#elif __3DS__
#define SCR_W   400
#define SCR_H   240
#elif __NX__
#define SCR_W   1280
#define SCR_H   720
#else
#define SCR_W   1280
#define SCR_H   720
#endif

#define C2DUI_HOME_PATH "./"
#define C2DUI_ROMS_PATHS_MAX 1

#ifdef __PFBA__

#elif __SNES9X__

#endif

#endif //C2DUI_H
