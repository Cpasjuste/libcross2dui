//
// Created by cpasjuste on 05/12/16.
//

#include <string>
#include <libconfig.h>
#include "config.h"

using namespace c2d;

Config::Config(const std::string &home,
               const std::vector<std::string> &roms_paths,
               const std::vector<Option> &options,
               const std::vector<RomList::Hardware> &hardwares) {

    homePath = home;
    configPath = homePath + "config.cfg";

    // gui config
    options_gui = options;

    // supported hardwares
    hardwareList = hardwares;

    // set default rom options
    options_rom.clear();
    for (int i = Option::Index::MENU_ROM_OPTIONS; i < Option::Index::END; i++) {
        options_rom.emplace_back(options_gui[i]);
    }

    load();
}

void Config::load(RomList::Rom *rom) {

    config_t cfg;
    config_init(&cfg);

    bool isRomCfg = rom != NULL;
    std::vector<Option> *options = isRomCfg ? &options_rom : &options_gui;
    std::string path = configPath;
    if (isRomCfg) {
        path = homePath;
        path += "configs/";
        path += rom->drv_name;
        path += ".cfg";
    }

    if (config_read_file(&cfg, path.c_str())) {

        //printf("###########################\n");
        //printf("CFG FOUND: %s\n", path.c_str());

        config_setting_t *settings_root = config_lookup(&cfg, "FBA_CONFIG");
        if (settings_root) {
            // verify cfg version
            int version = 0;
            if (!config_setting_lookup_int(settings_root, "VERSION", &version)
                || version != (__PFBA_VERSION_MAJOR__ * 100) + __PFBA_VERSION_MINOR__) {
                // update cfg to newer version
                printf("CFG VERSION (%i) != PFBA VERSION (%i)\n", version,
                       (__PFBA_VERSION_MAJOR__ * 100) + __PFBA_VERSION_MINOR__);
                save(rom);
                config_destroy(&cfg);
                return;
            }

            config_setting_t *settings = NULL;
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

            for (unsigned long i = 0; i < options->size(); i++) {
                if (options->at(i).flags & Option::Type::MENU) {
                    settings = config_setting_lookup(settings_root, options->at(i).getName());
                }
                if (settings) {
                    int value = 0;
                    if (config_setting_lookup_int(settings, options->at(i).getName(), &value)) {
                        options->at(i).value = value;
                        //printf("%s: %i\n", options->at(i).GetName(), value);
                    }
                }
            }
        }
        //printf("###########################\n");
    } else if (!isRomCfg) {
        // no need to save default rom config
        save();
    }

    config_destroy(&cfg);
}

void Config::save(RomList::Rom *rom) {

    config_t cfg{};
    config_init(&cfg);

    bool isRomCfg = rom != NULL;
    std::vector<Option> *options = isRomCfg ? &options_rom : &options_gui;
    std::string path = configPath;
    if (isRomCfg) {
        path = homePath;
        path += "configs/";
        path += rom->drv_name;
        path += ".cfg";
    }

    // create root
    config_setting_t *setting_root = config_root_setting(&cfg);

    // create main group
    config_setting_t *setting_fba = config_setting_add(setting_root, "FBA_CONFIG", CONFIG_TYPE_GROUP);
    // add version
    config_setting_t *setting_cfg = config_setting_add(setting_fba, "VERSION", CONFIG_TYPE_INT);
    config_setting_set_int(setting_cfg, (__PFBA_VERSION_MAJOR__ * 100) + __PFBA_VERSION_MINOR__);

    config_setting_t *sub_setting = NULL;

    if (!isRomCfg) {
        sub_setting = config_setting_add(setting_fba, "ROMS_PATHS", CONFIG_TYPE_GROUP);
        for (unsigned int i = 0; i < roms_paths.size(); i++) {
            char p[MAX_PATH];
            snprintf(p, MAX_PATH, "ROMS_PATH%i", i);
            config_setting_t *setting = config_setting_add(sub_setting, p, CONFIG_TYPE_STRING);
            config_setting_set_string(setting, roms_paths[i].c_str());
        }
    }

    for (unsigned long i = 0; i < options->size(); i++) {
        if (options->at(i).index == Option::Index::END) {
            continue;
        }
        if (options->at(i).flags & Option::Type::MENU) {
            sub_setting = config_setting_add(setting_fba, options->at(i).getName(), CONFIG_TYPE_GROUP);
            continue;
        }
        config_setting_t *setting = config_setting_add(sub_setting, options->at(i).getName(), CONFIG_TYPE_INT);
        config_setting_set_int(setting, options->at(i).value);
    }

    config_write_file(&cfg, path.c_str());
    config_destroy(&cfg);

    if (!isRomCfg) {
        // set default rom options // TODO: why did i do that ?
        options_rom.clear();
        for (int i = Option::Index::MENU_ROM_OPTIONS; i < Option::Index::END; i++) {
            options_rom.emplace_back(options_gui[i]);
        }
    }
}

int Config::getValue(int index, bool rom) {

    std::vector<Option> *opt =
            rom ? &options_rom : &options_gui;
    for (auto &i : *opt) {
        if (index == i.index) {
            return i.value;
        }
    }

    return 0;
}

const char *Config::getRomPath(int n) {
    if (n >= DIRS_MAX) {
        return roms_paths[0].c_str();
    } else {
        return roms_paths[n].c_str();
    }
}

std::vector<std::string> Config::getRomPaths() {
    return roms_paths;
}

std::vector<Option> *Config::getOptions(bool rom) {
    return rom ? &options_rom : &options_gui;
}

Option *Config::getOption(std::vector<Option> *options, int index) {
    for (unsigned int i = 0; i < options->size(); i++) {
        if (options->at(i).index == index) {
            return &options->at(i);
        }
    }
    return NULL;
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
    //keyboard_keys[12] = getValue(Option::Index::KEY_MENU1);
    //keyboard_keys[13] = getValue(Option::Index::KEY_MENU2);
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
    //joystick_keys[12] = getValue(Option::Index::JOY_MENU1);
    //joystick_keys[13] = getValue(Option::Index::JOY_MENU2);

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
    //keyboard_keys[12] = getValue(Option::Index::KEY_MENU1, true);
    //keyboard_keys[13] = getValue(Option::Index::KEY_MENU2, true);
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
    //joystick_keys[12] = getValue(Option::Index::JOY_MENU1, true);
    //joystick_keys[13] = getValue(Option::Index::JOY_MENU2, true);

    return joystick_keys;
}
