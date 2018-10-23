//
// Created by cpasjuste on 16/02/18.
//

#include "c2dui.h"

#define STATES_COUNT 4

using namespace c2d;
using namespace c2dui;

class UIState : public RectangleShape {

public:

    UIState(UIMain *ui, const FloatRect &rect, int id) : RectangleShape(rect) {

        this->ui = ui;
        this->id = id;

        setFillColor(Color::Gray);
        setOutlineColor(COL_GREEN);
        setOutlineThickness(2);

        // text if no save/screenshot
        middle_text = new C2DText("NO SAVE", (unsigned int) ui->getFontSize(), ui->getFont());
        middle_text->setOutlineThickness(2);
        middle_text->setOrigin(Origin::Center);
        middle_text->setPosition(getLocalBounds().left + getSize().x / 2,
                                 getLocalBounds().top + getSize().y / 2);
        add(middle_text);

        // text for slot id
        snprintf(bottom_text_char, 32, "SLOT %i", id);
        bottom_text = new C2DText(bottom_text_char, (unsigned int) ui->getFontSize(), ui->getFont());
        bottom_text->setOutlineThickness(2);
        bottom_text->setOrigin(bottom_text->getLocalBounds().width / 2, 0);
        bottom_text->setPosition(getLocalBounds().left + getSize().x / 2,
                                 getLocalBounds().top + getLocalBounds().width + 8);
        add(bottom_text);
    }

    void loadTexture() {

        if (texture) {
            delete (texture);
            texture = nullptr;
        }

        exist = ui->getIo()->exist(path);
        if (exist) {
            if (ui->getIo()->exist(shot)) {
                texture = new C2DTexture(shot);
                if (texture->available) {
                    float tex_scaling = std::min(
                            getSize().x / texture->getTextureRect().width,
                            getSize().y / texture->getTextureRect().height);
                    texture->setScale(tex_scaling, tex_scaling);
                    texture->setPosition(getSize().x / 2, getSize().y / 2);
                    texture->setOrigin(Origin::Center);
                    add(texture);
                    middle_text->setVisibility(Visibility::Hidden);
                }
            }
            if (!texture || !texture->available) {
                middle_text->setString("NO PIC");
                middle_text->setOrigin(Origin::Center);
                middle_text->setVisibility(Visibility::Visible);
            }
        } else {
            middle_text->setString("NO SAVE");
            middle_text->setOrigin(Origin::Center);
            middle_text->setVisibility(Visibility::Visible);
        }
    }

    void setRom(RomList::Rom *rom) {

        memset(path, 0, MAX_PATH);
        memset(shot, 0, MAX_PATH);
        snprintf(path, 1023, "%ssaves/%s%i.sav",
                 ui->getConfig()->getHomePath()->c_str(), rom->drv_name, id);
        snprintf(shot, 1023, "%ssaves/%s%i.png",
                 ui->getConfig()->getHomePath()->c_str(), rom->drv_name, id);

        loadTexture();
    }

    void load() {
        printf("StateLoad: %s\n", path);
        ui->getUiStateMenu()->loadStateCore(path);
    }

    void save() {
        printf("StateSave: %s\n", path);
        if (ui->getUiStateMenu()->saveStateCore(path)) {
            ui->getUiEmu()->getVideo()->save(shot);
            loadTexture();
        }
    }

    UIMain *ui;
    Texture *texture = nullptr;
    Text *middle_text = nullptr;
    Text *bottom_text = nullptr;
    char path[1024];
    char shot[1024];
    char bottom_text_char[32];
    bool exist = false;
    int id = 0;
};

class UIStateList : public RectangleShape {

public:

    UIStateList(UIMain *ui, const FloatRect &rect) : RectangleShape(rect) {

        setFillColor(Color::Transparent);

        // add states items
        float width = getSize().x / STATES_COUNT;
        for (int i = 0; i < STATES_COUNT; i++) {
            FloatRect r = {(width * i) + (width / 2), width / 2, width, width};
            states[i] = new UIState(ui, r, i);
            states[i]->setOrigin(Origin::Center);
            add(states[i]);
        }

        setSelection(0);
    }

    ~UIStateList() {
        for (auto &state : states) {
            delete state;
        }
    }

    UIState *getSelection() {
        return states[index];
    }

