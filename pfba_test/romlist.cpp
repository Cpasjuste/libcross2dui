//
// Created by cpasjuste on 29/05/18.
//

#include "c2dui.h"
#include "romlist.h"

using namespace c2d;
using namespace c2dui;

static inline bool endsWith(std::string const &value, std::string const &ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

RomList::RomList(C2DUIGuiMain *ui, const std::string &emuVersion) : C2DUIRomList(ui, emuVersion) {

}

void RomList::build() {

    char path[MAX_PATH];
    char pathUppercase[MAX_PATH]; // sometimes on FAT32 short files appear as all uppercase

    for (unsigned int i = 0; i < nBurnDrvCount; i++) {

        nBurnDrvActive = i;

        auto *rom = new Rom();
        char *zn;
        BurnDrvGetZipName(&zn, 0);
        strncpy(rom->zip, zn, 63);
        rom->drv = i;
        rom->drv_name = BurnDrvGetTextA(DRV_NAME);
        rom->parent = BurnDrvGetTextA(DRV_PARENT);
        rom->name = BurnDrvGetTextA(DRV_FULLNAME);
        rom->year = BurnDrvGetTextA(DRV_DATE);
        rom->manufacturer = BurnDrvGetTextA(DRV_MANUFACTURER);
        rom->system = BurnDrvGetTextA(DRV_SYSTEM);
        rom->genre = BurnDrvGetGenreFlags();
        rom->flags = BurnDrvGetFlags();
        rom->state = RomState::MISSING;
        rom->hardware = BurnDrvGetHardwareCode();

        // add rom to "ALL" game list
        hardwareList->at(0).supported_count++;
        if (rom->parent) {
            hardwareList->at(0).clone_count++;
        }

        // add rom to specific hardware
        Hardware *hardware = getHardware(rom->hardware);
        if (hardware) {
            hardware->supported_count++;
            if (rom->parent) {
                hardware->clone_count++;
            }
        }

        snprintf(path, MAX_PATH, "%s.zip", rom->zip);
        for (int k = 0; k < MAX_PATH; k++) {
            pathUppercase[k] = (char) toupper(path[k]);
        }

        for (unsigned int j = 0; j < C2DUI_ROMS_PATHS_MAX; j++) {

            if (files[j].empty()) {
                continue;
            }

            auto file = std::find(files[j].begin(), files[j].end(), path);
            if (file == files[j].end()) {
                file = std::find(files[j].begin(), files[j].end(), pathUppercase);
            }

            if (file != files[j].end()) {

                int prefix = (((rom->hardware | HARDWARE_PREFIX_CARTRIDGE) ^ HARDWARE_PREFIX_CARTRIDGE) & 0xff000000);

                switch (prefix) {
                    case HARDWARE_PREFIX_COLECO:
                        if (!endsWith(paths->at(j), "coleco/")) {
                            continue;
                        }
                        break;
                    case HARDWARE_PREFIX_SEGA_GAME_GEAR:
                        if (!endsWith(paths->at(j), "gamegear/")) {
                            continue;
                        }
                        break;
                    case HARDWARE_PREFIX_SEGA_MEGADRIVE:
                        if (!endsWith(paths->at(j), "megadriv/")) {
                            continue;
                        }
                        break;
                    case HARDWARE_PREFIX_MSX:
                        if (!endsWith(paths->at(j), "msx/")) {
                            continue;
                        }
                        break;
                    case HARDWARE_PREFIX_SEGA_SG1000:
                        if (!endsWith(paths->at(j), "sg1000/")) {
                            continue;
                        }
                        break;
                    case HARDWARE_PREFIX_SEGA_MASTER_SYSTEM:
                        if (!endsWith(paths->at(j), "sms/")) {
                            continue;
                        }
                        break;
                    case HARDWARE_PREFIX_PCENGINE:
                        switch (rom->hardware) {
                            case HARDWARE_PCENGINE_PCENGINE:
                                if (!endsWith(paths->at(j), "pce/")) {
                                    continue;
                                }
                                break;
                            case HARDWARE_PCENGINE_TG16:
                                if (!endsWith(paths->at(j), "tg16/")) {
                                    continue;
                                }
                                break;
                            case HARDWARE_PCENGINE_SGX:
                                if (!endsWith(paths->at(j), "sgx/")) {
                                    continue;
                                }
                                break;
                            default:
                                continue;
                        }
                        break;
                    default:
                        if (endsWith(paths->at(j), "coleco/")
                            || endsWith(paths->at(j), "gamegear/")
                            || endsWith(paths->at(j), "megadriv/")
                            || endsWith(paths->at(j), "msx/")
                            || endsWith(paths->at(j), "sg1000/")
                            || endsWith(paths->at(j), "sms/")
                            || endsWith(paths->at(j), "pce/")
                            || endsWith(paths->at(j), "sgx/")
                            || endsWith(paths->at(j), "tg16/")) {
                            continue;
                        }
                        break;
                }

                //snprintf(rom->zip_path, 256, "%s%s", paths[j].c_str(), file->c_str());
                rom->state = BurnDrvIsWorking() ? RomState::WORKING : RomState::NOT_WORKING;
                hardwareList->at(0).available_count++;

                if (rom->parent) {
                    hardwareList->at(0).available_clone_count++;
                }

                if (hardware) {
                    hardware->available_count++;
                    if (rom->parent) {
                        hardware->available_clone_count++;
                    }
                }
                break;
            }
        }

        // set "Io::File"" color for ui
        rom->color = COL_RED;
        if (rom->state == C2DUIRomList::RomState::NOT_WORKING) {
            rom->color = COL_ORANGE;
        } else if (rom->state == C2DUIRomList::RomState::WORKING) {
            rom->color = rom->parent == nullptr ? COL_GREEN : COL_YELLOW;
        }

        if (rom->state == RomState::MISSING) {
            hardwareList->at(0).missing_count++;
            if (hardware) {
                hardware->missing_count++;
            }
            if (rom->parent) {
                hardwareList->at(0).missing_clone_count++;
                if (hardware) {
                    hardware->missing_clone_count++;
                }
            }
        }

        list.push_back(rom);

        // UI
        if (i % 250 == 0) {
            sprintf(text_str, "Scanning... %i%% - ROMS : %i / %i",
                    (i * 100) / nBurnDrvCount, hardwareList->at(0).supported_count, nBurnDrvCount);
            text->setString(text_str);
            ui->getRenderer()->flip();
        }
        // UI
    }

    clean();
}
