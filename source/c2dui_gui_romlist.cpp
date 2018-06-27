//
// Created by cpasjuste on 22/11/16.
//
#include <algorithm>

#include "c2dui.h"

#ifdef __PFBA__
// TODO: remove pfba deps
#define BDF_ORIENTATION_FLIPPED     (1 << 1)
#define BDF_ORIENTATION_VERTICAL    (1 << 2)
#endif

using namespace c2d;
using namespace c2dui;

class C2DUIGuiRomInfo : public Rectangle {

public:

    C2DUIGuiRomInfo(C2DUIGuiMain *ui, const Font &font, int fontSize, const FloatRect &rect, float scale)
            : Rectangle(rect) {

        printf("C2DUIGuiRomInfo\n");

        this->ui = ui;

        setFillColor(Color::Transparent);
        scaling = scale;
        margin = UI_MARGIN * scaling;

        // info box
        infoBox = new Rectangle(FloatRect(0, getSize().y / 2 + margin,
                                          getSize().x, getSize().y / 2 - margin));
        infoBox->setFillColor(Color::GrayLight);
        infoBox->setOutlineColor(COL_GREEN);
        infoBox->setOutlineThickness(2);

        infoText = new Text("", font, (unsigned int) fontSize);
        infoText->setPosition(margin, margin);
        infoText->setOutlineThickness(2);
        infoText->setSizeMax(Vector2f(infoBox->getSize().x, 0));
        infoText->setLineSpacingModifier((int) (2 * scaling));
        infoBox->add(infoText);

        add(infoBox);

        // preview box
        previewBox = new Rectangle(FloatRect(0, 0, getSize().x, getSize().y / 2));
        previewBox->setFillColor(Color::GrayLight);
        previewBox->setOutlineColor(COL_YELLOW);
        previewBox->setOutlineThickness(2);

        previewText = new Text("No Preview Image Available", font, (unsigned int) fontSize);
        previewText->setPosition(previewBox->getSize().x / 2, previewBox->getSize().y / 2);
        previewText->setOutlineThickness(2);
        previewText->setSizeMax(Vector2f(infoBox->getSize().x, 0));
        previewText->setLineSpacingModifier((int) (8 * scaling));
        previewText->setOriginCenter();
        previewBox->add(previewText);

        add(previewBox);
    }

    ~C2DUIGuiRomInfo() {
        printf("~GuiRomInfo\n");
    }

