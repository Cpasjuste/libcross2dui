//
// Created by cpasjuste on 20/05/19.
//

#ifdef __SSCRAP__

#ifndef PEMU_C2DUI_SCRAPPER_H
#define PEMU_C2DUI_SCRAPPER_H

#include <string>
#include <vector>
#include <SDL_thread.h>

#include "ss_api.h"

namespace c2dui {

    class Scrapper {

    public:
        Scrapper(UIMain *ui);

        ~Scrapper();

        void addRom(const std::string &rom);

        UIMain *main = nullptr;
        ss_api::Api *scrap = nullptr;
        std::vector<std::string> scrapList;
        SDL_Thread *thread = nullptr;
        SDL_mutex *mutex = nullptr;
        bool running = true;
    };
}

#endif //PEMU_C2DUI_SCRAPPER_H
#endif // SSCRAP