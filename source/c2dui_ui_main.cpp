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

UIMain::UIMain(const Vector2f &size) : C2DRenderer(size) {
}

UIMain::~UIMain() {
    // ui elements (C2DObject)
    // are deleted by the renderer
}

void UIMain::init(UIRomList *uiRomList, UIMenu *uiMenu,
                  UIEmu *uiEmu, UIStateMenu *uiState) {

    this->uiRomList = uiRomList;
    add(this->uiRomList);

    // build menus from options
    this->uiMenu = uiMenu;
    add(this->uiMenu);

    this->uiEmu = uiEmu;
    add(this->uiEmu);

    this->uiState = uiState;
    add(this->uiState);

    // scaling factor mainly used for borders,
    // based on switch resolution..
    scaling = std::min(getSize().x / 1280, 1.0f);
    //printf("scaling: %f\n", scaling);

    uiMessageBox = new MessageBox(
            FloatRect(
                    getSize().x / 2,
                    getSize().y / 2,
                    getSize().x / 2,
                    getSize().y / 2),
            getInput(), skin->font, getFontSize());
    uiMessageBox->setOrigin(Origin::Center);
    uiMessageBox->setFillColor(Color::Gray);
    uiMessageBox->setOutlineColor(Color::Orange);
    uiMessageBox->setOutlineThickness(2);
    add(uiMessageBox);

    uiProgressBox = new UIProgressBox(this);
    add(uiProgressBox);

    updateInputMapping(false);
    getInput()->setRepeatDelay(INPUT_DELAY);

}

void UIMain::setConfig(Config *cfg) {
    config = cfg;
}

void UIMain::setSkin(Skin *s) {
    skin = s;
}

bool UIMain::onInput(c2d::Input::Player *players) {
    return Renderer::onInput(players);
}

void UIMain::onDraw(c2d::Transform &transform, bool draw) {

    if (uiEmu && !uiEmu->isVisible()) {
        unsigned int keys = getInput()->getKeys(0);
        if (keys != Input::Key::Delay) {
            if (keys && timer.getElapsedTime().asSeconds() > 6) {
                getInput()->setRepeatDelay(INPUT_DELAY / 8);
            } else if (keys && timer.getElapsedTime().asSeconds() > 4) {
                getInput()->setRepeatDelay(INPUT_DELAY / 4);
            } else if (keys && timer.getElapsedTime().asMilliseconds() > 2) {
                getInput()->setRepeatDelay(INPUT_DELAY / 2);
            } else if (!keys) {
                getInput()->setRepeatDelay(INPUT_DELAY);
                timer.restart();
            }
        }
    }

    Renderer::onDraw(transform, draw);
}

float UIMain::getScaling() {
    return scaling;
}

Skin *UIMain::getSkin() {
    return skin;
}

Config *UIMain::getConfig() {
    return config;
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
    return config->get(Option::Index::SKIN_FONT_SIZE)->getValueInt();
}

void UIMain::updateInputMapping(bool isRomConfig) {

    if (isRomConfig) {
        getInput()->setKeyboardMapping(config->getRomPlayerInputKeys(0));
        int dz = config->get(Option::Index::JOY_DEADZONE, true)->getValueInt();
        for (int i = 0; i < PLAYER_MAX; i++) {
            getInput()->setJoystickMapping(i, config->getRomPlayerInputButtons(i), dz);
            getInput()->players[i].lx.id = config->get(Option::Index::JOY_AXIS_LX, true)->getValueInt();
            getInput()->players[i].ly.id = config->get(Option::Index::JOY_AXIS_LY, true)->getValueInt();
            getInput()->players[i].rx.id = config->get(Option::Index::JOY_AXIS_RX, true)->getValueInt();
            getInput()->players[i].ry.id = config->get(Option::Index::JOY_AXIS_RY, true)->getValueInt();
        }
    } else {
        getInput()->setKeyboardMapping(config->getGuiPlayerInputKeys(0));
        int dz = config->get(Option::Index::JOY_DEADZONE)->getValueInt();
        for (int i = 0; i < PLAYER_MAX; i++) {
            getInput()->setJoystickMapping(i, config->getGuiPlayerInputButtons(i), dz);
            getInput()->players[i].lx.id = config->get(Option::Index::JOY_AXIS_LX)->getValueInt();
            getInput()->players[i].ly.id = config->get(Option::Index::JOY_AXIS_LY)->getValueInt();
            getInput()->players[i].rx.id = config->get(Option::Index::JOY_AXIS_RX)->getValueInt();
            getInput()->players[i].ry.id = config->get(Option::Index::JOY_AXIS_RY)->getValueInt();
        }
    }

#ifdef __SWITCH__
    int single_joy = config->get(Option::Index::JOY_SINGLEJOYCON);
    for (int i = 0; i < PLAYER_MAX; i++) {
        if (single_joy > 0) {
            hidSetNpadJoyAssignmentModeSingleByDefault((HidControllerID) i);
        } else {
            hidSetNpadJoyAssignmentModeDual((HidControllerID) i);
        }
    }
#endif
}
