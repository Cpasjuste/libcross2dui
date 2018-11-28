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

class UIRomInfo : public RectangleShape {

public:

    UIRomInfo(UIMain *ui, Font *font, int fontSize,
              const FloatRect &rect, float scale) : RectangleShape(rect) {

        printf("UIRomInfo\n");

        this->ui = ui;

        setFillColor(Color::Transparent);
        scaling = scale;
        margin = UI_MARGIN * scaling;

        // info box
        infoBox = new RectangleShape(FloatRect(0, getSize().y / 2 + margin,
                                               getSize().x, getSize().y / 2 - margin));
        infoBox->setFillColor(Color::Gray);
        infoBox->setOutlineColor(COL_GREEN);
        infoBox->setOutlineThickness(2);

        infoText = new Text("", (unsigned int) fontSize, font);
        infoText->setPosition(margin, margin);
        infoText->setOutlineThickness(2);
        infoText->setWidth(infoBox->getSize().x);
        infoText->setLineSpacingModifier((int) (6 * scaling));
        infoBox->add(infoText);

        add(infoBox);

        // preview box
        previewBox = new RectangleShape(FloatRect(0, 0, getSize().x, getSize().y / 2));
        previewBox->setFillColor(Color::Gray);
        previewBox->setOutlineColor(COL_YELLOW);
        previewBox->setOutlineThickness(2);

        previewText = new Text("No Preview Image Available", (unsigned int) fontSize, font);
        previewText->setPosition(previewBox->getSize().x / 2, previewBox->getSize().y / 2);
        previewText->setOutlineThickness(2);
        previewText->setWidth(infoBox->getSize().x);
        previewText->setOrigin(Origin::Center);
        previewBox->add(previewText);

        add(previewBox);
    }

    ~UIRomInfo() {
        printf("~UIRomInfo\n");
    }

    bool loadTexture(RomList::Rom *rom, bool isPreview) {

        const char *type = isPreview ? "previews" : "titles";

        // load image
        snprintf(texture_path, 1023, "%s%s/%s.png",
                 ui->getConfig()->getHomePath()->c_str(), type, rom->drv_name);
        texture = new C2DTexture(texture_path);
        if (!texture->available) {
            // try removing the extension (drv_name has extension (.zip, .smc) with psnes and no db.xml)
            char *drv_name_no_ext = Utility::removeExt(rom->drv_name, '/');
            if (drv_name_no_ext) {
                delete (texture);
                memset(texture_path, 0, 1024);
                snprintf(texture_path, 1023, "%s%s/%s.png",
                         ui->getConfig()->getHomePath()->c_str(), type, drv_name_no_ext);
                texture = new C2DTexture(texture_path);
                free(drv_name_no_ext);
            }
            if (!texture->available && rom->parent) {
                // try parent image
                delete (texture);
                memset(texture_path, 0, 1024);
                snprintf(texture_path, 1023, "%s%s/%s.png",
                         ui->getConfig()->getHomePath()->c_str(), type, rom->parent);
                texture = new C2DTexture(texture_path);
            }
        }

        // set image
        if (texture->available) {
            previewText->setVisibility(Visibility::Hidden);
            texture->setOrigin(Origin::Center);
            texture->setPosition(Vector2f(previewBox->getSize().x / 2, previewBox->getSize().y / 2));
            float tex_scaling = std::min(
                    previewBox->getSize().x / texture->getTextureRect().width,
                    previewBox->getSize().y / texture->getTextureRect().height);
            texture->setScale(tex_scaling, tex_scaling);
            add(texture);
        } else {
            previewText->setVisibility(Visibility::Visible);
            delete (texture);
            texture = nullptr;
            return false;
        }

        return true;
    }

