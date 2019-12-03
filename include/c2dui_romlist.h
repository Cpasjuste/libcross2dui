//
// Created by cpasjuste on 19/10/16.
//

#ifndef C2DUI_ROMLIST_H
#define C2DUI_ROMLIST_H

#include <vector>
#include "ss_gamelist.h"

#define HARDWARE_PREFIX_ALL 0xffffffff
#define HARDWARE_PREFIX_FAV 0x20000000

namespace c2dui {

    class UIMain;

    class RomList {

    public:

        struct Hardware {
            int prefix;
            std::string name;
            int clone_count = 0;
            int supported_count = 0;
            int available_count = 0;
            int available_clone_count = 0;
            int missing_count = 0;
            int missing_clone_count = 0;

            Hardware(int p, const std::string &n, int rs = 0, int ra = 0) {
                prefix = p;
                name = n;
                supported_count = rs;
                available_count = 0;
            }
        };

        RomList(UIMain *ui, const std::string &emuVersion);

        virtual ~RomList();

        virtual void build();

        void addFav(ss_api::Game *rom);

        void removeFav(ss_api::Game *rom);

        enum RomState {
            NOT_WORKING = 0,
            MISSING,
            WORKING
        };

        UIMain *ui;
        c2d::RectangleShape *rect;
        c2d::Text *text;
        ss_api::GameList gameList;
        std::vector<Hardware> *hardwareList;
        std::vector<std::string> paths;
        char icon_path[1024];
        char text_str[512];
        float time_start = 0;

        Hardware *getHardware(int hardware_prefix) {
            for (unsigned int i = 0; i < hardwareList->size(); i++) {
                if (isHardware(hardware_prefix, hardwareList->at(i).prefix)) {
                    return &hardwareList->at(i);
                }
            }
            return nullptr;
        };

        const char *getHardwareName(int hardware_prefix) {
            for (unsigned int i = 0; i < hardwareList->size(); i++) {
                if (hardwareList->at(i).prefix == hardware_prefix) {
                    return hardwareList->at(i).name.c_str();
                }
            }
            return "UNKNOW";
        };

        virtual bool isHardware(int hardware, int type) {
            return false;
        }
    };
}

#endif //C2DUI_ROMLIST_H