    void setSelection(int idx) {

        if (idx < 0 || idx > STATES_COUNT) {
            return;
        }

        index = idx;
        for (int i = 0; i < STATES_COUNT; i++) {
            states[i]->setOutlineColor(i == index ? COL_YELLOW : COL_GREEN);
            states[i]->setOutlineThickness(i == index ? 4 : 1);
            states[i]->setLayer(i == index ? 1 : 0);
            float scale = i == index ? 1.0f : 0.95f;
            states[i]->setScale(scale, scale);
        }
    }

    void left() {
        index--;
        if (index < 0) {
            index = STATES_COUNT - 1;
        }
        setSelection(index);
    }

    void right() {
        index++;
        if (index > STATES_COUNT - 1) {
            index = 0;
        }
        setSelection(index);
    }

    UIState *states[STATES_COUNT];
    int index = 0;
};

UIStateMenu::UIStateMenu(UIMain *u) : RectangleShape(Vector2f(0, 0)) {

    printf("UIStateMenu()\n");

    this->ui = u;

    setFillColor({55, 55, 55, 180});
    setOutlineColor(COL_ORANGE);
    setOutlineThickness(4);
    setPosition(UI_MARGIN * ui->getScaling(), UI_MARGIN * ui->getScaling());
    setSize(ui->getRenderer()->getSize().x - (UI_MARGIN * ui->getScaling() * 2),
            ui->getRenderer()->getSize().y - (UI_MARGIN * ui->getScaling() * 2));


    // menu title
    title = new Text("TITLE_______________________", C2D_DEFAULT_CHAR_SIZE, ui->getSkin()->font);
    title->setWidth(getSize().x - 16);
    title->setFillColor(Color::White);
    title->setOutlineThickness(2);
    title->setOutlineColor(COL_RED);
    title->setStyle((Uint32) c2d::Text::Underlined);
    title->setPosition(20 * ui->getScaling(), 20 * ui->getScaling());
    int start_y = (int) (title->getGlobalBounds().top + title->getGlobalBounds().height + 16 * ui->getScaling());
    add(title);


    uiStateList = new UIStateList(ui, {
            getLocalBounds().left + getSize().x / 2, (float) start_y + 32,
            getSize().x - 64, getSize().x / (STATES_COUNT + 1)
    });
    uiStateList->setOrigin(uiStateList->getSize().x / 2, 0);
    add(uiStateList);

    setVisibility(Visibility::Hidden);
}

void UIStateMenu::show() {

    isEmuRunning = ui->getUiEmu()->isVisible();
    // should always be the case...
    if (isEmuRunning) {
        // if frameskip is enabled, we may get a black buffer,
        // force a frame to be drawn
        ui->getUiEmu()->updateFb();
    }

    char name[128];
    snprintf(name, 128, "%s__________", ui->getUiRomList()->getSelection()->name.c_str());
    title->setString(name);

    for (auto &state : uiStateList->states) {
        state->setRom(ui->getUiRomList()->getSelection());
    }
    uiStateList->setSelection(0);

    setVisibility(Visibility::Visible);
}

void UIStateMenu::hide() {

    setVisibility(Visibility::Hidden);
}

int UIStateMenu::loop() {

    int ret = 0;
    unsigned int key = ui->getInput()->getKeys();

    if (key > 0) {

        if (key & Input::Key::KEY_LEFT) {
            uiStateList->left();
        } else if (key & Input::Key::KEY_RIGHT) {
            uiStateList->right();
        }

        // FIRE1
        if (key & Input::Key::KEY_FIRE1) {
            if (isEmuRunning) {
                UIState *state = uiStateList->getSelection();
                if (state->exist) {
                    int res = ui->getUiMessageBox()->show(
                            state->bottom_text->getString(),
                            "PRESS FIRE2 TO CANCEL", "LOAD", "SAVE");
                    if (res == MessageBox::LEFT) {
                        state->load();
                        hide();
                        ret = UI_KEY_RESUME_ROM;
                    } else if (res == MessageBox::RIGHT) {
                        state->save();
                        hide();
                        ret = UI_KEY_RESUME_ROM;
                    }
                } else {
                    state->save();
                    hide();
                    ret = UI_KEY_RESUME_ROM;
                }
            }
        }

        // FIRE2
        if (key & Input::Key::KEY_FIRE2) {
            hide();
            if (isEmuRunning) {
                ret = UI_KEY_RESUME_ROM;
            } else {
                ret = UI_KEY_SHOW_ROMLIST;
            }
        }

        // QUIT
        if (key & EV_QUIT) {
            return EV_QUIT;
        }
    }

    ui->getRenderer()->flip();

    return ret;
}
