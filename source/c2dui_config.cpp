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

    printf("C2DUIConfig::load: %s\n", path.c_str());

    if (config_read_file(&cfg, path.c_str())) {

        //printf("###########################\n");
        //printf("CFG FOUND: %s\n", path.c_str());
        config_setting_t *settings_root = config_lookup(&cfg, "FBA_CONFIG");
        if (settings_root) {
#ifdef __UPDATE_CONFIG__
            // verify cfg version
            int cfg_version = 0;
            if (!config_setting_lookup_int(settings_root, "VERSION", &cfg_version)
                || cfg_version != version) {
                // update cfg to newer version
                printf("CFG VERSION (%i) != PFBA VERSION (%i)\n", cfg_version, version);
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
                        //printf("%s: %i\n", options->at(i).GetName(), value);
                    }
                }
            }
        }
        //printf("###########################\n");
    } else {
        // reset default rom options for other roms usage
        options_rom.clear();
        for (int i = C2DUIOption::Index::MENU_ROM_OPTIONS; i < C2DUIOption::Index::END; i++) {
            options_rom.emplace_back(options_gui[i]);
        }
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
    config_setting_t *setting_fba = config_setting_add(setting_root, "FBA_CONFIG", CONFIG_TYPE_GROUP);
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
        if (option.index == C2DUIOption::Index::END) {
            continue;
        }
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

int C2DUIConfig::getValue(int index, bool rom) {

    std::vector<C2DUIOption> *opt =
            rom ? &options_rom : &options_gui;
    for (auto &i : *opt) {
        if (index == i.index) {
            return i.value;
        }
    }

    return 0;
}

std::string *C2DUIConfig::getHomePath() {
    return &homePath;
}

const char *C2DUIConfig::getRomPath(int n) {
    if (n >= C2DUI_ROMS_PATHS_MAX) {
        return roms_paths[0].c_str();
    } else {
        return roms_paths[n].c_str();
    }
}

std::vector<std::string> *C2DUIConfig::getRomPaths() {
    return &roms_paths;
}

std::vector<C2DUIOption> *C2DUIConfig::getOptions(bool rom) {
    return rom ? &options_rom : &options_gui;
}

C2DUIOption *C2DUIConfig::getOption(std::vector<C2DUIOption> *options, int index) {
    for (unsigned int i = 0; i < options->size(); i++) {
        if (options->at(i).index == index) {
            return &options->at(i);
        }
    }
    return nullptr;
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
