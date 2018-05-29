//
// Created by cpasjuste on 19/10/16.
//

#ifndef _ROMLIST_H
#define _ROMLIST_H

#include <vector>

namespace c2dui {

    class C2DUIRomList {

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

        C2DUIRomList(C2DUIGuiMain *ui, const std::string &emuVersion);

        ~C2DUIRomList();

        virtual void build() {};
        void clean();

        enum RomState {
            NOT_WORKING = 0,
            MISSING,
            WORKING
        };

        class Rom : public c2d::Io::File {
        public:
            char zip[64];
            char *drv_name;
            char *year = NULL;
            char *manufacturer = NULL;
            char *parent = NULL;
            int flags;
            int state;
            int hardware;
            char *system = NULL;
            unsigned int drv;
            int genre;
        };

        C2DUIGuiMain *ui;
        Rectangle *rect;
        Texture *texture;
        Text *text;
        std::vector<Rom *> list;
        std::vector<Hardware> *hardwareList;
        std::vector<std::string> *paths;
        std::vector<std::string> files[C2DUI_ROMS_PATHS_MAX];
        char text_str[512];
        float time_start = 0;

        Hardware *GetHardware(int hardware_prefix) {
            for (unsigned int i = 0; i < hardwareList->size(); i++) {
                if (IsHardware(hardware_prefix, hardwareList->at(i).prefix)) {
                    return &hardwareList->at(i);
                }
            }
            return NULL;
        };

        const char *GetHardwareName(int hardware_prefix) {
            for (unsigned int i = 0; i < hardwareList->size(); i++) {
                if (hardwareList->at(i).prefix == hardware_prefix) {
                    return hardwareList->at(i).name.c_str();
                }
            }
            return "UNKNOW";
        };
    };
}

#endif //_ROMLIST_H
