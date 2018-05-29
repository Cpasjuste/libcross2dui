//
// Created by cpasjuste on 22/11/16.
//

#include <algorithm>

#include "c2dui.h"

#include "gui_menu.h"
#include "gui_emu.h"
#include "gui_state.h"
#include "gui_romlist.h"
#include "gui_progressbox.h"

using namespace c2d;
using namespace c2dui;

C2DUIGuiMain::C2DUIGuiMain(Io *i, Renderer *r, C2DUISkin *s, C2DUIConfig *cfg, Input *in, Audio *aud) {

    io = i;
    renderer = r;
    skin = s;
    config = cfg;
    input = in;
    audio = aud;

    // scaling factor mainly used for borders,
    // based on vita resolution..
    scaling = std::min(renderer->getSize().x / 960, 1.0f);

    uiRomList = new GuiRomList(this, renderer->getSize());
    renderer->add(uiRomList);

    // build menus from options
    uiMenu = new GuiMenu(this);
    renderer->add(uiMenu);

    uiEmu = new GuiEmu(this);
    renderer->add(uiEmu);

    uiState = new GuiState(this);
    renderer->add(uiState);

    uiProgressBox = new GuiProgressBox(this);
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

GuiRomList *C2DUIGuiMain::getUiRomList() {
    return uiRomList;
}

GuiEmu *C2DUIGuiMain::getUiEmu() {
    return uiEmu;
}

GuiMenu *C2DUIGuiMain::getUiMenu() {
    return uiMenu;
}


GuiProgressBox *C2DUIGuiMain::getUiProgressBox() {
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
