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

    bool loadTexture(RomList::Rom *rom, bool isPreview) {

        texture = (C2DTexture *) uiRomList->getPreviewTexture(rom, !isPreview);
#ifndef __SWITCH__
        // TODO: fix slow stat fs on switch
        if (texture == nullptr) {
            texture = (C2DTexture *) uiRomList->getPreviewTexture(rom, isPreview);
        }
#endif
        // set image
        if (texture && texture->available) {
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

    void load(RomList::Rom *rom, bool isPreview = false) {

        if (texture) {
            delete (texture);
            texture = nullptr;
        }

        if (!rom) {
            printf("load(%s, %i)\n", "nullptr", isPreview);
            infoText->setVisibility(Visibility::Hidden);
        } else {
            printf("load(%s, %i)\n", rom->name.c_str(), isPreview);
#ifdef __SSCRAP__
            Api::JeuInfos jeuInfos = ui->getScrapper()->scrap->jeuInfos("cache/" + rom->name + ".json");
            if (jeuInfos.jeu.id.empty()) {
                ui->getScrapper()->addRom(rom->name);
            } else {
                printf("jeuInfos: nom: %s, system: %s\n",
                       jeuInfos.jeu.noms[0].text.c_str(), jeuInfos.jeu.systemenom.c_str());
            }
#endif
            // load title/preview texture
            loadTexture(rom, !isPreview);
#ifdef __SSCRAP__
            if (!jeuInfos.jeu.id.empty()) {
                info = jeuInfos.jeu.synopsis[0].text;
            } else {
#endif
            // update info text
            info = "FILE: ";
            info += rom->drv_name;
            info += "\nSTATUS: ";
            info += rom->state == RomList::RomState::MISSING ? "MISSING" : "AVAILABLE";
            if (rom->year) {
                info += "\nYEAR: ";
                info += rom->year;
            }
            if (rom->system) {
                info += "\nSYSTEM: ";
                info += rom->system;
            }
            if (rom->manufacturer) {
                info += "\nMANUFACTURER: ";
                info += rom->manufacturer;
            }
            Option *opt = ui->getConfig()->get(Option::Id::ROM_ROTATION);
            if (opt && !(opt->getFlags() & Option::Flags::HIDDEN)) {
                info += "\nROTATION: ";
                if (rom->flags & BDF_ORIENTATION_VERTICAL) {
                    info += "VERTICAL";
                } else {
                    info += "HORIZONTAL";
                }
                if (rom->flags & BDF_ORIENTATION_FLIPPED) {
                    info += " / FLIPPED";
                }
            }
#ifdef __SSCRAP__
            }
#endif
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
    RectangleShape *title = new RectangleShape({16, 16});
    skin->loadRectangleShape(title, {"MAIN", "TITLE"});
    add(title);

    // add help image if available
    RectangleShape *help = new RectangleShape({16, 16});
    skin->loadRectangleShape(help, {"MAIN", "HELP"});
    add(help);

    // add rom info ui
    rom_info = new UIRomInfo(ui, this, skin->font, ui->getFontSize());
    add(rom_info);

    // filter roms
    updateRomList();
}

RomList::Rom *UIRomListClassic::getSelection() {
    return (RomList::Rom *) list_box->getSelection();
}

void UIRomListClassic::updateRomList() {

    rom_index = 0;
    filterRomList();

    Skin::TextGroup textGroup = ui->getSkin()->getText({"MAIN", "ROM_LIST", "TEXT"});
    config::Group *grp = ui->getSkin()->getConfig()->getGroup("ROM_LIST")->getGroup("TEXT");
    Color colorMissing = grp->getOption("color_missing")->getColor();
    Color colorNotWorking = grp->getOption("color_not_working")->getColor();
    bool highlightUseFileColors = grp->getOption("highlight_use_text_color")->getInteger() == 1;

    // set item/rom text color
    if (!roms.empty()) {
        for (auto &rom : roms) {
            if (rom->state == RomList::RomState::WORKING) {
                rom->color = textGroup.color;
            } else if (rom->state == RomList::RomState::MISSING) {
                rom->color = colorMissing;
            }
            if (rom->state == RomList::RomState::NOT_WORKING) {
                rom->color = colorNotWorking;
            }
        }
    }

    if (!list_box) {
        // add rom list ui
        Skin::RectangleShapeGroup romListGroup = ui->getSkin()->getRectangleShape({"MAIN", "ROM_LIST"});
        list_box = new ListBox(ui->getSkin()->font, (int) textGroup.size,
                               romListGroup.rect, (std::vector<Io::File *> &) roms,
                               ui->getConfig()->get(Option::Id::GUI_SHOW_ICONS)->getValueBool());
        list_box->setFillColor(romListGroup.color);
        list_box->setOutlineColor(romListGroup.outlineColor);
        list_box->setOutlineThickness(romListGroup.outlineSize);
        list_box->setSelection(0);
        // rom item
        list_box->setTextOutlineColor(textGroup.outlineColor);
        list_box->setTextOutlineThickness(textGroup.outlineSize);
        // hihglight
        ui->getSkin()->loadRectangleShape(list_box->getHighlight(), {"SKIN_CONFIG", "HIGHLIGHT"});
        list_box->setHighlightUseFileColor(highlightUseFileColors);
        add(list_box);
    } else {
        list_box->setFiles((std::vector<Io::File *> &) roms);
    }

    if (rom_info) {
        rom_info->load(nullptr);
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
            rom_index = (int) (roms.size() - 1);
        list_box->setSelection(rom_index);
        show_preview = false;
        rom_info->load(nullptr);
        title_loaded = 0;
    } else if (keys & Input::Key::Down) {
        rom_index++;
        if ((unsigned int) rom_index >= roms.size())
            rom_index = 0;
        list_box->setSelection(rom_index);
        show_preview = false;
        rom_info->load(nullptr);
        title_loaded = 0;
    } else if (keys & Input::Key::Right) {
        rom_index += list_box->getMaxLines();
        if ((unsigned int) rom_index >= roms.size())
            rom_index = (int) (roms.size() - 1);
        list_box->setSelection(rom_index);
        show_preview = false;
        rom_info->load(nullptr);
        title_loaded = 0;
    } else if (keys & Input::Key::Left) {
        rom_index -= list_box->getMaxLines();
        if (rom_index < 0)
            rom_index = 0;
        list_box->setSelection(rom_index);
        show_preview = false;
        rom_info->load(nullptr);
        title_loaded = 0;
    } else if (keys & Input::Key::Fire1) {
        RomList::Rom *rom = getSelection();
        if (rom && rom->state != RomList::RomState::MISSING) {
            show_preview = false;
            ui->getConfig()->load(rom);
            ui->getUiEmu()->load(rom);
            return true;
        }
    } else if (keys & Input::Key::Fire4) {
        if (getSelection() != nullptr) {
            // remove from favorites
            if (getSelection()->hardware & HARDWARE_PREFIX_FAV) {
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
        }
    } else if (keys & Input::Key::Fire3) {
        if (getSelection() != nullptr) {
            // add to favorites
            if (!(getSelection()->hardware & HARDWARE_PREFIX_FAV)) {
                int res = ui->getUiMessageBox()->show("FAVORITES",
                                                      "add selection to favorites ?", "OK", "CANCEL");
                if (res == MessageBox::LEFT) {
                    rom_list->addFav(getSelection());
                }
            }
        }
    } else if (keys & Input::Key::Fire5) {
        show_preview = !show_preview;
        rom_info->load(roms.size() > (unsigned int) rom_index ?
                       roms[rom_index] : nullptr, show_preview);
    } else if (keys & Input::Key::Fire6) {
        show_preview = !show_preview;
        rom_info->load(roms.size() > (unsigned int) rom_index ?
                       roms[rom_index] : nullptr, show_preview);
    } else if (keys & Input::Key::Start) {
        ui->getUiMenu()->load();
    } else if (keys & Input::Key::Select) {
        if (getSelection() != nullptr) {
            ui->getConfig()->load(getSelection());
            ui->getUiMenu()->load(true);

        }
    } else if (keys & EV_QUIT) {
        ui->done = true;
    }

    return true;
}

void UIRomListClassic::onDraw(c2d::Transform &transform, bool draw) {

    unsigned int keys = ui->getInput()->getKeys();

    if (keys > 0 && keys != Input::Delay) {
        timer_load.restart();
    } else if (keys == 0) {
        if (!title_loaded && timer_load.getElapsedTime().asMilliseconds() > load_delay) {
            rom_info->load(roms.size() > (unsigned int) rom_index ?
                           roms[rom_index] : nullptr, show_preview);
            title_loaded = 1;
        }
    }

    RectangleShape::onDraw(transform, draw);
}

UIRomListClassic::~UIRomListClassic() {
    printf("~UIRomListClassic\n");
}
