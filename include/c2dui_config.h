//
// Created by cpasjuste on 05/12/16.
//

#ifndef _C2DUI_CONFIG_H_
#define _C2DUI_CONFIG_H_

#include "libconfig.h"

namespace c2dui {

    class Config {

    public:

        Config(c2d::Io *io, int version);

        virtual ~Config() = default;

        virtual void load(RomList::Rom *rom = nullptr);

        virtual void save(RomList::Rom *rom = nullptr);

        virtual void reset(bool isRom = true);

        virtual std::string getHomePath();

        virtual std::string getTitlesPath();

        virtual std::string getPreviewsPath();

        virtual std::string getMixesPath();

        virtual std::string *getRomPath(int n = 0);

        virtual std::vector<std::string> *getRomPaths();

        virtual std::vector<Option> *get(bool isRom = false);

        virtual Option *get(int id, bool isRom = false);

        virtual bool add(int target,
                         const std::string &text, const std::vector<std::string> &values,
                         int defaultValue, int id, unsigned int flags);

        virtual void append(const std::string &text, const std::vector<std::string> &values,
                            int defaultValue, int id, unsigned int flags);

        virtual void append(const std::string &text, int value, int id, unsigned int flags);

        bool hide(int id, bool isRom = false);

        virtual int *getPlayerInputKeys(int player, bool isRom = false);

        virtual int *getPlayerInputButtons(int player, bool isRom = false);

        virtual std::vector<RomList::Hardware> *getHardwareList();

    private:
        std::vector<RomList::Hardware> hardwareList;
        std::vector<Option> options_gui;
        std::vector<Option> options_rom;
        std::vector<std::string> roms_paths;
        std::string configPath;
        std::string dataPath;

        int version;
        int keyboard_keys[KEY_COUNT];
        int joystick_keys[KEY_COUNT];
    };
}

#endif //_C2DUI_CONFIG_H_
