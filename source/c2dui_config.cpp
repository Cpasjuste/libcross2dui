//
// Created by cpasjuste on 05/12/16.
//

#include "c2dui.h"

using namespace c2d;
using namespace c2dui;

C2DUIConfig::C2DUIConfig(const std::string &home, int ver) {

    homePath = home;
    configPath = homePath + "config.cfg";
    version = ver;

    // add default roms paths
    getRomPaths()->clear();
    getRomPaths()->emplace_back(home + "roms/");

    // add all hardware support by default ("show all roms")
    getHardwareList()->emplace_back(HARDWARE_PREFIX_ALL, "All");
    std::vector<std::string> hardware_names;
    for (auto &i : *getHardwareList()) {
        hardware_names.emplace_back(i.name);
    }

    std::vector<C2DUIOption> *ui_options = get();

    // main/gui config
    ui_options->emplace_back(
            C2DUIOption("MAIN", {"MAIN"}, 0, C2DUIOption::Index::MENU_MAIN, C2DUIOption::Type::MENU));
    ui_options->emplace_back(C2DUIOption("SHOW_ALL", {"WORKING", "ALL"}, 1, C2DUIOption::Index::GUI_SHOW_ALL));
    ui_options->emplace_back(C2DUIOption("SHOW_CLONES", {"OFF", "ON"}, 0, C2DUIOption::Index::GUI_SHOW_CLONES));
    ui_options->emplace_back(C2DUIOption("SHOW_ICONS", {"OFF", "ON"}, 0, C2DUIOption::Index::GUI_SHOW_ICONS));
    ui_options->at(ui_options->size() - 1).setInfo("Need a restart...");
    ui_options->emplace_back(C2DUIOption("USE_DATABASE", {"OFF", "ON"}, 0, C2DUIOption::Index::GUI_USE_DATABASE));
    ui_options->at(ui_options->size() - 1).setInfo("Need a restart...");
    ui_options->emplace_back(
            C2DUIOption("FULLSCREEN", {"OFF", "ON"}, 1, C2DUIOption::Index::GUI_FULLSCREEN, C2DUIOption::Type::HIDDEN));
#ifdef __SWITCH__
    ui_options->emplace_back(C2DUIOption("SINGLE_JOYCONS", {"OFF", "ON"}, 0, C2DUIOption::Index::JOY_SINGLEJOYCON));
#endif

    // skin config, hidden in gui for now
    ui_options->emplace_back(
            C2DUIOption("SKIN", {"SKIN"}, 0, C2DUIOption::Index::MENU_SKIN,
                        C2DUIOption::Type::MENU | C2DUIOption::Type::HIDDEN));
    ui_options->emplace_back(
            C2DUIOption("SKIN_FONT_SIZE", {"20"}, 20, C2DUIOption::Index::SKIN_FONT_SIZE, C2DUIOption::Type::HIDDEN));
    // default rom config
    ui_options->emplace_back(
            C2DUIOption("EMULATION", {"EMULATION"}, 0, C2DUIOption::Index::MENU_ROM_OPTIONS, C2DUIOption::Type::MENU));
    ui_options->emplace_back(
            C2DUIOption("SCALING", {"NONE", "2X", "3X", "FIT", "FIT 4:3", "FULL"}, 2, C2DUIOption::Index::ROM_SCALING));
    ui_options->emplace_back(
            C2DUIOption("FILTER", {"POINT", "LINEAR"}, 0, C2DUIOption::Index::ROM_FILTER));
    if (c2d_renderer->getShaderList() != nullptr) {
        ui_options->emplace_back(
                C2DUIOption("EFFECT", c2d_renderer->getShaderList()->getNames(), 0, C2DUIOption::Index::ROM_SHADER));
    } else {
        ui_options->emplace_back(
                C2DUIOption("EFFECT", {"NA"}, 0, C2DUIOption::Index::ROM_SHADER, C2DUIOption::Type::HIDDEN));
    }
    ui_options->emplace_back(
            C2DUIOption("SHOW_FPS", {"OFF", "ON"}, 0, C2DUIOption::Index::ROM_SHOW_FPS));

    // joystick
    ui_options->emplace_back(
            C2DUIOption("JOYPAD", {"JOYPAD"}, 0, C2DUIOption::Index::MENU_JOYPAD, C2DUIOption::Type::MENU));
    ui_options->emplace_back(
            C2DUIOption("JOY_UP", {"-1"}, KEY_JOY_UP_DEFAULT, C2DUIOption::Index::JOY_UP, C2DUIOption::Type::INPUT));
    ui_options->emplace_back(
            C2DUIOption("JOY_DOWN", {"-1"}, KEY_JOY_DOWN_DEFAULT, C2DUIOption::Index::JOY_DOWN,
                        C2DUIOption::Type::INPUT));
    ui_options->emplace_back(
            C2DUIOption("JOY_LEFT", {"-1"}, KEY_JOY_LEFT_DEFAULT, C2DUIOption::Index::JOY_LEFT,
                        C2DUIOption::Type::INPUT));
    ui_options->emplace_back(
            C2DUIOption("JOY_RIGHT", {"-1"}, KEY_JOY_RIGHT_DEFAULT, C2DUIOption::Index::JOY_RIGHT,
                        C2DUIOption::Type::INPUT));
    ui_options->emplace_back(
            C2DUIOption("JOY_FIRE1", {"0"}, KEY_JOY_FIRE1_DEFAULT, C2DUIOption::Index::JOY_FIRE1,
                        C2DUIOption::Type::INPUT));
    ui_options->emplace_back(
            C2DUIOption("JOY_FIRE2", {"1"}, KEY_JOY_FIRE2_DEFAULT, C2DUIOption::Index::JOY_FIRE2,
                        C2DUIOption::Type::INPUT));
    ui_options->emplace_back(
            C2DUIOption("JOY_FIRE3", {"2"}, KEY_JOY_FIRE3_DEFAULT, C2DUIOption::Index::JOY_FIRE3,
                        C2DUIOption::Type::INPUT));
    ui_options->emplace_back(
            C2DUIOption("JOY_FIRE4", {"3"}, KEY_JOY_FIRE4_DEFAULT, C2DUIOption::Index::JOY_FIRE4,
                        C2DUIOption::Type::INPUT));
    ui_options->emplace_back(
            C2DUIOption("JOY_FIRE5", {"4"}, KEY_JOY_FIRE5_DEFAULT, C2DUIOption::Index::JOY_FIRE5,
                        C2DUIOption::Type::INPUT));
    ui_options->emplace_back(
            C2DUIOption("JOY_FIRE6", {"5"}, KEY_JOY_FIRE6_DEFAULT, C2DUIOption::Index::JOY_FIRE6,
                        C2DUIOption::Type::INPUT));
    ui_options->emplace_back(
            C2DUIOption("JOY_COIN1", {"6"}, KEY_JOY_COIN1_DEFAULT, C2DUIOption::Index::JOY_COIN1,
                        C2DUIOption::Type::INPUT));
    ui_options->emplace_back(
            C2DUIOption("JOY_START1", {"7"}, KEY_JOY_START1_DEFAULT, C2DUIOption::Index::JOY_START1,
                        C2DUIOption::Type::INPUT));
    // TODO: add gui option for axis in option menu
    ui_options->emplace_back(
            C2DUIOption("JOY_AXIS_LX", {"0"}, KEY_JOY_AXIS_LX, C2DUIOption::Index::JOY_AXIS_LX,
                        C2DUIOption::Type::HIDDEN));
    ui_options->emplace_back(
            C2DUIOption("JOY_AXIS_LY", {"1"}, KEY_JOY_AXIS_LY, C2DUIOption::Index::JOY_AXIS_LY,
                        C2DUIOption::Type::HIDDEN));
    ui_options->emplace_back(
            C2DUIOption("JOY_AXIS_RX", {"2"}, KEY_JOY_AXIS_RX, C2DUIOption::Index::JOY_AXIS_RX,
                        C2DUIOption::Type::HIDDEN));
    ui_options->emplace_back(
            C2DUIOption("JOY_AXIS_RY", {"3"}, KEY_JOY_AXIS_RY, C2DUIOption::Index::JOY_AXIS_RY,
                        C2DUIOption::Type::HIDDEN));
    ui_options->emplace_back(C2DUIOption("JOY_DEADZONE",
                                         {"2000", "4000", "6000", "8000", "10000", "12000", "14000", "16000",
                                          "18000", "20000", "22000", "24000", "26000", "28000", "30000"}, 3,
                                         C2DUIOption::Index::JOY_DEADZONE, C2DUIOption::Type::INTEGER));

#ifndef NO_KEYBOARD
    // keyboard
    ui_options->emplace_back(
            C2DUIOption("KEYBOARD", {"KEYBOARD"}, 0, C2DUIOption::Index::MENU_KEYBOARD, C2DUIOption::Type::MENU));
    ui_options->emplace_back(C2DUIOption("KEY_UP", {std::to_string(KEY_KB_UP_DEFAULT)}, KEY_KB_UP_DEFAULT,
                                         C2DUIOption::Index::KEY_UP, C2DUIOption::Type::INPUT));        // KP_UP
    ui_options->emplace_back(C2DUIOption("KEY_DOWN", {std::to_string(KEY_KB_DOWN_DEFAULT)}, KEY_KB_DOWN_DEFAULT,
                                         C2DUIOption::Index::KEY_DOWN, C2DUIOption::Type::INPUT));    // KP_DOWN
    ui_options->emplace_back(C2DUIOption("KEY_LEFT", {std::to_string(KEY_KB_LEFT_DEFAULT)}, KEY_KB_LEFT_DEFAULT,
                                         C2DUIOption::Index::KEY_LEFT, C2DUIOption::Type::INPUT));    // KP_LEFT
    ui_options->emplace_back(C2DUIOption("KEY_RIGHT", {std::to_string(KEY_KB_RIGHT_DEFAULT)}, KEY_KB_RIGHT_DEFAULT,
                                         C2DUIOption::Index::KEY_RIGHT, C2DUIOption::Type::INPUT));  // KP_RIGHT
    ui_options->emplace_back(C2DUIOption("KEY_FIRE1", {std::to_string(KEY_KB_FIRE1_DEFAULT)}, KEY_KB_FIRE1_DEFAULT,
                                         C2DUIOption::Index::KEY_FIRE1, C2DUIOption::Type::INPUT));  // KP_1
    ui_options->emplace_back(C2DUIOption("KEY_FIRE2", {std::to_string(KEY_KB_FIRE2_DEFAULT)}, KEY_KB_FIRE2_DEFAULT,
                                         C2DUIOption::Index::KEY_FIRE2, C2DUIOption::Type::INPUT));  // KP_2
    ui_options->emplace_back(C2DUIOption("KEY_FIRE3", {std::to_string(KEY_KB_FIRE3_DEFAULT)}, KEY_KB_FIRE3_DEFAULT,
                                         C2DUIOption::Index::KEY_FIRE3, C2DUIOption::Type::INPUT));  // KP_3
    ui_options->emplace_back(C2DUIOption("KEY_FIRE4", {std::to_string(KEY_KB_FIRE4_DEFAULT)}, KEY_KB_FIRE4_DEFAULT,
                                         C2DUIOption::Index::KEY_FIRE4, C2DUIOption::Type::INPUT));  // KP_4
    ui_options->emplace_back(C2DUIOption("KEY_FIRE5", {std::to_string(KEY_KB_FIRE5_DEFAULT)}, KEY_KB_FIRE5_DEFAULT,
                                         C2DUIOption::Index::KEY_FIRE5, C2DUIOption::Type::INPUT));  // KP_5
    ui_options->emplace_back(C2DUIOption("KEY_FIRE6", {std::to_string(KEY_KB_FIRE6_DEFAULT)}, KEY_KB_FIRE6_DEFAULT,
                                         C2DUIOption::Index::KEY_FIRE6, C2DUIOption::Type::INPUT));  // KP_6
    ui_options->emplace_back(C2DUIOption("KEY_COIN1", {std::to_string(KEY_KB_COIN1_DEFAULT)}, KEY_KB_COIN1_DEFAULT,
                                         C2DUIOption::Index::KEY_COIN1, C2DUIOption::Type::INPUT));  // ESCAPE
    ui_options->emplace_back(C2DUIOption("KEY_START1", {std::to_string(KEY_KB_START1_DEFAULT)}, KEY_KB_START1_DEFAULT,
                                         C2DUIOption::Index::KEY_START1, C2DUIOption::Type::INPUT));// ENTER
#endif

}

