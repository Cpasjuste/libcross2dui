//
// Created by cpasjuste on 22/11/16.
//

#include <algorithm>

#ifdef __SWITCH__
extern "C" {
#include <switch/services/hid.h>
}
#endif

#include "c2dui.h"

using namespace c2d;
using namespace c2dui;

C2DUIGuiMain::C2DUIGuiMain(Renderer *r, Io *i, Input *in, C2DUIConfig *cfg, C2DUISkin *s) {

    io = i;
    renderer = r;
    skin = s;
    config = cfg;
    input = in;

    // scaling factor mainly used for borders,
    // based on vita resolution..
    scaling = std::min(renderer->getSize().x / 960, 1.0f);
    printf("scaling: %f\n", scaling);

    uiMessageBox = new MessageBox(
            FloatRect(
                    renderer->getSize().x / 2,
                    renderer->getSize().y / 2,
                    renderer->getSize().x / 2,
                    renderer->getSize().y / 2),
            input, *skin->font, getFontSize());
    uiMessageBox->setOriginCenter();
    uiMessageBox->setFillColor(Color::GrayLight);
    uiMessageBox->setOutlineColor(Color::Orange);
    uiMessageBox->setOutlineThickness(2);
    renderer->add(uiMessageBox);

    uiProgressBox = new C2DUIGuiProgressBox(this);
    renderer->add(uiProgressBox);

    updateInputMapping(false);
}

void C2DUIGuiMain::init(C2DUIGuiRomList *uiRomList, C2DUIGuiMenu *uiMenu,
                        C2DUIGuiEmu *uiEmu, C2DUIGuiState *uiState) {

    this->uiRomList = uiRomList;
    renderer->add(this->uiRomList);

    // build menus from options
    this->uiMenu = uiMenu;
    renderer->add(this->uiMenu);

    this->uiEmu = uiEmu;
    renderer->add(this->uiEmu);

    this->uiState = uiState;
    renderer->add(this->uiState);
}

C2DUIGuiMain::~C2DUIGuiMain() {
    // ui elements (C2DObject)
    // are deleted by the renderer
}

void C2DUIGuiMain::run() {

    printf("C2DUIGuiMain::run()\n");

    int action = 0;
    unsigned int key = 0;

    while (true) {

        if (uiMenu->getVisibility() == C2DObject::Visible) {
            action = uiMenu->update();
        } else if (uiState->getVisibility() == C2DObject::Visible) {
            action = uiState->update();
        } else if (uiEmu->getVisibility() == C2DObject::Visible) {
            action = uiEmu->update();
        } else {
            action = uiRomList->update();
        }

        key = getInput()->players[0].state;

        switch (action) {

            case UI_KEY_RUN_ROM:
                getInput()->clear(0);
                runRom(uiRomList->getSelection());
                break;

            case UI_KEY_RESUME_ROM:
                getInput()->clear(0);
                uiEmu->resume();
                break;

            case UI_KEY_STOP_ROM:
                getInput()->clear(0);
                uiEmu->stop();
                uiRomList->setVisibility(C2DObject::Visible);
                break;

            case UI_KEY_SHOW_MEMU_UI:
                getInput()->clear(0);
                uiMenu->load();
                break;

            case UI_KEY_SHOW_MEMU_ROM:
                getInput()->clear(0);
                getConfig()->load(uiRomList->getSelection());
                uiMenu->load(true);
                break;

            case UI_KEY_SHOW_MEMU_STATE:
                getInput()->clear(0);
                uiState->load();
                break;

            case UI_KEY_FILTER_ROMS:
                uiRomList->updateRomList();
                break;

            case UI_KEY_SHOW_ROMLIST:
                getInput()->clear(0);
                uiMenu->setVisibility(C2DObject::Hidden);
                uiRomList->setVisibility(C2DObject::Visible);
                break;

            case EV_QUIT:
                return;

            default:
                break;
        }

        if (uiEmu->isPaused()) {
            if (key > 0) {
                if (timer_input.getElapsedTime().asSeconds() > 12) {
                    getRenderer()->delay(INPUT_DELAY / 8);
                } else if (timer_input.getElapsedTime().asSeconds() > 6) {
                    getRenderer()->delay(INPUT_DELAY / 5);
                } else if (timer_input.getElapsedTime().asSeconds() > 2) {
                    getRenderer()->delay(INPUT_DELAY / 2);
                } else {
                    getRenderer()->delay(INPUT_DELAY);
                }
            } else {
                timer_input.restart();
            }
        }
    }
}

