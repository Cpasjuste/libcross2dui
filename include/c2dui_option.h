//
// Created by cpasjuste on 05/12/16.
//

#ifndef _C2DUI_OPTION_H_
#define _C2DUI_OPTION_H_

#include <string>
#include <vector>

#define BIT(n) (1U<<(n))

namespace c2dui {

    class C2DUIOption {

    public:

        enum Type {
            MENU = BIT(0),
            INTEGER = BIT(1),
            INPUT = BIT(2),
            HIDDEN = BIT(3)
        };

        enum Index {
            MENU_MAIN = 0,
            GUI_SHOW_ALL,
            GUI_SHOW_CLONES,
            GUI_SHOW_HARDWARE,
            GUI_SHOW_ICONS,
            GUI_USE_DATABASE,
            GUI_FULLSCREEN,
#ifdef __SWITCH__
            JOY_SINGLEJOYCON,
#endif
            MENU_SKIN,
            SKIN_FONT_SIZE,
            MENU_ROM_OPTIONS,
            ROM_SCALING,
            ROM_FILTER,
            ROM_SHADER,
            ROM_ROTATION,
            ROM_SHOW_FPS,
            ROM_FRAMESKIP,
            ROM_NEOBIOS,
            ROM_AUDIO,
            ROM_HIGH_RES,
            ROM_CHEATS,
            MENU_JOYPAD,
            JOY_UP,
            JOY_DOWN,
            JOY_LEFT,
            JOY_RIGHT,
            JOY_FIRE1,
            JOY_FIRE2,
            JOY_FIRE3,
            JOY_FIRE4,
            JOY_FIRE5,
            JOY_FIRE6,
            JOY_COIN1,
            JOY_START1,
            JOY_AXIS_LX,
            JOY_AXIS_LY,
            JOY_AXIS_RX,
            JOY_AXIS_RY,
            JOY_DEADZONE,
#ifndef NO_KEYBOARD
            MENU_KEYBOARD,
            KEY_UP,
            KEY_DOWN,
            KEY_LEFT,
            KEY_RIGHT,
            KEY_FIRE1,
            KEY_FIRE2,
            KEY_FIRE3,
            KEY_FIRE4,
            KEY_FIRE5,
            KEY_FIRE6,
            KEY_COIN1,
            KEY_START1,
#endif
        };

        C2DUIOption(const std::string &text, const std::vector<std::string> &options,
                    int defaultValue, Index idx, unsigned int flags = INTEGER);

        unsigned int flags = INTEGER;
        int index = 0;
        int value = 0;

        const char *getName();

        const char *getValue();

        void next();

        void prev();

    private:
        std::string text;
        std::vector<std::string> options;
    };
}

#endif //_C2DUI_OPTION_H_