void C2DUIConfig::load(C2DUIRomList::Rom *rom) {

    config_t cfg;
    config_init(&cfg);

    bool isRomCfg = rom != nullptr;
    std::vector<C2DUIOption> *options = isRomCfg ? &options_rom : &options_gui;
    std::string path = configPath;
    if (isRomCfg) {
        path = homePath;
        path += "configs/";
        path += rom->drv_name;
        path += ".cfg";
    }

    printf("C2DUIConfig::load: %s ...", path.c_str());

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
                } else {
                    //printf("rom_paths setting not found\n");
                }
            }

            for (auto &option : *options) {
                if (option.flags & C2DUIOption::Type::MENU) {
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
        //printf("###########################\n");
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

void C2DUIConfig::save(C2DUIRomList::Rom *rom) {

    config_t cfg{};
    config_init(&cfg);

    bool isRomCfg = rom != nullptr;
    std::vector<C2DUIOption> *options = isRomCfg ? &options_rom : &options_gui;
    std::string path = configPath;
    if (isRomCfg) {
        path = homePath;
        path += "configs/";
        path += rom->drv_name;
        path += ".cfg";
    }

    printf("C2DUIConfig::save: %s\n", path.c_str());

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
        if (option.flags & C2DUIOption::Type::MENU) {
            sub_setting = config_setting_add(setting_fba, option.getName(), CONFIG_TYPE_GROUP);
            continue;
        }
        config_setting_t *setting = config_setting_add(sub_setting, option.getName(), CONFIG_TYPE_INT);
        config_setting_set_int(setting, option.value);
    }

    config_write_file(&cfg, path.c_str());
    config_destroy(&cfg);
}

void C2DUIConfig::reset(bool isRom) {

    options_rom.clear();

    int start = 0, end = (int) options_gui.size();
    for (unsigned int i = 0; i < options_gui.size(); i++) {
        if (options_gui[i].id == C2DUIOption::Index::MENU_ROM_OPTIONS) {
            start = i;
            break;
        }
    }

    for (int i = start; i < end; i++) {
        options_rom.emplace_back(options_gui[i]);
    }
}

std::string *C2DUIConfig::getHomePath() {
    return &homePath;
}

std::string *C2DUIConfig::getRomPath(int n) {
    if (n >= C2DUI_ROMS_PATHS_MAX) {
        return &roms_paths[0];
    } else {
        return &roms_paths[n];
    }
}

std::vector<std::string> *C2DUIConfig::getRomPaths() {
    return &roms_paths;
}

std::vector<C2DUIOption> *C2DUIConfig::get(bool isRom) {
    return isRom ? &options_rom : &options_gui;
}

C2DUIOption *C2DUIConfig::get(int index, bool isRom) {

    std::vector<C2DUIOption> *options =
            isRom ? &options_rom : &options_gui;

    for (auto &option : *options) {
        if (option.id == index) {
            return &option;
        }
    }
    return nullptr;
}

bool C2DUIConfig::add(int target,
                      const std::string &text, const std::vector<std::string> &values,
                      int defaultValue, int index, bool isRom, unsigned int flags) {

    std::vector<C2DUIOption> *options =
            isRom ? &options_rom : &options_gui;

    for (auto &option : *options) {
        if (option.id == index) {
            options->insert(options->begin() + target + 1,
                            C2DUIOption(text, values, defaultValue, index, flags));
            return true;
        }
    }

    return false;
}

int C2DUIConfig::getValue(int index, bool isRom) {

    std::vector<C2DUIOption> *options =
            isRom ? &options_rom : &options_gui;

    for (auto &option : *options) {
        if (option.id == index) {
            return option.value;
        }
    }

    return 0;
}

bool C2DUIConfig::hide(int index, bool isRom) {

    std::vector<C2DUIOption> *options =
            isRom ? &options_rom : &options_gui;

    for (auto &option : *options) {
        if (option.id == index) {
            option.flags |= C2DUIOption::HIDDEN;
            return true;
        }
    }

    return false;
}

int *C2DUIConfig::getGuiPlayerInputKeys(int player) {

#ifndef NO_KEYBOARD
    // TODO: player > 0 not supported yet
    keyboard_keys[0] = getValue(C2DUIOption::Index::KEY_UP);
    keyboard_keys[1] = getValue(C2DUIOption::Index::KEY_DOWN);
    keyboard_keys[2] = getValue(C2DUIOption::Index::KEY_LEFT);
    keyboard_keys[3] = getValue(C2DUIOption::Index::KEY_RIGHT);
    keyboard_keys[4] = getValue(C2DUIOption::Index::KEY_COIN1);
    keyboard_keys[5] = getValue(C2DUIOption::Index::KEY_START1);
    keyboard_keys[6] = getValue(C2DUIOption::Index::KEY_FIRE1);
    keyboard_keys[7] = getValue(C2DUIOption::Index::KEY_FIRE2);
    keyboard_keys[8] = getValue(C2DUIOption::Index::KEY_FIRE3);
    keyboard_keys[9] = getValue(C2DUIOption::Index::KEY_FIRE4);
    keyboard_keys[10] = getValue(C2DUIOption::Index::KEY_FIRE5);
    keyboard_keys[11] = getValue(C2DUIOption::Index::KEY_FIRE6);
#endif

    return keyboard_keys;
}

int *C2DUIConfig::getGuiPlayerInputButtons(int player) {

    // TODO: player > 0 not supported yet
    joystick_keys[0] = getValue(C2DUIOption::Index::JOY_UP);
    joystick_keys[1] = getValue(C2DUIOption::Index::JOY_DOWN);
    joystick_keys[2] = getValue(C2DUIOption::Index::JOY_LEFT);
    joystick_keys[3] = getValue(C2DUIOption::Index::JOY_RIGHT);
    joystick_keys[4] = getValue(C2DUIOption::Index::JOY_COIN1);
    joystick_keys[5] = getValue(C2DUIOption::Index::JOY_START1);
    joystick_keys[6] = getValue(C2DUIOption::Index::JOY_FIRE1);
    joystick_keys[7] = getValue(C2DUIOption::Index::JOY_FIRE2);
    joystick_keys[8] = getValue(C2DUIOption::Index::JOY_FIRE3);
    joystick_keys[9] = getValue(C2DUIOption::Index::JOY_FIRE4);
    joystick_keys[10] = getValue(C2DUIOption::Index::JOY_FIRE5);
    joystick_keys[11] = getValue(C2DUIOption::Index::JOY_FIRE6);

    return joystick_keys;
}

int *C2DUIConfig::getRomPlayerInputKeys(int player) {

#ifndef NO_KEYBOARD
    // TODO: player > 0 not supported yet
    keyboard_keys[0] = getValue(C2DUIOption::Index::KEY_UP, true);
    keyboard_keys[1] = getValue(C2DUIOption::Index::KEY_DOWN, true);
    keyboard_keys[2] = getValue(C2DUIOption::Index::KEY_LEFT, true);
    keyboard_keys[3] = getValue(C2DUIOption::Index::KEY_RIGHT, true);
    keyboard_keys[4] = getValue(C2DUIOption::Index::KEY_COIN1, true);
    keyboard_keys[5] = getValue(C2DUIOption::Index::KEY_START1, true);
    keyboard_keys[6] = getValue(C2DUIOption::Index::KEY_FIRE1, true);
    keyboard_keys[7] = getValue(C2DUIOption::Index::KEY_FIRE2, true);
    keyboard_keys[8] = getValue(C2DUIOption::Index::KEY_FIRE3, true);
    keyboard_keys[9] = getValue(C2DUIOption::Index::KEY_FIRE4, true);
    keyboard_keys[10] = getValue(C2DUIOption::Index::KEY_FIRE5, true);
    keyboard_keys[11] = getValue(C2DUIOption::Index::KEY_FIRE6, true);
#endif

    return keyboard_keys;
}

int *C2DUIConfig::getRomPlayerInputButtons(int player) {

    // TODO: player > 0 not supported yet
    joystick_keys[0] = getValue(C2DUIOption::Index::JOY_UP, true);
    joystick_keys[1] = getValue(C2DUIOption::Index::JOY_DOWN, true);
    joystick_keys[2] = getValue(C2DUIOption::Index::JOY_LEFT, true);
    joystick_keys[3] = getValue(C2DUIOption::Index::JOY_RIGHT, true);
    joystick_keys[4] = getValue(C2DUIOption::Index::JOY_COIN1, true);
    joystick_keys[5] = getValue(C2DUIOption::Index::JOY_START1, true);
    joystick_keys[6] = getValue(C2DUIOption::Index::JOY_FIRE1, true);
    joystick_keys[7] = getValue(C2DUIOption::Index::JOY_FIRE2, true);
    joystick_keys[8] = getValue(C2DUIOption::Index::JOY_FIRE3, true);
    joystick_keys[9] = getValue(C2DUIOption::Index::JOY_FIRE4, true);
    joystick_keys[10] = getValue(C2DUIOption::Index::JOY_FIRE5, true);
    joystick_keys[11] = getValue(C2DUIOption::Index::JOY_FIRE6, true);

    return joystick_keys;
}

std::vector<C2DUIRomList::Hardware> *C2DUIConfig::getHardwareList() {

    return &hardwareList;
}