    void update(RomList::Rom *rom, bool isPreview) {

        if (texture) {
            delete (texture);
            texture = nullptr;
        }

        if (!rom) {
            infoText->setVisibility(Visibility::Hidden);
            previewText->setVisibility(Visibility::Visible);
        } else {
            // load title/preview texture
            if (!loadTexture(rom, isPreview)) {
                loadTexture(rom, !isPreview);
            }

            // update info text
            Option *rotation_opt = ui->getConfig()->get(Option::Index::ROM_ROTATION);
            if (rotation_opt && !(rotation_opt->flags & Option::Type::HIDDEN)) {
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
    Texture *texture = nullptr;
    RectangleShape *infoBox = nullptr;
    Text *infoText = nullptr;
    RectangleShape *previewBox = nullptr;
    Text *previewText = nullptr;
    char texture_path[1024];
    char info[1024];
    char rotation[64];
    float margin = 0;
    float scaling = 1;
};

UIRomList::UIRomList(UIMain *u, RomList *romList, const c2d::Vector2f &size) : RectangleShape(size) {

    printf("UIRomList\n");

    ui = u;
    rom_list = romList;

    // set gui main "window"
    setFillColor(Color::GrayDark);
    setOutlineColor(COL_ORANGE);
    setOutlineThickness(ui->getScaling() < 1 ? 1 : 2);
    setPosition(getOutlineThickness(), getOutlineThickness());
    setSize(Vector2f(getSize().x - getOutlineThickness() * 2, getSize().y - getOutlineThickness() * 2));

    // add title image if available
    Skin *skin = ui->getSkin();
    if (skin->tex_title->available) {
        skin->tex_title->setPosition(UI_MARGIN * ui->getScaling(), UI_MARGIN * ui->getScaling());
        float scale = (getLocalBounds().width / 3) / skin->tex_title->getTextureRect().width;
        skin->tex_title->setScale(scale, scale);
        add(skin->tex_title);
    }

    // add rom info ui
    rom_info = new UIRomInfo(ui, skin->font, ui->getFontSize(),
                             FloatRect(
                                     (getLocalBounds().width / 2) + UI_MARGIN * ui->getScaling(),
                                     UI_MARGIN * ui->getScaling(),
                                     (getLocalBounds().width / 2) - UI_MARGIN * ui->getScaling() * 2,
                                     getLocalBounds().height - UI_MARGIN * ui->getScaling() * 2),
                             ui->getScaling());
    rom_info->infoBox->setOutlineThickness(getOutlineThickness());
    add(rom_info);

    // filter roms
    updateRomList();
}

RomList::Rom *UIRomList::getSelection() {
    return (RomList::Rom *) list_box->getSelection();
}

RomList *UIRomList::getRomList() {
    return rom_list;
}

void UIRomList::updateRomList() {

    rom_index = 0;
    roms.clear();

    static RomList *list = rom_list;
    int showClone = ui->getConfig()->getValue(Option::Index::GUI_SHOW_CLONES);
    int showHardwareCfg = ui->getConfig()->getValue(Option::Index::GUI_SHOW_HARDWARE);
    int showHardware = ui->getConfig()->getHardwareList()->at((unsigned int) showHardwareCfg).prefix;

    // psnes and pnes have only 2 (0/1) values, so work with value string
    Option *opt = ui->getConfig()->get(Option::Index::GUI_SHOW_ALL);
    int showAll = opt->value;
    if (strcmp(opt->getValue(), "FAVORITES") == 0) {
        showAll = 2;
    }

    if (showAll == 2) {
        // filter/show favorites
        remove_copy_if(rom_list->list.begin(), rom_list->list.end(), back_inserter(roms),
                       [](RomList::Rom *r) {
                           return !(r->hardware & HARDWARE_PREFIX_FAV);
                       });
    } else {
        remove_copy_if(rom_list->list.begin(), rom_list->list.end(), back_inserter(roms),
                       [showAll, showClone, showHardware](RomList::Rom *r) {
                           return (showAll && r->state != RomList::RomState::WORKING)
                                  || (!showClone && r->parent != nullptr)
                                  || ((unsigned int) showHardware != HARDWARE_PREFIX_ALL
                                      && !list->isHardware(r->hardware, showHardware));
                       });
    }

    if (!list_box) {
        // add rom list ui
        float top = ui->getSkin()->tex_title->getGlobalBounds().top
                    + ui->getSkin()->tex_title->getGlobalBounds().height
                    + UI_MARGIN * ui->getScaling();
        FloatRect rect = {
                UI_MARGIN * ui->getScaling(), top,
                (getLocalBounds().width / 2) - UI_MARGIN * ui->getScaling(),
                getLocalBounds().height - top - UI_MARGIN * ui->getScaling()};
        list_box = new ListBox(ui->getSkin()->font, ui->getFontSize(), rect, (std::vector<Io::File *> &) roms,
                               ui->getConfig()->getValue(Option::Index::GUI_SHOW_ICONS) == 1);
        list_box->setHighlightUseFileColor(true);
        list_box->setOutlineThickness(getOutlineThickness());
        list_box->setFillColor(Color::Gray);
        list_box->setOutlineColor(COL_ORANGE);
        auto *tween = new TweenAlpha(50, 150, 1.0f, TweenLoop::PingPong);
        list_box->getHighlight()->add(tween);
        add(list_box);
    } else {
        list_box->setFiles((std::vector<Io::File *> &) roms);
    }

    if (rom_info) {
        rom_info->update(nullptr, show_preview);
        title_loaded = 0;
        timer_load.restart();
    }
}

int UIRomList::loop() {

    unsigned int key = ui->getInput()->getKeys();

    if (key > 0 && key != EV_DELAY) {

        if (key & Input::Key::KEY_UP) {
            rom_index--;
            if (rom_index < 0)
                rom_index = (int) (roms.size() - 1);
            list_box->setSelection(rom_index);
            show_preview = false;
            rom_info->update(nullptr, show_preview);
            title_loaded = 0;
        } else if (key & Input::Key::KEY_DOWN) {
            rom_index++;
            if ((unsigned int) rom_index >= roms.size())
                rom_index = 0;
            list_box->setSelection(rom_index);
            show_preview = false;
            rom_info->update(nullptr, show_preview);
            title_loaded = 0;
        } else if (key & Input::Key::KEY_RIGHT) {
            rom_index += list_box->getMaxLines();
            if ((unsigned int) rom_index >= roms.size())
                rom_index = (int) (roms.size() - 1);
            list_box->setSelection(rom_index);
            show_preview = false;
            rom_info->update(nullptr, show_preview);
            title_loaded = 0;
        } else if (key & Input::Key::KEY_LEFT) {
            rom_index -= list_box->getMaxLines();
            if (rom_index < 0)
                rom_index = 0;
            list_box->setSelection(rom_index);
            show_preview = false;
            rom_info->update(nullptr, show_preview);
            title_loaded = 0;
        } else if (key & Input::Key::KEY_FIRE1) {
            if (getSelection() != nullptr && getSelection()->state != RomList::RomState::MISSING) {
                show_preview = false;
                return UI_KEY_RUN_ROM;
            }
        } else if (key & Input::Key::KEY_FIRE3) {
            if (getSelection() != nullptr) {
                // remove from favorites
                if (getSelection()->hardware & HARDWARE_PREFIX_FAV) {
                    int res = ui->getUiMessageBox()->show("FAVORITES",
                                                          "remove selection from favorites ?", "OK", "CANCEL");
                    if (res == MessageBox::LEFT) {
                        rom_list->removeFav(getSelection());
                        Option *opt = ui->getConfig()->get(Option::Index::GUI_SHOW_ALL);
                        if (strcmp(opt->getValue(), "FAVORITES") == 0) {
                            // update list if we are in favorites
                            updateRomList();
                        }
                    }
                }
            }
        } else if (key & Input::Key::KEY_FIRE4) {
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
        } else if (key & Input::Key::KEY_FIRE5) {
            show_preview = !show_preview;
            rom_info->update(roms.size() > (unsigned int) rom_index ?
                             roms[rom_index] : nullptr, show_preview);
        } else if (key & Input::Key::KEY_FIRE6) {
            show_preview = !show_preview;
            rom_info->update(roms.size() > (unsigned int) rom_index ?
                             roms[rom_index] : nullptr, show_preview);
        } else if (key & Input::Key::KEY_START) {
            return UI_KEY_SHOW_MEMU_UI;
        } else if (key & Input::Key::KEY_COIN) {
            if (getSelection() != nullptr) {
                return UI_KEY_SHOW_MEMU_ROM;
            }
        } else if (key & EV_QUIT) {
            return EV_QUIT;
        }

        timer_load.restart();

    } else {
        if (!title_loaded && timer_load.getElapsedTime().asMilliseconds() > load_delay) {
            rom_info->update(roms.size() > (unsigned int) rom_index ?
                             roms[rom_index] : nullptr, show_preview);
            title_loaded = 1;
        }
    }

    ui->getRenderer()->flip();

    return 0;
}

UIRomList::~UIRomList() {

    printf("~UIRomList\n");
    delete (rom_list);
}