void C2DUIGuiMain::runRom(C2DUIRomList::Rom *rom) {

    if (!rom) {
        return;
    }

    // load rom settings
    printf("C2DUIGuiMain::runRom: config->load(%s)\n", rom->drv_name);
    getConfig()->load(rom);

    printf("C2DUIGuiMain::runRom: uiEmu->run(%s)\n", rom->name);
    getUiEmu()->run(rom);
}

float C2DUIGuiMain::getScaling() {
    return scaling;
}

Input *C2DUIGuiMain::getInput() {
    return input;
}

Renderer *C2DUIGuiMain::getRenderer() {
    return renderer;
}

C2DUISkin *C2DUIGuiMain::getSkin() {
    return skin;
}

C2DUIConfig *C2DUIGuiMain::getConfig() {
    return config;
}

Io *C2DUIGuiMain::getIo() {
    return io;
}

C2DUIGuiRomList *C2DUIGuiMain::getUiRomList() {
    return uiRomList;
}

C2DUIGuiEmu *C2DUIGuiMain::getUiEmu() {
    return uiEmu;
}

C2DUIGuiMenu *C2DUIGuiMain::getUiMenu() {
    return uiMenu;
}


C2DUIGuiProgressBox *C2DUIGuiMain::getUiProgressBox() {
    return uiProgressBox;
}

MessageBox *C2DUIGuiMain::getUiMessageBox() {
    return uiMessageBox;
}

Font *C2DUIGuiMain::getFont() {
    return skin->font;
}

int C2DUIGuiMain::getFontSize() {
    return config->getValue(C2DUIOption::Index::SKIN_FONT_SIZE);
}

void C2DUIGuiMain::updateInputMapping(bool isRomConfig) {

    if (isRomConfig) {
        input->setKeyboardMapping(config->getRomPlayerInputKeys(0));
        int dz = 2000 + config->getValue(C2DUIOption::Index::JOY_DEADZONE, true) * 2000;
        for (int i = 0; i < PLAYER_COUNT; i++) {
            input->setJoystickMapping(i, config->getRomPlayerInputButtons(i), dz);
            input->players[i].lx.id = config->getValue(C2DUIOption::Index::JOY_AXIS_LX, true);
            input->players[i].ly.id = config->getValue(C2DUIOption::Index::JOY_AXIS_LY, true);
            input->players[i].rx.id = config->getValue(C2DUIOption::Index::JOY_AXIS_RX, true);
            input->players[i].ry.id = config->getValue(C2DUIOption::Index::JOY_AXIS_RY, true);
        }
    } else {
        input->setKeyboardMapping(config->getGuiPlayerInputKeys(0));
        int dz = 2000 + config->getValue(C2DUIOption::Index::JOY_DEADZONE) * 2000;
        for (int i = 0; i < PLAYER_COUNT; i++) {
            input->setJoystickMapping(i, config->getGuiPlayerInputButtons(i), dz);
            input->players[i].lx.id = config->getValue(C2DUIOption::Index::JOY_AXIS_LX);
            input->players[i].ly.id = config->getValue(C2DUIOption::Index::JOY_AXIS_LY);
            input->players[i].rx.id = config->getValue(C2DUIOption::Index::JOY_AXIS_RX);
            input->players[i].ry.id = config->getValue(C2DUIOption::Index::JOY_AXIS_RY);
        }
    }

#ifdef __SWITCH__
    int single_joy = config->getValue(C2DUIOption::Index::JOY_SINGLEJOYCON);
    for (int i = 0; i < PLAYER_COUNT; i++) {
        if (single_joy > 0) {
            hidSetNpadJoyAssignmentModeSingleByDefault((HidControllerID) i);
        } else {
            hidSetNpadJoyAssignmentModeDual((HidControllerID) i);
        }
    }
#endif
}
