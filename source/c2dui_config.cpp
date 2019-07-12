//
// Created by cpasjuste on 05/12/16.
//

#include "c2dui.h"
#include "c2dui_config.h"

using namespace c2d;
using namespace c2dui;

Config::Config(const std::string &home, int ver) {

    printf("Config(%s, v%i)\n", home.c_str(), ver);

    homePath = home;
    configPath = homePath + "config.cfg";
    version = ver;

    /// add default roms paths
    getRomPaths()->clear();
    getRomPaths()->emplace_back(home + "roms/");
    // default hardware filter (all)
    getHardwareList()->emplace_back(HARDWARE_PREFIX_ALL, "All");

    /// default options available for all cores
    /////////////////////////////////////////////////
    /// main/gui config
    /////////////////////////////////////////////////
    append("UI_OPTIONS", {}, 0, 1000, Option::Flags::DELIMITER);
    append("MAIN", {"MAIN"}, 0, Option::Id::MENU_MAIN, Option::Flags::MENU);
    append("SHOW", {"ALL", "FAVORITES"}, 0, Option::Id::GUI_SHOW_ALL);
    append("SHOW_CLONES", {"OFF", "ON"}, 0, Option::Id::GUI_SHOW_CLONES);
#if !(defined(__PSP2__) || defined(__3DS__)) // two slow
    append("SHOW_ICONS", {"OFF", "ON"}, 0, Option::Id::GUI_SHOW_ICONS);
    get()->at(get()->size() - 1).setInfo("Enabling icons needs a restart...");
#endif
    append("USE_DATABASE", {"OFF", "ON"}, 0, Option::Id::GUI_USE_DATABASE);
    get()->at(get()->size() - 1).setInfo("Using a DB needs a restart...");
    append("FULLSCREEN", {"OFF", "ON"}, 1, Option::Id::GUI_FULLSCREEN, Option::Flags::HIDDEN);
    // build zipped skin list
    std::vector<std::string> paths;
    std::vector<Io::File> files = c2d_renderer->getIo()->getDirList(homePath + "skins/", true);
    for (const auto &file : files) {
        if (file.name[0] == '.') {
            continue;
        }
        paths.emplace_back(Utility::removeExt(file.name));
    }
    append("SKIN", paths, 0, Option::Id::GUI_SKIN);
    get()->at(get()->size() - 1).setInfo("Changing skin needs a restart...");
#ifdef __SWITCH__
    append("SINGLE_JOYCONS", {"OFF", "ON"}, 0, Option::Id::JOY_SINGLEJOYCON);
#endif

    /////////////////////////////////////////////////
    /// default rom config
    /////////////////////////////////////////////////
    append("DEFAULT_ROMS_OPTIONS", {}, 0, 1001, Option::Flags::DELIMITER);
    append("EMULATION", {"EMULATION"}, 0, Option::Id::MENU_ROM_OPTIONS, Option::Flags::MENU);
#ifdef __FREEPLAY__
    append("SCALING", {"NONE", "2X", "3X", "FIT", "FIT 4:3", "FULL"}, 5, Option::Id::ROM_SCALING);
    append("FILTER", {"POINT", "LINEAR"}, 1, Option::Id::ROM_FILTER);
#else
    append("SCALING", {"NONE", "2X", "3X", "FIT", "FIT 4:3", "FULL"}, 2, Option::Id::ROM_SCALING);
    append("FILTER", {"POINT", "LINEAR"}, 0, Option::Id::ROM_FILTER);
#endif
    if (c2d_renderer->getShaderList() != nullptr) {
        append("EFFECT", c2d_renderer->getShaderList()->getNames(), 0, Option::Id::ROM_SHADER);
    } else {
        append("EFFECT", {"NONE"}, 0, Option::Id::ROM_SHADER, Option::Flags::HIDDEN);
    }
    append("SHOW_FPS", {"OFF", "ON"}, 0, Option::Id::ROM_SHOW_FPS);

    /// joysticks config
    append("JOYPAD", {"JOYPAD"}, 0, Option::Id::MENU_JOYPAD, Option::Flags::MENU);
    append("JOY_UP", KEY_JOY_UP_DEFAULT, Option::Id::JOY_UP, Option::Flags::INPUT);
    append("JOY_DOWN", KEY_JOY_DOWN_DEFAULT, Option::Id::JOY_DOWN, Option::Flags::INPUT);
    append("JOY_LEFT", KEY_JOY_LEFT_DEFAULT, Option::Id::JOY_LEFT, Option::Flags::INPUT);
    append("JOY_RIGHT", KEY_JOY_RIGHT_DEFAULT, Option::Id::JOY_RIGHT, Option::Flags::INPUT);
    append("JOY_FIRE1", KEY_JOY_FIRE1_DEFAULT, Option::Id::JOY_FIRE1, Option::Flags::INPUT);
    append("JOY_FIRE2", KEY_JOY_FIRE2_DEFAULT, Option::Id::JOY_FIRE2, Option::Flags::INPUT);
    append("JOY_FIRE3", KEY_JOY_FIRE3_DEFAULT, Option::Id::JOY_FIRE3, Option::Flags::INPUT);
    append("JOY_FIRE4", KEY_JOY_FIRE4_DEFAULT, Option::Id::JOY_FIRE4, Option::Flags::INPUT);
    append("JOY_FIRE5", KEY_JOY_FIRE5_DEFAULT, Option::Id::JOY_FIRE5, Option::Flags::INPUT);
    append("JOY_FIRE6", KEY_JOY_FIRE6_DEFAULT, Option::Id::JOY_FIRE6, Option::Flags::INPUT);
    append("JOY_COIN1", KEY_JOY_COIN1_DEFAULT, Option::Id::JOY_COIN1, Option::Flags::INPUT);
    append("JOY_START1", KEY_JOY_START1_DEFAULT, Option::Id::JOY_START1, Option::Flags::INPUT);
    // TODO: add gui option for axis in option menu
    append("JOY_AXIS_LX", KEY_JOY_AXIS_LX, Option::Id::JOY_AXIS_LX, Option::Flags::HIDDEN);
    append("JOY_AXIS_LY", KEY_JOY_AXIS_LY, Option::Id::JOY_AXIS_LY, Option::Flags::HIDDEN);
    append("JOY_AXIS_RX", KEY_JOY_AXIS_RX, Option::Id::JOY_AXIS_RX, Option::Flags::HIDDEN);
    append("JOY_AXIS_RY", KEY_JOY_AXIS_RY, Option::Id::JOY_AXIS_RY, Option::Flags::HIDDEN);
    append("JOY_DEADZONE",
           {"2000", "4000", "6000", "8000", "10000", "12000", "14000", "16000",
            "18000", "20000", "22000", "24000", "26000", "28000", "30000"}, 3, Option::Id::JOY_DEADZONE);
#ifndef NO_KEYBOARD
    // keyboard
    append("KEYBOARD", {"KEYBOARD"}, 0, Option::Id::MENU_KEYBOARD, Option::Flags::MENU);
    append("KEY_UP", KEY_KB_UP_DEFAULT, Option::Id::KEY_UP, Option::Flags::INPUT);        // KP_UP
    append("KEY_DOWN", KEY_KB_DOWN_DEFAULT, Option::Id::KEY_DOWN, Option::Flags::INPUT);    // KP_DOWN
    append("KEY_LEFT", KEY_KB_LEFT_DEFAULT, Option::Id::KEY_LEFT, Option::Flags::INPUT);    // KP_LEFT
    append("KEY_RIGHT", KEY_KB_RIGHT_DEFAULT, Option::Id::KEY_RIGHT, Option::Flags::INPUT);  // KP_RIGHT
    append("KEY_FIRE1", KEY_KB_FIRE1_DEFAULT, Option::Id::KEY_FIRE1, Option::Flags::INPUT);  // KP_1
    append("KEY_FIRE2", KEY_KB_FIRE2_DEFAULT, Option::Id::KEY_FIRE2, Option::Flags::INPUT);  // KP_2
    append("KEY_FIRE3", KEY_KB_FIRE3_DEFAULT, Option::Id::KEY_FIRE3, Option::Flags::INPUT);  // KP_3
    append("KEY_FIRE4", KEY_KB_FIRE4_DEFAULT, Option::Id::KEY_FIRE4, Option::Flags::INPUT);  // KP_4
    append("KEY_FIRE5", KEY_KB_FIRE5_DEFAULT, Option::Id::KEY_FIRE5, Option::Flags::INPUT);  // KP_5
    append("KEY_FIRE6", KEY_KB_FIRE6_DEFAULT, Option::Id::KEY_FIRE6, Option::Flags::INPUT);  // KP_6
    append("KEY_COIN1", KEY_KB_COIN1_DEFAULT, Option::Id::KEY_COIN1, Option::Flags::INPUT);  // ESCAPE
    append("KEY_START1", KEY_KB_START1_DEFAULT, Option::Id::KEY_START1, Option::Flags::INPUT);// ENTER
#endif

}

