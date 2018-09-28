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

    /// add all hardware support by default ("show all roms")
    getHardwareList()->emplace_back(HARDWARE_PREFIX_ALL, "All");
    std::vector<std::string> hardware_names;
    for (auto &i : *getHardwareList()) {
        hardware_names.emplace_back(i.name);
    }

    /// default options available for all cores

    /////////////////////////////////////////////////
    /// main/gui config
    /////////////////////////////////////////////////
    append("MAIN", {"MAIN"}, 0, Option::Index::MENU_MAIN, Option::Type::MENU);
    append("SHOW_ALL", {"WORKING", "ALL"}, 1, Option::Index::GUI_SHOW_ALL);
    append("SHOW_CLONES", {"OFF", "ON"}, 0, Option::Index::GUI_SHOW_CLONES);
#ifndef __PSP2__ // two slow on vita
    append("SHOW_ICONS", {"OFF", "ON"}, 0, Option::Index::GUI_SHOW_ICONS);
    get()->at(get()->size() - 1).setInfo("Need a restart...");
#endif
    append("USE_DATABASE", {"OFF", "ON"}, 0, Option::Index::GUI_USE_DATABASE);
    get()->at(get()->size() - 1).setInfo("Need a restart...");
    append("FULLSCREEN", {"OFF", "ON"}, 1, Option::Index::GUI_FULLSCREEN, Option::Type::HIDDEN);
#ifdef __SWITCH__
    append("SINGLE_JOYCONS", {"OFF", "ON"}, 0, Option::Index::JOY_SINGLEJOYCON);
#endif

    /////////////////////////////////////////////////
    /// skin config, hidden in gui for now
    /////////////////////////////////////////////////
    append("SKIN", {"SKIN"}, 0, Option::Index::MENU_SKIN, Option::Type::MENU | Option::Type::HIDDEN);
    append("SKIN_FONT_SIZE", {"20"}, 20, Option::Index::SKIN_FONT_SIZE, Option::Type::HIDDEN);

    /////////////////////////////////////////////////
    /// default rom config
    /////////////////////////////////////////////////
    append("EMULATION", {"EMULATION"}, 0, Option::Index::MENU_ROM_OPTIONS, Option::Type::MENU);
    append("SCALING", {"NONE", "2X", "3X", "FIT", "FIT 4:3", "FULL"}, 2, Option::Index::ROM_SCALING);
    append("FILTER", {"POINT", "LINEAR"}, 0, Option::Index::ROM_FILTER);
    if (c2d_renderer->getShaderList() != nullptr) {
        append("EFFECT", c2d_renderer->getShaderList()->getNames(), 0, Option::Index::ROM_SHADER);
    } else {
        append("EFFECT", {"NONE"}, 0, Option::Index::ROM_SHADER, Option::Type::HIDDEN);
    }
    append("SHOW_FPS", {"OFF", "ON"}, 0, Option::Index::ROM_SHOW_FPS);

    /// joysticks config
    append("JOYPAD", {"JOYPAD"}, 0, Option::Index::MENU_JOYPAD, Option::Type::MENU);
    append("JOY_UP", {"-1"}, KEY_JOY_UP_DEFAULT, Option::Index::JOY_UP, Option::Type::INPUT);
    append("JOY_DOWN", {"-1"}, KEY_JOY_DOWN_DEFAULT, Option::Index::JOY_DOWN,
           Option::Type::INPUT);
    append("JOY_LEFT", {"-1"}, KEY_JOY_LEFT_DEFAULT, Option::Index::JOY_LEFT,
           Option::Type::INPUT);
    append("JOY_RIGHT", {"-1"}, KEY_JOY_RIGHT_DEFAULT, Option::Index::JOY_RIGHT,
           Option::Type::INPUT);
    append("JOY_FIRE1", {"0"}, KEY_JOY_FIRE1_DEFAULT, Option::Index::JOY_FIRE1,
           Option::Type::INPUT);
    append("JOY_FIRE2", {"1"}, KEY_JOY_FIRE2_DEFAULT, Option::Index::JOY_FIRE2,
           Option::Type::INPUT);
    append("JOY_FIRE3", {"2"}, KEY_JOY_FIRE3_DEFAULT, Option::Index::JOY_FIRE3,
           Option::Type::INPUT);
    append("JOY_FIRE4", {"3"}, KEY_JOY_FIRE4_DEFAULT, Option::Index::JOY_FIRE4,
           Option::Type::INPUT);
    append("JOY_FIRE5", {"4"}, KEY_JOY_FIRE5_DEFAULT, Option::Index::JOY_FIRE5,
           Option::Type::INPUT);
    append("JOY_FIRE6", {"5"}, KEY_JOY_FIRE6_DEFAULT, Option::Index::JOY_FIRE6,
           Option::Type::INPUT);
    append("JOY_COIN1", {"6"}, KEY_JOY_COIN1_DEFAULT, Option::Index::JOY_COIN1,
           Option::Type::INPUT);
    append("JOY_START1", {"7"}, KEY_JOY_START1_DEFAULT, Option::Index::JOY_START1,
           Option::Type::INPUT);
    // TODO: add gui option for axis in option menu
    append("JOY_AXIS_LX", {"0"}, KEY_JOY_AXIS_LX, Option::Index::JOY_AXIS_LX,
           Option::Type::HIDDEN);
    append("JOY_AXIS_LY", {"1"}, KEY_JOY_AXIS_LY, Option::Index::JOY_AXIS_LY,
           Option::Type::HIDDEN);
    append("JOY_AXIS_RX", {"2"}, KEY_JOY_AXIS_RX, Option::Index::JOY_AXIS_RX,
           Option::Type::HIDDEN);
    append("JOY_AXIS_RY", {"3"}, KEY_JOY_AXIS_RY, Option::Index::JOY_AXIS_RY,
           Option::Type::HIDDEN);
    append("JOY_DEADZONE",
           {"2000", "4000", "6000", "8000", "10000", "12000", "14000", "16000",
            "18000", "20000", "22000", "24000", "26000", "28000", "30000"}, 3,
           Option::Index::JOY_DEADZONE, Option::Type::INTEGER);
