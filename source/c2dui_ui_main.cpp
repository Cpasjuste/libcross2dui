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

UIMain::UIMain(const Vector2f &size) : C2DRenderer({size.x, size.y}) {

    /*
    renderer = r;
    skin = s;
    config = cfg;

    // scaling factor mainly used for borders,
    // based on switch resolution..
    scaling = std::min(renderer->getSize().x / 1280, 1.0f);
    //printf("scaling: %f\n", scaling);

    uiMessageBox = new MessageBox(
            FloatRect(
                    renderer->getSize().x / 2,
                    renderer->getSize().y / 2,
                    renderer->getSize().x / 2,
                    renderer->getSize().y / 2),
            getInput(), skin->font, getFontSize());
    uiMessageBox->setOrigin(Origin::Center);
    uiMessageBox->setFillColor(Color::Gray);
    uiMessageBox->setOutlineColor(Color::Orange);
    uiMessageBox->setOutlineThickness(2);
    renderer->add(uiMessageBox);

    uiProgressBox = new UIProgressBox(this);
    renderer->add(uiProgressBox);

    updateInputMapping(false);
    getInput()->setRepeatEnable(true);
    getInput()->setRepeatDelay(INPUT_DELAY);
    */
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

void UIMain::onDraw(c2d::Transform &transform) {

    if (uiEmu && !uiEmu->isVisible()) {
        unsigned int keys = getInput()->getKeys(0);
        if (keys != Input::Key::Delay) {
            if (keys && timer.getElapsedTime().asSeconds() > 4) {
                getInput()->setRepeatDelay(INPUT_DELAY / 20);
            } else if (keys && timer.getElapsedTime().asSeconds() > 2) {
                getInput()->setRepeatDelay(INPUT_DELAY / 10);
            } else if (keys && timer.getElapsedTime().asMilliseconds() > INPUT_DELAY) {
                getInput()->setRepeatDelay(INPUT_DELAY / 5);
            } else if (!keys) {
                getInput()->setRepeatDelay(INPUT_DELAY);
                timer.restart();
            }
        }
    }

    Renderer::onDraw(transform);
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
    return config->getValue(Option::Index::SKIN_FONT_SIZE);
}

void UIMain::updateInputMapping(bool isRomConfig) {

    if (isRomConfig) {
        getInput()->setKeyboardMapping(config->getRomPlayerInputKeys(0));
        int dz = 2000 + config->getValue(Option::Index::JOY_DEADZONE, true) * 2000;
        for (int i = 0; i < PLAYER_MAX; i++) {
            getInput()->setJoystickMapping(i, config->getRomPlayerInputButtons(i), dz);
            getInput()->players[i].lx.id = config->getValue(Option::Index::JOY_AXIS_LX, true);
            getInput()->players[i].ly.id = config->getValue(Option::Index::JOY_AXIS_LY, true);
            getInput()->players[i].rx.id = config->getValue(Option::Index::JOY_AXIS_RX, true);
            getInput()->players[i].ry.id = config->getValue(Option::Index::JOY_AXIS_RY, true);
        }
    } else {
        getInput()->setKeyboardMapping(config->getGuiPlayerInputKeys(0));
        int dz = 2000 + config->getValue(Option::Index::JOY_DEADZONE) * 2000;
        for (int i = 0; i < PLAYER_MAX; i++) {
            getInput()->setJoystickMapping(i, config->getGuiPlayerInputButtons(i), dz);
            getInput()->players[i].lx.id = config->getValue(Option::Index::JOY_AXIS_LX);
            getInput()->players[i].ly.id = config->getValue(Option::Index::JOY_AXIS_LY);
            getInput()->players[i].rx.id = config->getValue(Option::Index::JOY_AXIS_RX);
            getInput()->players[i].ry.id = config->getValue(Option::Index::JOY_AXIS_RY);
        }
    }

#ifdef __SWITCH__
    int single_joy = config->getValue(Option::Index::JOY_SINGLEJOYCON);
    for (int i = 0; i < PLAYER_MAX; i++) {
        if (single_joy > 0) {
            hidSetNpadJoyAssignmentModeSingleByDefault((HidControllerID) i);
        } else {
            hidSetNpadJoyAssignmentModeDual((HidControllerID) i);
        }
    }
#endif
}
