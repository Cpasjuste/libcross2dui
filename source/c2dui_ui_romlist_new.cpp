//
// Created by cpasjuste on 22/11/16.
//
#include <algorithm>

#include "c2dui.h"

#define ROM_ITEMS_OUTLINE_SIZE 5

/// pFBA
#define BDF_ORIENTATION_FLIPPED     (1 << 1)
#define BDF_ORIENTATION_VERTICAL    (1 << 2)

using namespace c2d;
using namespace c2dui;

class UIRomItem : public RoundedRectangleShape {

public:

    UIRomItem(UIMain *u, RomList::Rom *r, const FloatRect &rect)
            : RoundedRectangleShape({rect.width, rect.height}) {
        ui = u;
        rom = r;
        setPosition(rect.left, rect.top);
        setFillColor(Color::White);
        setOutlineThickness(ROM_ITEMS_OUTLINE_SIZE);
        setOutlineColor(Color::GrayLight);
    }

    void setPreview(Texture *tex) {

        if (preview) {
            delete (preview);
            preview = nullptr;
        }

        if (!tex) {
            return;
        }

        preview = tex;
        if (preview->available) {
            preview->setOrigin(Origin::Top);
            preview->setPosition(getSize().x / 2, 0);
            float scale = std::min(
                    getSize().x / preview->getSize().x,
                    ((getSize().y / 5) * 4) / preview->getSize().y);
            preview->setScale(scale, scale);
            add(preview);
        } else {
            delete (preview);
            preview = nullptr;
        }
    }

private:

    void onDraw(Transform &transform) override {
        if (getGlobalBounds().intersects(ui->getGlobalBounds())) {
            if (!previewLoaded) {
                printf("loading preview: %s\n", rom->name.c_str());
                setPreview(ui->getUiRomList()->getPreviewTexture(rom, false));
                previewLoaded = true;
            }
            Shape::onDraw(transform);
        } else {
            if (previewLoaded) {
                printf("unloading preview: %s\n", rom->name.c_str());
                setPreview(nullptr);
                previewLoaded = false;
            }
        }
    }

    UIMain *ui = nullptr;
    RomList::Rom *rom = nullptr;
    Texture *preview = nullptr;
    bool previewLoaded = false;
};

class UIRomHighlight : public RoundedRectangleShape {

public:

    explicit UIRomHighlight(const FloatRect &rect) : RoundedRectangleShape({rect.width, rect.height}) {
        setPosition(rect.left, rect.top);
        setOrigin(Origin::Center);
        setFillColor(Color::Cyan);
        setAlpha(100);
        setOutlineThickness(ROM_ITEMS_OUTLINE_SIZE);
        setOutlineColor(Color::Cyan);
    }
};

UIRomListNew::UIRomListNew(UIMain *u, RomList *romList, const c2d::Vector2f &size)
        : UIRomList(u, romList, size) {

    printf("UIRomListNew\n");

    ui = u;
    rom_list = romList;

    add(new C2DTexture("skin/romlist_bg.png"));

    rom_item_size = {getSize().x / 6, getSize().y / 3};
    highlight = new UIRomHighlight({0, 0,
                                    rom_item_size.x + ROM_ITEMS_OUTLINE_SIZE * 2,
                                    rom_item_size.y + ROM_ITEMS_OUTLINE_SIZE * 2});
    add(highlight);

    updateRomList();
}

RomList::Rom *UIRomListNew::getSelection() {
    return nullptr;
}

void UIRomListNew::updateRomList() {

    filterRomList();

    delete (rom_items_layer);
    rom_items_layer = new Rectangle({0, getSize().y / 2, 16, 16});
    add(rom_items_layer);

    float start_x = 32 + rom_item_size.x / 2;

    for (unsigned int i = 0; i < roms.size(); i++) {
        UIRomItem *item = new UIRomItem(ui, roms[i], {0, 0, rom_item_size.x, rom_item_size.y});
        item->setOrigin(Origin::Center);
        item->setPosition(((rom_item_size.x + 32) * i) + start_x, rom_items_layer->getSize().y / 2);
        rom_items_layer->add(item);
        rom_items.push_back(item);
    }

    rom_index = 0;
}

bool UIRomListNew::onInput(c2d::Input::Player *players) {

    if (ui->getUiMenu()->isVisible()) {
        return false;
    }

    unsigned int keys = players[0].keys;

    if (keys & Input::Right) {
        rom_index++;
        if (rom_index > 4) {
            rom_items_layer->move(-rom_item_size.x - 32, 0);
        }
    }

    if (keys & EV_QUIT) {
        ui->done = true;
    }

    return true;
}

void UIRomListNew::onDraw(c2d::Transform &transform) {
    RectangleShape::onDraw(transform);
    highlight->setPosition(rom_items.at((unsigned int) rom_index)->getGlobalPosition());
}

UIRomListNew::~UIRomListNew() {
    printf("~UIRomListNew\n");
}
