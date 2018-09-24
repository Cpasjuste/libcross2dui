//
// Created by cpasjuste on 16/02/18.
//

#include "c2dui.h"

#define STATES_COUNT 4

using namespace c2d;
using namespace c2dui;

#ifdef __PFBA__

extern int BurnStateLoad(char *szName, int bAll, int (*pLoadGame)());

extern int MakeScreenShot(const char *dest);

extern int BurnStateSave(char *szName, int bAll);

extern int DrvInitCallback();

#elif __PSNES__

extern unsigned char S9xFreezeGame(const char *path);

extern unsigned char S9xUnfreezeGame(const char *path);

#endif

class GUISaveState : public Rectangle {

public:

    GUISaveState(C2DUIGuiMain *ui, const FloatRect &rect, int id) : Rectangle(rect) {

        this->ui = ui;
        this->id = id;

        setFillColor(Color::GrayLight);
        setOutlineColor(COL_GREEN);
        setOutlineThickness(2);

        // text if no save/screenshot
        middle_text = new C2DText("NO SAVE", *ui->getFont(), (unsigned int) ui->getFontSize());
        middle_text->setOutlineThickness(2);
        middle_text->setOriginCenter();
        middle_text->setPosition(getLocalBounds().left + getSize().x / 2,
                                 getLocalBounds().top + getSize().y / 2);
        add(middle_text);

        // text for slot id
        snprintf(bottom_text_char, 32, "SLOT %i", id);
        bottom_text = new C2DText(bottom_text_char, *ui->getFont(), (unsigned int) ui->getFontSize());
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
                    texture->setOriginCenter();
                    add(texture);
                    middle_text->setVisibility(Hidden);
                }
            }
            if (!texture || !texture->available) {
                middle_text->setString("NO PIC");
                middle_text->setOriginCenter();
                middle_text->setVisibility(Visible);
            }
        } else {
            middle_text->setString("NO SAVE");
            middle_text->setOriginCenter();
            middle_text->setVisibility(Visible);
        }
    }

    void setRom(C2DUIRomList::Rom *rom) {

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
#ifdef __PFBA__
        BurnStateLoad(path, 1, &DrvInitCallback);
#elif __PSNES__
        S9xUnfreezeGame(path);
#endif
    }

    void save() {
        printf("StateSave: %s\n", path);
#ifdef __PFBA__
        BurnStateSave(path, 1);
        MakeScreenShot(shot);
#elif __PSNES__
        S9xFreezeGame(path);
        ui->getUiEmu()->getVideo()->save(shot);
#endif
        loadTexture();
    }

    C2DUIGuiMain *ui;
    Texture *texture = nullptr;
    Text *middle_text = nullptr;
    Text *bottom_text = nullptr;
    char path[1024];
    char shot[1024];
    char bottom_text_char[32];
    bool exist = false;
    int id = 0;
};

class C2DUIGuiSaveStateList : public Rectangle {

public:

    C2DUIGuiSaveStateList(C2DUIGuiMain *ui, const FloatRect &rect) : Rectangle(rect) {

        setFillColor(Color::Transparent);

        // add states items
        float width = getSize().x / STATES_COUNT;
        for (int i = 0; i < STATES_COUNT; i++) {
            FloatRect r = {(width * i) + (width / 2), width / 2, width, width};
            states[i] = new GUISaveState(ui, r, i);
            states[i]->setOriginCenter();
            add(states[i]);
        }

        setSelection(0);
    }

    ~C2DUIGuiSaveStateList() {
        for (auto &state : states) {
            delete state;
        }
    }

    GUISaveState *getSelection() {
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

    GUISaveState *states[STATES_COUNT];
    int index = 0;
};

C2DUIGuiState::C2DUIGuiState(C2DUIGuiMain *u) : Rectangle(Vector2f(0, 0)) {

    printf("C2DUIGuiState()\n");

    this->ui = u;

    setFillColor({55, 55, 55, 180});
    setOutlineColor(COL_ORANGE);
    setOutlineThickness(4);
    setPosition(UI_MARGIN * ui->getScaling(), UI_MARGIN * ui->getScaling());
    setSize(ui->getRenderer()->getSize().x - (UI_MARGIN * ui->getScaling() * 2),
            ui->getRenderer()->getSize().y - (UI_MARGIN * ui->getScaling() * 2));


    // menu title
    title = new Text("TITLE_______________________", *ui->getSkin()->font);
    title->setSizeMax(Vector2f(getSize().x - 16, 0));
    title->setFillColor(Color::White);
    title->setOutlineThickness(2);
    title->setOutlineColor(COL_RED);
    title->setStyle((Uint32) c2d::Text::Underlined);
    title->setPosition(20 * ui->getScaling(), 20 * ui->getScaling());
    int start_y = (int) (title->getGlobalBounds().top + title->getGlobalBounds().height + 16 * ui->getScaling());
    add(title);


    uiStateList = new C2DUIGuiSaveStateList(ui, {
            getLocalBounds().left + getSize().x / 2, (float) start_y + 32,
            getSize().x - 64, getSize().x / (STATES_COUNT + 1)
    });
    uiStateList->setOrigin(uiStateList->getSize().x / 2, 0);
    add(uiStateList);

    setVisibility(Hidden);
}

void C2DUIGuiState::load() {

    isEmuRunning = ui->getUiEmu()->getVisibility() == Visible;
    // should always be the case...
    if (isEmuRunning) {
        // if frameskip is enabled, we may get a black buffer,
        // force a frame to be drawn
        ui->getUiEmu()->updateFb();
    }

    char name[128];
    snprintf(name, 128, "%s__________", ui->getUiRomList()->getSelection()->name);
    title->setString(name);

    for (auto &state : uiStateList->states) {
        state->setRom(ui->getUiRomList()->getSelection());
    }
    uiStateList->setSelection(0);

    setVisibility(Visible);
}

void C2DUIGuiState::unload() {

    setVisibility(Hidden);
}

int C2DUIGuiState::update() {

    int ret = 0;
    unsigned int key = ui->getInput()->update()[0].state;

    if (key > 0) {

        if (key & Input::Key::KEY_LEFT) {
            uiStateList->left();
        } else if (key & Input::Key::KEY_RIGHT) {
            uiStateList->right();
        }

        // FIRE1
        if (key & Input::Key::KEY_FIRE1) {
            if (isEmuRunning) {
                GUISaveState *state = uiStateList->getSelection();
                if (state->exist) {
                    int res = ui->getUiMessageBox()->show(
                            state->bottom_text->getString(),
                            "PRESS FIRE2 TO CANCEL", "LOAD", "SAVE");
                    if (res == MessageBox::LEFT) {
                        state->load();
                        unload();
                        ret = UI_KEY_RESUME_ROM;
                    } else if (res == MessageBox::RIGHT) {
                        state->save();
                        unload();
                        ret = UI_KEY_RESUME_ROM;
                    }
                } else {
                    state->save();
                    unload();
                    ret = UI_KEY_RESUME_ROM;
                }
            }
        }

        // FIRE2
        if (key & Input::Key::KEY_FIRE2) {
            unload();
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
