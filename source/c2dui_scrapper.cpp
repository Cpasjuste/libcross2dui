//
// Created by cpasjuste on 20/05/19.
//

#include <algorithm>
#include <SDL_timer.h>
#include <c2dui.h>
#include "c2dui_scrapper.h"

using namespace c2dui;
using namespace ss_api;

static int scrap_thread(void *ptr) {

    auto scrapper = (Scrapper *) ptr;
    std::vector<std::string> roms;

    while (scrapper->running) {

        SDL_Delay(100);

        SDL_LockMutex(scrapper->mutex);
        roms = scrapper->scrapList;
        scrapper->scrapList.clear();
        SDL_UnlockMutex(scrapper->mutex);

        if (!roms.empty()) {
            for (const auto &rom : roms) {

                if (!scrapper->running) {
                    break;
                }

                std::string jsonPath = "cache/" + rom + ".json";

                if (scrapper->main->getIo()->exist(jsonPath)) {
                    continue;
                }

                Api::JeuInfos jeuInfos = scrapper->scrap->jeuInfos("cache/" + rom + ".json");
                if (jeuInfos.jeu.id.empty()) {
                    jeuInfos = scrapper->scrap->jeuInfos("", "", "", "3", "rom", rom, "", "", SSID, SSPWD);
                    if (!jeuInfos.jeu.id.empty()) {
                        jeuInfos.save("cache/" + rom + ".json");
                        std::vector<Jeu::Media> medias = scrapper->scrap->getMedia(
                                jeuInfos.jeu, Jeu::Media::Type::Mixrbv2, Api::Region::WOR);
                        if (!medias.empty()) {
                            std::string name = c2d::Utility::removeExt(rom);
                            std::string type = "titles";
                            std::string home_path = scrapper->main->getConfig()->getHomePath();
                            std::string path = home_path + type + "/" + name + ".png";
                            if (!scrapper->main->getIo()->exist(path)) {
                                scrapper->scrap->download(medias[0], path);
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}

Scrapper::Scrapper(UIMain *m) {

    main = m;
    scrap = new Api(DEVID, DEVPWD, "pemu");
    mutex = SDL_CreateMutex();
    thread = SDL_CreateThread(scrap_thread, "scrap_thread", (void *) this);
}

void Scrapper::addRom(const std::string &rom) {

    SDL_LockMutex(mutex);

    if (std::find(scrapList.begin(), scrapList.end(), rom) == scrapList.end()) {
        printf("Scrapper::addRom: adding %s\n", rom.c_str());
        scrapList.insert(scrapList.begin(), rom);
    }

    SDL_UnlockMutex(mutex);
}

Scrapper::~Scrapper() {

    running = false;
    SDL_WaitThread(thread, nullptr);
    SDL_DestroyMutex(mutex);
    delete (scrap);
    printf("Scrapper::~Scrapper\n");
}