    bool loadTexture(C2DUIRomList::Rom *rom, bool isPreview) {

        const char *type = isPreview ? "previews" : "titles";

        // load image
        snprintf(texture_path, 1023, "%s%s/%s.png",
                 ui->getConfig()->getHomePath()->c_str(), type, rom->drv_name);
        texture = new C2DTexture(texture_path);
        if (!texture->available) {
#ifdef __PSNES__
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
#endif
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
            texture->setOriginCenter();
            texture->setPosition(Vector2f(previewBox->getSize().x / 2, previewBox->getSize().y / 2));
            float tex_scaling = std::min(
                    previewBox->getSize().x / texture->getSize().x,
                    previewBox->getSize().y / texture->getSize().y);
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

    void update(C2DUIRomList::Rom *rom, bool isPreview) {

        if (texture) {
            delete (texture);
            texture = nullptr;
        }

        if (!rom) {
            infoText->setVisibility(Hidden);
            previewText->setVisibility(Visibility::Visible);
        } else {
            // load title/preview texture
            if (!loadTexture(rom, isPreview)) {
                loadTexture(rom, !isPreview);
            }

            // update info text
#ifdef __PFBA__
            strcpy(rotation, "ROTATION: HORIZONTAL");
            if (rom->flags & BDF_ORIENTATION_VERTICAL) {
                sprintf(rotation, "ROTATION: VERTICAL");
                if (rom->flags & BDF_ORIENTATION_FLIPPED) {
                    strncat(rotation, " / FLIPPED", MAX_PATH);
                }
            }
            snprintf(info, 1024, "FILE: %s\nSTATUS: %s\nSYSTEM: %s\nMANUFACTURER: %s\nYEAR: %s\n%s",
                     rom->path, rom->state == C2DUIRomList::RomState::MISSING ? "MISSING" : "AVAILABLE",
                     rom->system, rom->manufacturer, rom->year, rotation);
            infoText->setString(info);
            infoText->setVisibility(Visible);
#elif __PSNES__
            snprintf(info, 1023, "FILE: %s\nSTATUS: %s\nMANUFACTURER: %s\nYEAR: %s",
                     rom->path, rom->state == C2DUIRomList::RomState::MISSING ? "MISSING" : "AVAILABLE",
                     rom->manufacturer, rom->year);
            infoText->setString(info);
            infoText->setVisibility(Visible);
#endif
        }
    }

    C2DUIGuiMain *ui = nullptr;
    Texture *texture = nullptr;
    Rectangle *infoBox = nullptr;
    Text *infoText = nullptr;
    Rectangle *previewBox = nullptr;
    Text *previewText = nullptr;
    char texture_path[1024];
    char info[1024];
    char rotation[64];
    float margin = 0;
    float scaling = 1;
};

C2DUIGuiRomList::C2DUIGuiRomList(C2DUIGuiMain *u, C2DUIRomList *romList, const c2d::Vector2f &size) : Rectangle(size) {

    printf("GuiRomList\n");

    ui = u;
    rom_list = romList;

    // set gui main "window"
    setFillColor(Color::Gray);
    setOutlineColor(COL_ORANGE);
    setOutlineThickness(ui->getScaling() < 1 ? 1 : 2);
    setPosition(getOutlineThickness(), getOutlineThickness());
    setSize(Vector2f(getSize().x - getOutlineThickness() * 2, getSize().y - getOutlineThickness() * 2));

    // add title image if available
    C2DUISkin *skin = ui->getSkin();
    if (skin->tex_title->available) {
        skin->tex_title->setPosition(UI_MARGIN * ui->getScaling(), UI_MARGIN * ui->getScaling());
        float scale = (getLocalBounds().width / 3) / skin->tex_title->getSize().x;
        skin->tex_title->setScale(scale, scale);
        add(skin->tex_title);
    }

    // filter roms
    updateRomList();

    // add rom info ui
    rom_info = new C2DUIGuiRomInfo(ui, *skin->font, ui->getFontSize(),
                                   FloatRect(
                                           (getLocalBounds().width / 2) + UI_MARGIN * ui->getScaling(),
                                           UI_MARGIN * ui->getScaling(),
                                           (getLocalBounds().width / 2) - UI_MARGIN * ui->getScaling() * 2,
                                           getLocalBounds().height - UI_MARGIN * ui->getScaling() * 2),
                                   ui->getScaling());
    rom_info->infoBox->setOutlineThickness(getOutlineThickness());
    rom_info->update(roms.empty() ? nullptr : roms[0], show_preview);
    add(rom_info);
}

C2DUIRomList::Rom *C2DUIGuiRomList::getSelection() {
    return (C2DUIRomList::Rom *) list_box->getSelection();
}

C2DUIRomList *C2DUIGuiRomList::getRomList() {
    return rom_list;
}

void C2DUIGuiRomList::updateRomList() {

    rom_index = 0;
    roms.clear();

    static C2DUIRomList *list = rom_list;
    int showClone = ui->getConfig()->getValue(C2DUIOption::Index::GUI_SHOW_CLONES);
    int showAll = ui->getConfig()->getValue(C2DUIOption::Index::GUI_SHOW_ALL);
    int showHardwareCfg = ui->getConfig()->getValue(C2DUIOption::Index::GUI_SHOW_HARDWARE);
    int showHardware = ui->getConfig()->getHardwareList()->at((unsigned int) showHardwareCfg).prefix;

    remove_copy_if(rom_list->list.begin(), rom_list->list.end(), back_inserter(roms),
                   [showAll, showClone, showHardware](C2DUIRomList::Rom *r) {
                       return (!showAll && r->state != C2DUIRomList::RomState::WORKING)
                              || (!showClone && r->parent != nullptr)
                              || ((unsigned int) showHardware != HARDWARE_PREFIX_ALL
                                  && !list->isHardware(r->hardware, showHardware));
                   });

    if (!list_box) {
        // add rom list ui
        float top = ui->getSkin()->tex_title->getGlobalBounds().top
                    + ui->getSkin()->tex_title->getGlobalBounds().height
                    + UI_MARGIN * ui->getScaling();
        FloatRect rect = {
                UI_MARGIN * ui->getScaling(), top,
                (getLocalBounds().width / 2) - UI_MARGIN * ui->getScaling(),
                getLocalBounds().height - top - UI_MARGIN * ui->getScaling()};
        list_box = new ListBox(*ui->getSkin()->font, ui->getFontSize(), rect, (std::vector<Io::File *> &) roms,
                               ui->getConfig()->getValue(C2DUIOption::Index::GUI_SHOW_ICONS) == 1);
        list_box->setOutlineThickness(getOutlineThickness());
        list_box->setFillColor(Color::GrayLight);
        list_box->setOutlineColor(COL_ORANGE);
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

int C2DUIGuiRomList::update() {

    unsigned int key = ui->getInput()->update()[0].state;

    if (key > 0) {

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
            if (getSelection() != nullptr
                && getSelection()->state != C2DUIRomList::RomState::MISSING) {
                show_preview = false;
                return UI_KEY_RUN_ROM;
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

C2DUIGuiRomList::~C2DUIGuiRomList() {

    printf("~C2DUIGuiRomList\n");
    delete (rom_list);
}
