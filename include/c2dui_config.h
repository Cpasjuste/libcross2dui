//
// Created by cpasjuste on 05/12/16.
//

#ifndef _C2DUI_CONFIG_H_
#define _C2DUI_CONFIG_H_

#include "libconfig.h"

namespace c2dui {

    class C2DUIConfig {

    public:

        C2DUIConfig(const std::string &homePath, int version);

        virtual ~C2DUIConfig() {};

        virtual void load(C2DUIRomList::Rom *rom = nullptr);

        virtual void save(C2DUIRomList::Rom *rom = nullptr);

        virtual void reset(bool isRom = true);

        virtual std::string *getHomePath();

        virtual std::string *getRomPath(int n = 0);

        virtual std::vector<std::string> *getRomPaths();

        virtual std::vector<C2DUIOption> *get(bool isRom = false);

        virtual C2DUIOption *get(int id, bool isRom = false);

        virtual int getValue(int id, bool isRom = false);

        virtual bool add(int target,
                         const std::string &text, const std::vector<std::string> &values,
                         int defaultValue, int id, bool isRom,
                         unsigned int flags = C2DUIOption::INTEGER);

        bool hide(int id, bool isRom = false);

        virtual std::vector<C2DUIRomList::Hardware> *getHardwareList();

        virtual int *getGuiPlayerInputKeys(int player);

        virtual int *getGuiPlayerInputButtons(int player);

        virtual int *getRomPlayerInputKeys(int player);

        virtual int *getRomPlayerInputButtons(int player);

    private:
        std::vector<C2DUIRomList::Hardware> hardwareList;
        std::vector<C2DUIOption> options_gui;
        std::vector<C2DUIOption> options_rom;
        std::vector<std::string> roms_paths;
        std::string homePath;
        std::string configPath;
        int version;
        int keyboard_keys[KEY_COUNT];
        int joystick_keys[KEY_COUNT];
    };
}

#endif //_C2DUI_CONFIG_H_