void Config::load(RomList::Rom *rom) {

    config_t cfg;
    config_init(&cfg);

    bool isRomCfg = rom != nullptr;
    std::vector<Option> *options = isRomCfg ? &options_rom : &options_gui;
    std::string path = configPath;
    if (isRomCfg) {
        path = homePath;
        path += "configs/";
        path += rom->drv_name;
        path += ".cfg";
    }

    printf("Config::load: %s ...", path.c_str());

    if (config_read_file(&cfg, path.c_str())) {

        config_setting_t *settings_root = config_lookup(&cfg, "CONFIG");
        if (settings_root) {
            printf("OK\n");
#ifdef __UPDATE_CONFIG__
            // verify cfg version
            int cfg_version = 0;
            if (!config_setting_lookup_int(settings_root, "VERSION", &cfg_version)
                || cfg_version != version) {
                // update cfg to newer version
                printf("CONFIG VERSION (%i) != EXE VERSION (%i)\n", cfg_version, version);
                save(rom);
                config_destroy(&cfg);
                return;
            }
#endif
            config_setting_t *settings = nullptr;
            if (!isRomCfg) {
                settings = config_setting_lookup(settings_root, "ROMS_PATHS");
                if (settings) {
                    for (unsigned int i = 0; i < roms_paths.size(); i++) {
                        char p[MAX_PATH];
                        snprintf(p, MAX_PATH, "ROMS_PATH%i", i);
                        const char *value;
                        if (config_setting_lookup_string(settings, p, &value)) {
                            roms_paths[i] = value;
                            if (!roms_paths[i].empty() && roms_paths[i].back() != '/')
                                roms_paths[i] += '/';
                            printf("%s: %s\n", p, value);
                        }
                    }
                }
            }

            for (auto &option : *options) {
                if (option.getFlags() & Option::Flags::DELIMITER) {
                    continue;
                }
                if (option.getFlags() & Option::Flags::MENU) {
                    settings = config_setting_lookup(settings_root, option.getName().c_str());
                }
                if (settings) {
                    int value = 0;
                    if (config_setting_lookup_int(settings, option.getName().c_str(), &value)) {
                        if (option.getFlags() & Option::Flags::INPUT) {
                            option.setValueInt(value);
                        } else {
                            option.setIndex(value);
                        }
                        //printf("[%i] => %s: %i\n", option.index, option.getName(), value);
                    }
                }
            }
        } else {
            printf("NOK: root configuration not found\n");
            save(rom);
            config_destroy(&cfg);
            return;
        }
    } else {
        printf("NOK: file not found\n");
        // reset default rom options for other roms usage
        reset();
        // no need to save default rom config
        if (!isRomCfg) {
            save();
        }
    }

    config_destroy(&cfg);
}

