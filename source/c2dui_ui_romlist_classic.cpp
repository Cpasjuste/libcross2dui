//
// Created by cpasjuste on 22/11/16.
//
#include <algorithm>

#include "c2dui.h"

#ifdef __SSCRAP__

#include "ss_api.h"

#endif

/// pFBA
#define BDF_ORIENTATION_FLIPPED     (1 << 1)
#define BDF_ORIENTATION_VERTICAL    (1 << 2)

using namespace c2d;
using namespace c2dui;
#ifdef __SSCRAP__
using namespace ss_api;
#endif

#ifdef __WINDOWS__
#undef MessageBox
#endif

class UIRomInfo : public Rectangle {

public:

    UIRomInfo(UIMain *u, UIRomList *uiRList, Font *font, int fontSize)
            : Rectangle(u->getSize()) {

        printf("UIRomInfo\n");

        ui = u;
        uiRomList = uiRList;

        // info box
        infoBox = new RectangleShape({16, 16});
        ui->getSkin()->loadRectangleShape(infoBox, {"MAIN", "ROM_INFO"});
        infoText = new Text("", (unsigned int) fontSize, font);
        ui->getSkin()->loadText(infoText, {"MAIN", "ROM_INFO", "TEXT"});
        infoBox->add(infoText);
        add(infoBox);

        // preview box
        previewBox = new RectangleShape(FloatRect(0, 0, getSize().x, getSize().y / 2));
        ui->getSkin()->loadRectangleShape(previewBox, {"MAIN", "ROM_IMAGE"});
        previewText = new Text("", (unsigned int) fontSize, font);
        ui->getSkin()->loadText(previewText, {"MAIN", "ROM_IMAGE", "TEXT"});
        previewBox->add(previewText);
        add(previewBox);
    }

    ~UIRomInfo() override {
        printf("~UIRomInfo\n");
    }

    bool loadTexture(const Game &game, bool isPreview) {

        texture = (C2DTexture *) uiRomList->getPreviewTexture(game, !isPreview);
#ifndef __SWITCH__
        // TODO: fix slow stat fs on switch
        if (texture == nullptr) {
            texture = (C2DTexture *) uiRomList->getPreviewTexture(game, isPreview);
        }
#endif
        // set image
        if ((texture != nullptr) && texture->available) {
            previewText->setVisibility(Visibility::Hidden);
            texture->setOrigin(Origin::Center);
            texture->setPosition(Vector2f(previewBox->getSize().x / 2, previewBox->getSize().y / 2));
            float tex_scaling = std::min(
                    previewBox->getSize().x / texture->getTextureRect().width,
                    previewBox->getSize().y / texture->getTextureRect().height);
            texture->setScale(tex_scaling, tex_scaling);
            previewBox->add(texture);
        } else {
            previewText->setVisibility(Visibility::Visible);
            return false;
        }

        return true;
    }

    void load(const Game &game, bool isPreview = false) {

        if (texture != nullptr) {
            delete (texture);
            texture = nullptr;
        }

        if (game.id.empty()) {
            //printf("load(%s, %i)\n", "nullptr", isPreview);
            infoText->setVisibility(Visibility::Hidden);
            infoText->setString("");
        } else {
            printf("load(%s, %i)\n", game.getName().text.c_str(), (int) isPreview);
            // load title/preview texture
            loadTexture(game, !isPreview);
            info = "FILE: " + game.path + "\n";
            info += game.getSynopsis().text;
            infoText->setString(info);
            infoText->setVisibility(Visibility::Visible);
        }
    }

    UIMain *ui = nullptr;
    UIRomList *uiRomList = nullptr;
    C2DTexture *texture = nullptr;
    RectangleShape *infoBox = nullptr;
    Text *infoText = nullptr;
    RectangleShape *previewBox = nullptr;
    Text *previewText = nullptr;
    std::string info;
};

UIRomListClassic::UIRomListClassic(UIMain *u, RomList *romList, const c2d::Vector2f &size)
        : UIRomList(u, romList, size) {

    printf("UIRomListClassic\n");

    Skin *skin = ui->getSkin();

    // set gui main "window"
    skin->loadRectangleShape(this, {"MAIN"});

    // add title image if available
    auto title = new RectangleShape({16, 16});
    skin->loadRectangleShape(title, {"MAIN", "TITLE"});
    add(title);

    // add help image if available
    auto help = new RectangleShape({16, 16});
    skin->loadRectangleShape(help, {"MAIN", "HELP"});
    add(help);

    // add rom info ui
    rom_info = new UIRomInfo(ui, this, skin->font, ui->getFontSize());
    add(rom_info);

    // filter roms
    updateRomList();
}

Game UIRomListClassic::getSelection() {
    return list_box->getSelection();
}

