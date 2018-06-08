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

    class C2DUIGuiMain {

    public:

        C2DUIGuiMain(c2d::Renderer *renderer, c2d::Io *io, c2d::Input *input, c2d::Audio *audio,
                     c2dui::C2DUIConfig *config, c2dui::C2DUISkin *skin);

        virtual ~C2DUIGuiMain();

        virtual void runRom(C2DUIRomList::Rom *rom);

        void init(C2DUIGuiRomList *uiRomList, C2DUIGuiMenu *uiMenu,
                  C2DUIGuiEmu *uiEmu, C2DUIGuiState *uiState);

        void run();

        void updateInputMapping(bool isRomCfg);

        c2d::Renderer *getRenderer();

        c2dui::C2DUISkin *getSkin();

        C2DUIConfig *getConfig();

        c2d::Input *getInput();

        c2d::Io *getIo();

        c2d::Audio *getAudio();

        void setAudio(c2d::Audio *audio);

        C2DUIGuiRomList *getUiRomList();

        C2DUIGuiEmu *getUiEmu();

        C2DUIGuiMenu *getUiMenu();

        C2DUIGuiProgressBox *getUiProgressBox();

        c2d::MessageBox *getUiMessageBox();

        c2d::Font *getFont();

        int getFontSize();

        float getScaling();

    private:

        c2dui::C2DUIConfig *config = NULL;
        c2dui::C2DUISkin *skin = NULL;

        c2d::Io *io = NULL;
        c2d::Renderer *renderer = NULL;
        c2d::Input *input = NULL;
        c2d::Audio *audio = NULL;

        C2DUIGuiMenu *uiMenu = NULL;
        C2DUIGuiRomList *uiRomList = NULL;
        C2DUIGuiEmu *uiEmu = NULL;
        C2DUIGuiState *uiState = NULL;
        C2DUIGuiProgressBox *uiProgressBox = NULL;
        c2d::MessageBox *uiMessageBox = NULL;

        float scaling = 1;

        c2d::C2DClock timer_input;
    };
}

#endif //GUI_MAIN_H