void Config::save(RomList::Rom *rom) {

    config_t cfg{};
    config_init(&cfg);

    bool isRomCfg = rom != nullptr;
    std::vector<Option> *options = isRomCfg ? &options_rom : &options_gui;
    std::string path = configPath;
    if (isRomCfg) {
        path = homePath;
        path += "configs/";
        path += rom->drv_name;
        path += ".cfg";
    }

    printf("Config::save: %s\n", path.c_str());

    // create root
    config_setting_t *setting_root = config_root_setting(&cfg);

    // create main group
    config_setting_t *setting_fba = config_setting_add(setting_root, "CONFIG", CONFIG_TYPE_GROUP);
    // add version
    config_setting_t *setting_cfg = config_setting_add(setting_fba, "VERSION", CONFIG_TYPE_INT);
    config_setting_set_int(setting_cfg, version);

    config_setting_t *sub_setting = nullptr;

    if (!isRomCfg) {
        sub_setting = config_setting_add(setting_fba, "ROMS_PATHS", CONFIG_TYPE_GROUP);
        for (unsigned int i = 0; i < roms_paths.size(); i++) {
            char p[MAX_PATH];
            snprintf(p, MAX_PATH, "ROMS_PATH%i", i);
            config_setting_t *setting = config_setting_add(sub_setting, p, CONFIG_TYPE_STRING);
            config_setting_set_string(setting, roms_paths[i].c_str());
        }
    }

    for (auto &option : *options) {
        if (option.getFlags() & Option::Flags::DELIMITER) {
            continue;
        }
        if (option.getFlags() & Option::Flags::MENU) {
            sub_setting = config_setting_add(setting_fba, option.getName().c_str(), CONFIG_TYPE_GROUP);
            continue;
        }
        config_setting_t *setting = config_setting_add(sub_setting, option.getName().c_str(), CONFIG_TYPE_INT);
        if (option.getFlags() & Option::Flags::INPUT) {
            config_setting_set_int(setting, option.getValueInt());
        } else {
            config_setting_set_int(setting, option.getIndex());
        }
    }

    config_write_file(&cfg, path.c_str());
    config_destroy(&cfg);
}

void Config::reset(bool isRom) {

    options_rom.clear();

    int start = 0, end = (int) options_gui.size();
    for (unsigned int i = 0; i < options_gui.size(); i++) {
        if (options_gui[i].getId() == Option::Id::MENU_ROM_OPTIONS) {
            start = i;
            break;
        }
    }

    for (int i = start; i < end; i++) {
        options_rom.emplace_back(options_gui[i]);
    }
}

std::string Config::getHomePath() {
    return homePath;
}

