//
// Created by cpasjuste on 14/01/18.
//

#ifndef GUI_H
#define GUI_H

#define UI_KEY_FILTER_ROMS      100
#define UI_KEY_SHOW_ROMLIST     101
#define UI_KEY_SHOW_MEMU_UI     102
#define UI_KEY_SHOW_MEMU_ROM    103
#define UI_KEY_SHOW_MEMU_STATE  104
#define UI_KEY_RUN_ROM          105
#define UI_KEY_RESUME_ROM       106
#define UI_KEY_STOP_ROM         107

#define UI_MARGIN   16
#define INPUT_DELAY 500

#define COL_GREEN   Color(153, 255, 51)
#define COL_YELLOW  Color(255, 225, 51)
#define COL_ORANGE  Color(255, 153, 51)
#define COL_RED     Color(255, 51, 51)

namespace c2dui {

    class UIMain : public c2d::C2DRenderer {

    public:

        UIMain(const c2d::Vector2f &size);

        virtual ~UIMain();

        void init(UIRomList *uiRomList, UIMenu *uiMenu,
                  UIEmu *uiEmu, UIStateMenu *uiState);

        void setConfig(Config *cfg);

        void setSkin(Skin *skin);

        bool onInput(c2d::Input::Player *players) override;

        void onDraw(c2d::Transform &transform) override;

        void updateInputMapping(bool isRomCfg);

        c2dui::Skin *getSkin();

        Config *getConfig();

        UIRomList *getUiRomList();

        UIEmu *getUiEmu();

        UIMenu *getUiMenu();

        UIStateMenu *getUiStateMenu();

        UIProgressBox *getUiProgressBox();

        c2d::MessageBox *getUiMessageBox();

        c2d::Font *getFont() override;

        int getFontSize();

        float getScaling();

        bool done = false;

    private:

        c2dui::Config *config = nullptr;
        c2dui::Skin *skin = nullptr;
        UIMenu *uiMenu = nullptr;
        UIRomList *uiRomList = nullptr;
        UIEmu *uiEmu = nullptr;
        UIStateMenu *uiState = nullptr;
        UIProgressBox *uiProgressBox = nullptr;
        c2d::MessageBox *uiMessageBox = nullptr;
        c2d::C2DClock timer;

        float scaling = 1;

    };
}

#endif //GUI_MAIN_H