void UIRomListClassic::updateRomList() {

    rom_index = 0;
    // TODO: sscrap - filterRomList
    filterRomList();

    Skin::TextGroup textGroup = ui->getSkin()->getText({"MAIN", "ROM_LIST", "TEXT"});
    config::Group *grp = ui->getSkin()->getConfig()->getGroup("ROM_LIST")->getGroup("TEXT");
    Color colorMissing = grp->getOption("color_missing")->getColor();
    bool highlightUseFileColors = grp->getOption("highlight_use_text_color")->getInteger() == 1;

    if (!list_box) {
        // add rom list ui
        Skin::RectangleShapeGroup romListGroup = ui->getSkin()->getRectangleShape({"MAIN", "ROM_LIST"});
        bool use_icons = false;
#if !(defined(__PSP2__) || defined(__3DS__)) // two slow
        use_icons = ui->getConfig()->get(Option::Id::GUI_SHOW_ICONS)->getValueBool();
#endif
        list_box = new UIListBox(ui->getSkin()->font, (int) textGroup.size, romListGroup.rect, games, use_icons);
        list_box->colorMissing = colorMissing;
        list_box->colorAvailable = textGroup.color;
        list_box->setFillColor(romListGroup.color);
        list_box->setOutlineColor(romListGroup.outlineColor);
        list_box->setOutlineThickness((float) romListGroup.outlineSize);
        list_box->setSelection(0);
        // rom item
        list_box->setTextOutlineColor(textGroup.outlineColor);
        list_box->setTextOutlineThickness((float) textGroup.outlineSize);
        // hihglight
        Skin::RectangleShapeGroup rectShape = ui->getSkin()->getRectangleShape({"SKIN_CONFIG", "HIGHLIGHT"});
        list_box->getHighlight()->setFillColor(rectShape.color);
        list_box->getHighlight()->setOutlineColor(rectShape.outlineColor);
        list_box->getHighlight()->setOutlineThickness((float) rectShape.outlineSize);
        list_box->setHighlightUseFileColor(highlightUseFileColors);
        add(list_box);
    } else {
        list_box->setGames(games);
    }

    if (rom_info) {
        rom_info->load(Game());
        title_loaded = 0;
        timer_load.restart();
    }
}

bool UIRomListClassic::onInput(c2d::Input::Player *players) {

    if (ui->getUiMenu()->isVisible()
        || ui->getUiStateMenu()->isVisible()
        || ui->getUiProgressBox()->isVisible()) {
        return false;
    }

    unsigned int keys = players[0].keys;
    if (keys & Input::Key::Up) {
        rom_index--;
        if (rom_index < 0)
            rom_index = (int) (games.size() - 1);
        list_box->up();
        show_preview = false;
        rom_info->load(Game());
        title_loaded = 0;
    } else if (keys & Input::Key::Down) {
        rom_index++;
        if ((unsigned int) rom_index >= games.size())
            rom_index = 0;
        list_box->down();
        show_preview = false;
        rom_info->load(Game());
        title_loaded = 0;
    } else if (keys & Input::Key::Right) {
        rom_index += list_box->getMaxLines();
        if ((unsigned int) rom_index >= games.size())
            rom_index = (int) (games.size() - 1);
        list_box->setSelection(rom_index);
        show_preview = false;
        rom_info->load(Game());
        title_loaded = 0;
    } else if (keys & Input::Key::Left) {
        rom_index -= list_box->getMaxLines();
        if (rom_index < 0)
            rom_index = 0;
        list_box->setSelection(rom_index);
        show_preview = false;
        rom_info->load(Game());
        title_loaded = 0;
    } else if (keys & Input::Key::Fire1) {
        Game game = getSelection();
        if (game.available) {
            show_preview = false;
            ui->getConfig()->load(game);
            ui->getUiEmu()->load(game);
            return true;
        }
    } else if (keys & Input::Key::Fire4) {
        if (!getSelection().id.empty()) {
            // remove from favorites
            // TODO: sscrap - favs
            /*
            if (getSelection()->hardware & (unsigned int) HARDWARE_PREFIX_FAV) {
                int res = ui->getUiMessageBox()->show("FAVORITES",
                                                      "remove selection from favorites ?", "OK", "CANCEL");
                if (res == MessageBox::LEFT) {
                    rom_list->removeFav(getSelection());
                    Option *opt = ui->getConfig()->get(Option::Id::GUI_SHOW_ALL);
                    if (opt->getValueString() == "FAVORITES") {
                        // update list if we are in favorites
                        updateRomList();
                    }
                }
            }
            */
        }
    } else if (keys & Input::Key::Fire3) {
        // TODO: sscrap - favs
        /*
        if (getSelection() != nullptr) {
            // add to favorites
            if (!(getSelection()->hardware & (unsigned int) HARDWARE_PREFIX_FAV)) {
                int res = ui->getUiMessageBox()->show("FAVORITES",
                                                      "add selection to favorites ?", "OK", "CANCEL");
                if (res == MessageBox::LEFT) {
                    rom_list->addFav(getSelection());
                }
            }
        }
        */
    } else if (keys & Input::Key::Fire5) {
        show_preview = !show_preview;
        rom_info->load(games.size() > (unsigned int) rom_index ?
                       games[rom_index] : Game(), show_preview);
    } else if (keys & Input::Key::Fire6) {
        show_preview = !show_preview;
        rom_info->load(games.size() > (unsigned int) rom_index ?
                       games[rom_index] : Game(), show_preview);
    } else if (keys & Input::Key::Start) {
        ui->getUiMenu()->load();
    } else if (keys & Input::Key::Select) {
        if (!getSelection().id.empty()) {
            ui->getConfig()->load(getSelection());
            ui->getUiMenu()->load(true);

        }
    } else if (keys & EV_QUIT) {
        ui->done = true;
    }

    return true;
}

void UIRomListClassic::onUpdate() {

    RectangleShape::onUpdate();

    unsigned int keys = ui->getInput()->getKeys();

    if (keys > 0 && keys != Input::Delay) {
        timer_load.restart();
    } else if (keys == 0) {
        if (!title_loaded && timer_load.getElapsedTime().asMilliseconds() > load_delay) {
            rom_info->load(games.size() > (unsigned int) rom_index ?
                           games[rom_index] : Game(), show_preview);
            title_loaded = 1;
        }
    }
}

UIRomListClassic::~UIRomListClassic() {
    printf("~UIRomListClassic\n");
}