#ifndef NO_KEYBOARD
    // keyboard
    append("KEYBOARD", {"KEYBOARD"}, 0, Option::Index::MENU_KEYBOARD, Option::Type::MENU);
    append("KEY_UP", {std::to_string(KEY_KB_UP_DEFAULT)}, KEY_KB_UP_DEFAULT,
           Option::Index::KEY_UP, Option::Type::INPUT);        // KP_UP
    append("KEY_DOWN", {std::to_string(KEY_KB_DOWN_DEFAULT)}, KEY_KB_DOWN_DEFAULT,
           Option::Index::KEY_DOWN, Option::Type::INPUT);    // KP_DOWN
    append("KEY_LEFT", {std::to_string(KEY_KB_LEFT_DEFAULT)}, KEY_KB_LEFT_DEFAULT,
           Option::Index::KEY_LEFT, Option::Type::INPUT);    // KP_LEFT
    append("KEY_RIGHT", {std::to_string(KEY_KB_RIGHT_DEFAULT)}, KEY_KB_RIGHT_DEFAULT,
           Option::Index::KEY_RIGHT, Option::Type::INPUT);  // KP_RIGHT
    append("KEY_FIRE1", {std::to_string(KEY_KB_FIRE1_DEFAULT)}, KEY_KB_FIRE1_DEFAULT,
           Option::Index::KEY_FIRE1, Option::Type::INPUT);  // KP_1
    append("KEY_FIRE2", {std::to_string(KEY_KB_FIRE2_DEFAULT)}, KEY_KB_FIRE2_DEFAULT,
           Option::Index::KEY_FIRE2, Option::Type::INPUT);  // KP_2
    append("KEY_FIRE3", {std::to_string(KEY_KB_FIRE3_DEFAULT)}, KEY_KB_FIRE3_DEFAULT,
           Option::Index::KEY_FIRE3, Option::Type::INPUT);  // KP_3
    append("KEY_FIRE4", {std::to_string(KEY_KB_FIRE4_DEFAULT)}, KEY_KB_FIRE4_DEFAULT,
           Option::Index::KEY_FIRE4, Option::Type::INPUT);  // KP_4
    append("KEY_FIRE5", {std::to_string(KEY_KB_FIRE5_DEFAULT)}, KEY_KB_FIRE5_DEFAULT,
           Option::Index::KEY_FIRE5, Option::Type::INPUT);  // KP_5
    append("KEY_FIRE6", {std::to_string(KEY_KB_FIRE6_DEFAULT)}, KEY_KB_FIRE6_DEFAULT,
           Option::Index::KEY_FIRE6, Option::Type::INPUT);  // KP_6
    append("KEY_COIN1", {std::to_string(KEY_KB_COIN1_DEFAULT)}, KEY_KB_COIN1_DEFAULT,
           Option::Index::KEY_COIN1, Option::Type::INPUT);  // ESCAPE
    append("KEY_START1", {std::to_string(KEY_KB_START1_DEFAULT)}, KEY_KB_START1_DEFAULT,
           Option::Index::KEY_START1, Option::Type::INPUT);// ENTER
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
                if (option.flags & Option::Type::MENU) {
                    settings = config_setting_lookup(settings_root, option.getName());
                }
                if (settings) {
                    int value = 0;
                    if (config_setting_lookup_int(settings, option.getName(), &value)) {
                        option.value = value;
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
        if (option.flags & Option::Type::MENU) {
            sub_setting = config_setting_add(setting_fba, option.getName(), CONFIG_TYPE_GROUP);
            continue;
        }
        config_setting_t *setting = config_setting_add(sub_setting, option.getName(), CONFIG_TYPE_INT);
        config_setting_set_int(setting, option.value);
    }

    config_write_file(&cfg, path.c_str());
    config_destroy(&cfg);
}

