//
// Created by cpasjuste on 28/05/18.
//

#ifndef C2DUI_H
#define C2DUI_H

#define C2DUI_HOME_PATH "./"
#ifdef __PFBA__
#define C2DUI_ROMS_PATHS_MAX 20
#else
#define C2DUI_ROMS_PATHS_MAX 1
#endif

#ifdef __PSP2__
#undef C2DUI_HOME_PATH
#ifdef __PFBA__
#define C2DUI_HOME_PATH "ux0:/data/pfba/"
#else
#define C2DUI_HOME_PATH "ux0:/data/psnes/"
#endif
#define SCR_W   960
#define SCR_H   544
#elif __PS3__
#define SCR_W   1280
#define SCR_H   720
#elif __3DS__
#define SCR_W   400
#define SCR_H   240
#elif __SWITCH__
#define SCR_W   1280
#define SCR_H   720
#else
#define SCR_W   1280
#define SCR_H   720
#endif

#include "c2d.h"
#include "c2dui_option.h"
#include "c2dui_option_menu.h"
#include "c2dui_romlist.h"
#include "c2dui_skin.h"
#include "c2dui_video.h"
#include "c2dui_config.h"

#include "c2dui_ui_menu.h"
#include "c2dui_ui_emu.h"
#include "c2dui_ui_romlist.h"
#include "c2dui_ui_progressbox.h"
#include "c2dui_ui_menu_state.h"
#include "c2dui_ui_main.h"

#endif //C2DUI_H
