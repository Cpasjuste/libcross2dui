//
// Created by cpasjuste on 19/10/16.
//

#ifndef _ROMLIST_H
#define _ROMLIST_H

#include <vector>

#define HARDWARE_PREFIX_ALL 0xffffffff

namespace c2dui {

    class C2DUIGuiMain;

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

        virtual ~C2DUIRomList();

        virtual void build();

        enum RomState {
            NOT_WORKING = 0,
            MISSING,
            WORKING
        };

        class Rom : public c2d::Io::File {
        public:
            const char *parent = NULL;
            const char *drv_name;
            const char *year = NULL;
            const char *manufacturer = NULL;
            const char *system = NULL;
            unsigned int flags;
            int state = RomState::MISSING;
            int hardware;
            unsigned int drv;
            int genre;
        };

        C2DUIGuiMain *ui;
        c2d::Rectangle *rect;
        c2d::Texture *texture;
        c2d::Text *text;
        std::vector<Rom *> list;
        std::vector<Hardware> *hardwareList;
        std::vector<std::string> *paths;
        std::vector<std::vector<std::string>> files;
        char icon_path[1024];
        char text_str[512];
        float time_start = 0;

        Hardware *getHardware(int hardware_prefix) {
            for (unsigned int i = 0; i < hardwareList->size(); i++) {
                if (isHardware(hardware_prefix, hardwareList->at(i).prefix)) {
                    return &hardwareList->at(i);
                }
            }
            return NULL;
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

#endif //_ROMLIST_H
