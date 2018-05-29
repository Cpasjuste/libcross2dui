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

        virtual int getValue(int id, bool rom = false);

        virtual std::string *getHomePath();

        virtual const char *getRomPath(int n);

        virtual std::vector<std::string> *getRomPaths();

        virtual std::vector<C2DUIOption> *getOptions(bool rom = false);

        virtual C2DUIOption *getOption(std::vector<C2DUIOption> *options, int index);

        virtual int *getGuiPlayerInputKeys(int player);

        virtual int *getGuiPlayerInputButtons(int player);

        virtual int *getRomPlayerInputKeys(int player);

        virtual int *getRomPlayerInputButtons(int player);

        virtual std::vector<C2DUIRomList::Hardware> *getHardwareList();

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
