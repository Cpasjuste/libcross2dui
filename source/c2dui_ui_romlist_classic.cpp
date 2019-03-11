//
// Created by cpasjuste on 22/11/16.
//
#include <algorithm>

#include "c2dui.h"

/// pFBA
#define BDF_ORIENTATION_FLIPPED     (1 << 1)
#define BDF_ORIENTATION_VERTICAL    (1 << 2)

using namespace c2d;
using namespace c2dui;

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

        texture = uiRomList->getPreviewTexture(rom, isPreview);

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
            if (texture) {
                delete (texture);
                texture = nullptr;
            }
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
            infoText->setVisibility(Visibility::Hidden);
        } else {
            // load title/preview texture
            if (!loadTexture(rom, isPreview)) {
                loadTexture(rom, !isPreview);
            }

            // update info text
            Option *rotation_opt = ui->getConfig()->get(Option::Index::ROM_ROTATION);
            if (rotation_opt && !(rotation_opt->getFlags() & Option::Flags::HIDDEN)) {
                strcpy(rotation, "ROTATION: HORIZONTAL");
                if (rom->flags & BDF_ORIENTATION_VERTICAL) {
                    sprintf(rotation, "ROTATION: VERTICAL");
                    if (rom->flags & BDF_ORIENTATION_FLIPPED) {
                        strncat(rotation, " / FLIPPED", MAX_PATH);
                    }
                }
                snprintf(info, 1024, "FILE: %s\nSTATUS: %s\nSYSTEM: %s\nMANUFACTURER: %s\nYEAR: %s\n%s",
                         rom->name.c_str(), rom->state == RomList::RomState::MISSING ? "MISSING" : "AVAILABLE",
                         rom->system, rom->manufacturer, rom->year, rotation);
            } else {
                snprintf(info, 1023, "FILE: %s\nSTATUS: %s\nMANUFACTURER: %s\nYEAR: %s",
                         rom->name.c_str(), rom->state == RomList::RomState::MISSING ? "MISSING" : "AVAILABLE",
                         rom->manufacturer, rom->year);
            }
            infoText->setString(info);
            infoText->setVisibility(Visibility::Visible);
        }
    }

    UIMain *ui = nullptr;
    UIRomList *uiRomList = nullptr;
    Texture *texture = nullptr;
    RectangleShape *infoBox = nullptr;
    Text *infoText = nullptr;
    RectangleShape *previewBox = nullptr;
    Text *previewText = nullptr;
    char info[1024]{};
    char rotation[64]{};
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

    Skin::RectangleShapeGroup itemGroup = ui->getSkin()->getRectangleShape({"MAIN", "ROM_LIST", "ROM_ITEM"});

    if (!list_box) {
        // add rom list ui
        Skin::RectangleShapeGroup romListGroup = ui->getSkin()->getRectangleShape({"MAIN", "ROM_LIST"});
        list_box = new ListBox(ui->getSkin()->font, (int) itemGroup.rect.height,
                               romListGroup.rect, (std::vector<Io::File *> &) roms,
                               ui->getConfig()->get(Option::Index::GUI_SHOW_ICONS)->getValueBool());
        list_box->setFillColor(romListGroup.color);
        list_box->setOutlineColor(romListGroup.outlineColor);
        list_box->setOutlineThickness(romListGroup.outlineSize);
        list_box->setSelection(0);
        // rom item
        list_box->setTextOutlineColor(itemGroup.outlineColor);
        list_box->setTextOutlineThickness(itemGroup.outlineSize);
        // hihglight
        ui->getSkin()->loadRectangleShape(list_box->getHighlight(), {"SKIN_CONFIG", "HIGHLIGHT"});
        add(list_box);
    } else {
        list_box->setFiles((std::vector<Io::File *> &) roms);
    }

    // set item/rom text color
    if (!roms.empty()) {
        for (auto &rom : roms) {
            if (rom->state == RomList::RomState::WORKING) {
                rom->color = itemGroup.color;
            }
        }
    }

    if (rom_info) {
        rom_info->load(nullptr);
        title_loaded = 0;
        timer_load.restart();
    }
}

bool UIRomListClassic::onInput(c2d::Input::Player *players) {

    if (ui->getUiMenu()->isVisible()) {
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
    } else if (keys & Input::Key::Fire3) {
        if (getSelection() != nullptr) {
            // remove from favorites
            if (getSelection()->hardware & HARDWARE_PREFIX_FAV) {
                int res = ui->getUiMessageBox()->show("FAVORITES",
                                                      "remove selection from favorites ?", "OK", "CANCEL");
                if (res == MessageBox::LEFT) {
                    rom_list->removeFav(getSelection());
                    Option *opt = ui->getConfig()->get(Option::Index::GUI_SHOW_ALL);
                    if (strcmp(opt->getValueString(), "FAVORITES") == 0) {
                        // update list if we are in favorites
                        updateRomList();
                    }
                }
            }
        }
    } else if (keys & Input::Key::Fire4) {
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

    unsigned int key = ui->getInput()->getKeys();

    if (key > 0 && key != Input::Key::Delay) {
        timer_load.restart();
    } else {
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
