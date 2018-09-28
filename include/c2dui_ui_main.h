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
#define INPUT_DELAY 150

#define COL_GREEN   Color(153, 255, 51)
#define COL_YELLOW  Color(255, 225, 51)
#define COL_ORANGE  Color(255, 153, 51)
#define COL_RED     Color(255, 51, 51)

namespace c2dui {

    class UIMain {

    public:

        UIMain(c2d::Renderer *renderer, c2d::Io *io, c2d::Input *input,
                     c2dui::Config *config, c2dui::Skin *skin);

        virtual ~UIMain();

        virtual void runRom(RomList::Rom *rom);

        void init(UIRomList *uiRomList, UIMenu *uiMenu,
                  UIEmu *uiEmu, UIStateMenu *uiState);

        void run();

        void updateInputMapping(bool isRomCfg);

        c2d::Renderer *getRenderer();

        c2dui::Skin *getSkin();

        Config *getConfig();

        c2d::Input *getInput();

        c2d::Io *getIo();

        UIRomList *getUiRomList();

        UIEmu *getUiEmu();

        UIMenu *getUiMenu();

        UIProgressBox *getUiProgressBox();

        c2d::MessageBox *getUiMessageBox();

        c2d::Font *getFont();

        int getFontSize();

        float getScaling();

    private:

        c2dui::Config *config = nullptr;
        c2dui::Skin *skin = nullptr;

        c2d::Io *io = nullptr;
        c2d::Renderer *renderer = nullptr;
        c2d::Input *input = nullptr;

        UIMenu *uiMenu = nullptr;
        UIRomList *uiRomList = nullptr;
        UIEmu *uiEmu = nullptr;
        UIStateMenu *uiState = nullptr;
        UIProgressBox *uiProgressBox = nullptr;
        c2d::MessageBox *uiMessageBox = nullptr;

        float scaling = 1;

        c2d::C2DClock timer_input;
    };
}

#endif //GUI_MAIN_H