std::string Config::getTitlesPath() {
    return homePath + "titles/";
}

std::string Config::getPreviewsPath() {
    return homePath + "previews/";
}

std::string Config::getMixesPath() {
    return homePath + "mixes/";
}

std::string *Config::getRomPath(int n) {
    if ((size_t) n >= getRomPaths()->size()) {
        return &roms_paths[0];
    } else {
        return &roms_paths[n];
    }
}

std::vector<std::string> *Config::getRomPaths() {
    return &roms_paths;
}

std::vector<Option> *Config::get(bool isRom) {
    return isRom ? &options_rom : &options_gui;
}

Option *Config::get(int index, bool isRom) {

    std::vector<Option> *options = get(isRom);

    for (auto &option : *options) {
        if (option.getId() == index) {
            return &option;
        }
    }
    return nullptr;
}

bool Config::add(int target,
                 const std::string &text, const std::vector<std::string> &values,
                 int defaultValue, int index, unsigned int flags) {

    for (unsigned int i = 0; i < options_gui.size(); i++) {
        if (options_gui[i].getId() == target) {
            options_gui.insert(options_gui.begin() + i + 1,
                               Option(text, values, defaultValue, index, flags));
            return true;
        }
    }

    return false;
}

void Config::append(const std::string &text, const std::vector<std::string> &values,
                    int defaultValue, int id, unsigned int flags) {
    options_gui.emplace_back(text, values, defaultValue, id, flags);
}

void Config::append(const std::string &text, int value, int id, unsigned int flags) {
    append(text, {std::to_string(value)}, 0, id, flags);
}

bool Config::hide(int index, bool isRom) {

    std::vector<Option> *options = get(isRom);

    for (auto &option : *options) {
        if (option.getId() == index) {
            unsigned int flags = option.getFlags();
            flags |= Option::HIDDEN;
            option.setFlags(flags);
            return true;
        }
    }

    return false;
}

int *Config::getPlayerInputKeys(int player, bool isRom) {

#ifndef NO_KEYBOARD
    // TODO: player > 0 not supported yet
    keyboard_keys[0] = get(Option::Id::KEY_UP, isRom)->getValueInt();
    keyboard_keys[1] = get(Option::Id::KEY_DOWN, isRom)->getValueInt();
    keyboard_keys[2] = get(Option::Id::KEY_LEFT, isRom)->getValueInt();
    keyboard_keys[3] = get(Option::Id::KEY_RIGHT, isRom)->getValueInt();
    keyboard_keys[4] = get(Option::Id::KEY_COIN1, isRom)->getValueInt();
    keyboard_keys[5] = get(Option::Id::KEY_START1, isRom)->getValueInt();
    keyboard_keys[6] = get(Option::Id::KEY_FIRE1, isRom)->getValueInt();
    keyboard_keys[7] = get(Option::Id::KEY_FIRE2, isRom)->getValueInt();
    keyboard_keys[8] = get(Option::Id::KEY_FIRE3, isRom)->getValueInt();
    keyboard_keys[9] = get(Option::Id::KEY_FIRE4, isRom)->getValueInt();
    keyboard_keys[10] = get(Option::Id::KEY_FIRE5, isRom)->getValueInt();
    keyboard_keys[11] = get(Option::Id::KEY_FIRE6, isRom)->getValueInt();
#endif

    return keyboard_keys;
}

int *Config::getPlayerInputButtons(int player, bool isRom) {

    // TODO: player > 0 not supported yet
    joystick_keys[0] = get(Option::Id::JOY_UP, isRom)->getValueInt();
    joystick_keys[1] = get(Option::Id::JOY_DOWN, isRom)->getValueInt();
    joystick_keys[2] = get(Option::Id::JOY_LEFT, isRom)->getValueInt();
    joystick_keys[3] = get(Option::Id::JOY_RIGHT, isRom)->getValueInt();
    joystick_keys[4] = get(Option::Id::JOY_COIN1, isRom)->getValueInt();
    joystick_keys[5] = get(Option::Id::JOY_START1, isRom)->getValueInt();
    joystick_keys[6] = get(Option::Id::JOY_FIRE1, isRom)->getValueInt();
    joystick_keys[7] = get(Option::Id::JOY_FIRE2, isRom)->getValueInt();
    joystick_keys[8] = get(Option::Id::JOY_FIRE3, isRom)->getValueInt();
    joystick_keys[9] = get(Option::Id::JOY_FIRE4, isRom)->getValueInt();
    joystick_keys[10] = get(Option::Id::JOY_FIRE5, isRom)->getValueInt();
    joystick_keys[11] = get(Option::Id::JOY_FIRE6, isRom)->getValueInt();

    return joystick_keys;
}

std::vector<RomList::Hardware> *Config::getHardwareList() {
    return &hardwareList;
}
