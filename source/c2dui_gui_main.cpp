//
// Created by cpasjuste on 22/11/16.
//

#include <algorithm>

#include "c2dui.h"

using namespace c2d;
using namespace c2dui;

C2DUIGuiMain::C2DUIGuiMain(Renderer *r, Io *i, Input *in, Audio *a,
                           C2DUIConfig *cfg, C2DUISkin *s) {

    io = i;
    renderer = r;
    skin = s;
    config = cfg;
    input = in;
    audio = a;

    // scaling factor mainly used for borders,
    // based on vita resolution..
    scaling = std::min(renderer->getSize().x / 960, 1.0f);

    uiRomList = new C2DUIGuiRomList(this, renderer->getSize());
    renderer->add(uiRomList);

    // build menus from options
    uiMenu = new C2DUIGuiMenu(this);
    renderer->add(uiMenu);

    uiEmu = new C2DUIGuiEmu(this);
    renderer->add(uiEmu);

    uiState = new C2DUIGuiState(this);
    renderer->add(uiState);

    uiProgressBox = new C2DUIGuiProgressBox(this);
    renderer->add(uiProgressBox);

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

    updateInputMapping(false);
}

C2DUIGuiMain::~C2DUIGuiMain() {
    // ui elements (C2DObject)
    // are deleted by the renderer
}

void C2DUIGuiMain::run() {

    int key = 0;

    while (true) {

        if (uiMenu->getVisibility() == C2DObject::Visible) {
            key = uiMenu->update();
        } else if (uiState->getVisibility() == C2DObject::Visible) {
            key = uiState->update();
        } else if (uiEmu->getVisibility() == C2DObject::Visible) {
            key = uiEmu->update();
        } else {
            key = uiRomList->update();
        }

        switch (key) {

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
    }
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

c2d::Audio *C2DUIGuiMain::getAudio() {
    return audio;
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
}