void Config::reset(bool isRom) {

    options_rom.clear();

    int start = 0, end = (int) options_gui.size();
    for (unsigned int i = 0; i < options_gui.size(); i++) {
        if (options_gui[i].id == Option::Index::MENU_ROM_OPTIONS) {
            start = i;
            break;
        }
    }

    for (int i = start; i < end; i++) {
        options_rom.emplace_back(options_gui[i]);
    }
}

std::string *Config::getHomePath() {
    return &homePath;
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
        if (option.id == index) {
            return &option;
        }
    }
    return nullptr;
}

bool Config::add(int target,
                 const std::string &text, const std::vector<std::string> &values,
                 int defaultValue, int index, unsigned int flags) {

    for (auto &option : options_gui) {
        if (option.id == target) {
            options_gui.insert(options_gui.begin() + target,
                               Option(text, values, defaultValue, index, flags));
            return true;
        }
    }

    return false;
}

void Config::append(const std::string &text, const std::vector<std::string> &values,
                    int defaultValue, int index, unsigned int flags) {

    options_gui.emplace_back(text, values, defaultValue, index, flags);
}

int Config::getValue(int index, bool isRom) {

    std::vector<Option> *options = get(isRom);

    for (auto &option : *options) {
        if (option.id == index) {
            return option.value;
        }
    }

    return 0;
}

bool Config::hide(int index, bool isRom) {

    std::vector<Option> *options = get(isRom);

    for (auto &option : *options) {
        if (option.id == index) {
            option.flags |= Option::HIDDEN;
            return true;
        }
    }

    return false;
}

int *Config::getGuiPlayerInputKeys(int player) {

#ifndef NO_KEYBOARD
    // TODO: player > 0 not supported yet
    keyboard_keys[0] = getValue(Option::Index::KEY_UP);
    keyboard_keys[1] = getValue(Option::Index::KEY_DOWN);
    keyboard_keys[2] = getValue(Option::Index::KEY_LEFT);
    keyboard_keys[3] = getValue(Option::Index::KEY_RIGHT);
    keyboard_keys[4] = getValue(Option::Index::KEY_COIN1);
    keyboard_keys[5] = getValue(Option::Index::KEY_START1);
    keyboard_keys[6] = getValue(Option::Index::KEY_FIRE1);
    keyboard_keys[7] = getValue(Option::Index::KEY_FIRE2);
    keyboard_keys[8] = getValue(Option::Index::KEY_FIRE3);
    keyboard_keys[9] = getValue(Option::Index::KEY_FIRE4);
    keyboard_keys[10] = getValue(Option::Index::KEY_FIRE5);
    keyboard_keys[11] = getValue(Option::Index::KEY_FIRE6);
#endif

    return keyboard_keys;
}

