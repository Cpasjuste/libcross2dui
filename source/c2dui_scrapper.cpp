//
// Created by cpasjuste on 20/05/19.
//
#if 0
#ifdef __SSCRAP__

#include <algorithm>
#include <SDL_timer.h>
#include <c2dui.h>
#include "c2dui_scrapper.h"

using namespace c2dui;
using namespace ss_api;

static int scrap_thread(void *ptr) {

    auto scrapper = (Scrapper *) ptr;
    std::string rom;

    while (scrapper->running) {

        // get first rom name to be scrapped and remove it from list
        SDL_LockMutex(scrapper->mutex);
        if (!scrapper->scrapList.empty()) {
            rom = scrapper->scrapList[0];
            scrapper->scrapList.erase(scrapper->scrapList.begin());
        }
        SDL_UnlockMutex(scrapper->mutex);

        // if we have nothing to do, sleep a little
        if (rom.empty()) {
            SDL_Delay(100);
            continue;
        }

        std::string cachePath = "cache/" + rom + ".json";
        // check if json rom already cache exist
        if (scrapper->main->getIo()->exist(cachePath)) {
            rom.clear();
            continue;
        }

        // scrap rom
        Api::JeuInfos jeuInfos = scrapper->scrap->jeuInfos("", "", "", "3", "rom", rom, "", "", SS_ID, SS_PWD);
        if (!jeuInfos.jeu.id.empty()) {
            // game found, save json data
            jeuInfos.save(cachePath);
            rom = c2d::Utility::removeExt(rom);
            // download title image
            std::vector<Jeu::Media> medias = scrapper->scrap->getMedia(
                    jeuInfos.jeu, Jeu::Media::Type::SSTitle, Api::Region::WOR);
            if (!medias.empty() && scrapper->running) {
                std::string path = scrapper->main->getConfig()->getTitlesPath() + rom + ".png";
                if (!scrapper->main->getIo()->exist(path)) {
                    scrapper->scrap->download(medias[0], path);
                }
            }
            // download preview image
            medias = scrapper->scrap->getMedia(
                    jeuInfos.jeu, Jeu::Media::Type::SS, Api::Region::WOR);
            if (!medias.empty() && scrapper->running) {
                std::string path = scrapper->main->getConfig()->getPreviewsPath() + rom + ".png";
                if (!scrapper->main->getIo()->exist(path)) {
                    scrapper->scrap->download(medias[0], path);
                }
            }
            // download mix image
            medias = scrapper->scrap->getMedia(
                    jeuInfos.jeu, Jeu::Media::Type::Mixrbv2, Api::Region::WOR);
            if (!medias.empty() && scrapper->running) {
                std::string path = scrapper->main->getConfig()->getMixesPath() + rom + ".png";
                if (!scrapper->main->getIo()->exist(path)) {
                    scrapper->scrap->download(medias[0], path);
                }
            }
        }

        // done
        rom.clear();
    }

    return 0;
}

Scrapper::Scrapper(UIMain *m) {

    main = m;
    scrap = new Api(SS_DEV_ID, SS_DEV_PWD, "pemu");
    mutex = SDL_CreateMutex();
    thread = SDL_CreateThread(scrap_thread, "scrap_thread", (void *) this);
}

void Scrapper::addRom(const std::string &rom) {

    SDL_LockMutex(mutex);

    if (std::find(scrapList.begin(), scrapList.end(), rom) == scrapList.end()) {
        printf("Scrapper::addRom: %s\n", rom.c_str());
        scrapList.insert(scrapList.begin(), rom);
    }

    SDL_UnlockMutex(mutex);
}

Scrapper::~Scrapper() {

    running = false;
    SDL_LockMutex(mutex);
    SDL_WaitThread(thread, nullptr);
    SDL_UnlockMutex(mutex);
    SDL_DestroyMutex(mutex);
    delete (scrap);
    printf("~Scrapper\n");
}

#endif // SSCRAP
#endif