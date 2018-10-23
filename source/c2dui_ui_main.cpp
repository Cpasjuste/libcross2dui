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

UIMain::UIMain(Renderer *r, Io *i, Input *in, Config *cfg, Skin *s) {

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
            input, skin->font, getFontSize());
    uiMessageBox->setOrigin(Origin::Center);
    uiMessageBox->setFillColor(Color::Gray);
    uiMessageBox->setOutlineColor(Color::Orange);
    uiMessageBox->setOutlineThickness(2);
    renderer->add(uiMessageBox);

    uiProgressBox = new UIProgressBox(this);
    renderer->add(uiProgressBox);

    updateInputMapping(false);
    input->setRepeatEnable(true);
    input->setRepeatDelay(INPUT_DELAY);
}

void UIMain::init(UIRomList *uiRomList, UIMenu *uiMenu,
                  UIEmu *uiEmu, UIStateMenu *uiState) {

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

UIMain::~UIMain() {
    // ui elements (C2DObject)
    // are deleted by the renderer
}

void UIMain::run() {

    printf("C2DUIGuiMain::run()\n");

    int action = 0;
    unsigned int key = 0;

    while (true) {

        if (uiMenu->isVisible()) {
            action = uiMenu->loop();
        } else if (uiState->isVisible()) {
            action = uiState->loop();
        } else if (uiEmu->isVisible()) {
            action = uiEmu->loop();
        } else {
            action = uiRomList->loop();
        }

        key = getInput()->getKeys();

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
                uiRomList->setVisibility(Visibility::Visible);
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
                uiState->show();
                break;

            case UI_KEY_FILTER_ROMS:
                uiRomList->updateRomList();
                break;

            case UI_KEY_SHOW_ROMLIST:
                getInput()->clear(0);
                uiMenu->setVisibility(Visibility::Hidden);
                uiRomList->setVisibility(Visibility::Visible);
                break;

            case EV_QUIT:
                return;

            default:
                break;
        }

        if (uiEmu->isPaused() || !uiEmu->isVisible()) {
            if (key > 0) {
                if (timer_input.getElapsedTime().asSeconds() > 6) {
                    input->setRepeatDelay(INPUT_DELAY / 8);
                } else if (timer_input.getElapsedTime().asSeconds() > 4) {
                    input->setRepeatDelay(INPUT_DELAY / 5);
                } else if (timer_input.getElapsedTime().asSeconds() > 2) {
                    input->setRepeatDelay(INPUT_DELAY / 2);
                } else {
                    input->setRepeatDelay(INPUT_DELAY);
                }
            } else {
                timer_input.restart();
            }
        }
    }
}

void UIMain::runRom(RomList::Rom *rom) {

    if (!rom) {
        return;
    }

    // load rom settings
    printf("C2DUIGuiMain::runRom: config->load(%s)\n", rom->drv_name);
    getConfig()->load(rom);
    getUiEmu()->run(rom);
}

float UIMain::getScaling() {
    return scaling;
}

Input *UIMain::getInput() {
    return input;
}

Renderer *UIMain::getRenderer() {
    return renderer;
}

Skin *UIMain::getSkin() {
    return skin;
}

Config *UIMain::getConfig() {
    return config;
}

Io *UIMain::getIo() {
    return io;
}

UIRomList *UIMain::getUiRomList() {
    return uiRomList;
}

UIEmu *UIMain::getUiEmu() {
    return uiEmu;
}

UIMenu *UIMain::getUiMenu() {
    return uiMenu;
}

UIStateMenu *UIMain::getUiStateMenu() {
    return uiState;
}

UIProgressBox *UIMain::getUiProgressBox() {
    return uiProgressBox;
}

MessageBox *UIMain::getUiMessageBox() {
    return uiMessageBox;
}

Font *UIMain::getFont() {
    return skin->font;
}

int UIMain::getFontSize() {
    return config->getValue(Option::Index::SKIN_FONT_SIZE);
}

void UIMain::updateInputMapping(bool isRomConfig) {

    if (isRomConfig) {
        input->setKeyboardMapping(config->getRomPlayerInputKeys(0));
        int dz = 2000 + config->getValue(Option::Index::JOY_DEADZONE, true) * 2000;
        for (int i = 0; i < PLAYER_COUNT; i++) {
            input->setJoystickMapping(i, config->getRomPlayerInputButtons(i), dz);
            input->players[i].lx.id = config->getValue(Option::Index::JOY_AXIS_LX, true);
            input->players[i].ly.id = config->getValue(Option::Index::JOY_AXIS_LY, true);
            input->players[i].rx.id = config->getValue(Option::Index::JOY_AXIS_RX, true);
            input->players[i].ry.id = config->getValue(Option::Index::JOY_AXIS_RY, true);
        }
    } else {
        input->setKeyboardMapping(config->getGuiPlayerInputKeys(0));
        int dz = 2000 + config->getValue(Option::Index::JOY_DEADZONE) * 2000;
        for (int i = 0; i < PLAYER_COUNT; i++) {
            input->setJoystickMapping(i, config->getGuiPlayerInputButtons(i), dz);
            input->players[i].lx.id = config->getValue(Option::Index::JOY_AXIS_LX);
            input->players[i].ly.id = config->getValue(Option::Index::JOY_AXIS_LY);
            input->players[i].rx.id = config->getValue(Option::Index::JOY_AXIS_RX);
            input->players[i].ry.id = config->getValue(Option::Index::JOY_AXIS_RY);
        }
    }

#ifdef __SWITCH__
    int single_joy = config->getValue(Option::Index::JOY_SINGLEJOYCON);
    for (int i = 0; i < PLAYER_COUNT; i++) {
        if (single_joy > 0) {
            hidSetNpadJoyAssignmentModeSingleByDefault((HidControllerID) i);
        } else {
            hidSetNpadJoyAssignmentModeDual((HidControllerID) i);
        }
    }
#endif
}