int *Config::getGuiPlayerInputButtons(int player) {

    // TODO: player > 0 not supported yet
    joystick_keys[0] = getValue(Option::Index::JOY_UP);
    joystick_keys[1] = getValue(Option::Index::JOY_DOWN);
    joystick_keys[2] = getValue(Option::Index::JOY_LEFT);
    joystick_keys[3] = getValue(Option::Index::JOY_RIGHT);
    joystick_keys[4] = getValue(Option::Index::JOY_COIN1);
    joystick_keys[5] = getValue(Option::Index::JOY_START1);
    joystick_keys[6] = getValue(Option::Index::JOY_FIRE1);
    joystick_keys[7] = getValue(Option::Index::JOY_FIRE2);
    joystick_keys[8] = getValue(Option::Index::JOY_FIRE3);
    joystick_keys[9] = getValue(Option::Index::JOY_FIRE4);
    joystick_keys[10] = getValue(Option::Index::JOY_FIRE5);
    joystick_keys[11] = getValue(Option::Index::JOY_FIRE6);

    return joystick_keys;
}

int *Config::getRomPlayerInputKeys(int player) {

#ifndef NO_KEYBOARD
    // TODO: player > 0 not supported yet
    keyboard_keys[0] = getValue(Option::Index::KEY_UP, true);
    keyboard_keys[1] = getValue(Option::Index::KEY_DOWN, true);
    keyboard_keys[2] = getValue(Option::Index::KEY_LEFT, true);
    keyboard_keys[3] = getValue(Option::Index::KEY_RIGHT, true);
    keyboard_keys[4] = getValue(Option::Index::KEY_COIN1, true);
    keyboard_keys[5] = getValue(Option::Index::KEY_START1, true);
    keyboard_keys[6] = getValue(Option::Index::KEY_FIRE1, true);
    keyboard_keys[7] = getValue(Option::Index::KEY_FIRE2, true);
    keyboard_keys[8] = getValue(Option::Index::KEY_FIRE3, true);
    keyboard_keys[9] = getValue(Option::Index::KEY_FIRE4, true);
    keyboard_keys[10] = getValue(Option::Index::KEY_FIRE5, true);
    keyboard_keys[11] = getValue(Option::Index::KEY_FIRE6, true);
#endif

    return keyboard_keys;
}

int *Config::getRomPlayerInputButtons(int player) {

    // TODO: player > 0 not supported yet
    joystick_keys[0] = getValue(Option::Index::JOY_UP, true);
    joystick_keys[1] = getValue(Option::Index::JOY_DOWN, true);
    joystick_keys[2] = getValue(Option::Index::JOY_LEFT, true);
    joystick_keys[3] = getValue(Option::Index::JOY_RIGHT, true);
    joystick_keys[4] = getValue(Option::Index::JOY_COIN1, true);
    joystick_keys[5] = getValue(Option::Index::JOY_START1, true);
    joystick_keys[6] = getValue(Option::Index::JOY_FIRE1, true);
    joystick_keys[7] = getValue(Option::Index::JOY_FIRE2, true);
    joystick_keys[8] = getValue(Option::Index::JOY_FIRE3, true);
    joystick_keys[9] = getValue(Option::Index::JOY_FIRE4, true);
    joystick_keys[10] = getValue(Option::Index::JOY_FIRE5, true);
    joystick_keys[11] = getValue(Option::Index::JOY_FIRE6, true);

    return joystick_keys;
}

std::vector<RomList::Hardware> *Config::getHardwareList() {

    return &hardwareList;
}
