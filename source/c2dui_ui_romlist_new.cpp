//
// Created by cpasjuste on 22/11/16.
//
#include <algorithm>

#include "c2dui.h"

#define ROM_ITEMS_OUTLINE 5
#define ROM_ITEMS_MARGIN 32

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
        setOutlineThickness(ROM_ITEMS_OUTLINE);
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

    void onDraw(Transform &transform, bool draw = true) override {

        // draw from -1 item to screen size + 1 item
        FloatRect bounds = getGlobalBounds();
        if (bounds.left > -(getSize().x + ROM_ITEMS_MARGIN) &&
            bounds.left < ui->getSize().x + getSize().x + ROM_ITEMS_MARGIN) {
            if (!previewLoaded) {
                //printf("loading preview: %s\n", rom->name.c_str());
                setPreview(ui->getUiRomList()->getPreviewTexture(rom, false));
                previewLoaded = true;
            }
            Shape::onDraw(transform, draw);
        } else {
            if (previewLoaded) {
                //printf("unloading preview: %s\n", rom->name.c_str());
                setPreview(nullptr);
                previewLoaded = false;
            }
            Shape::onDraw(transform, false);
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
        setOutlineThickness(ROM_ITEMS_OUTLINE);
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
                                    rom_item_size.x + ROM_ITEMS_OUTLINE * 2,
                                    rom_item_size.y + ROM_ITEMS_OUTLINE * 2});
    add(highlight);

    updateRomList();
}

RomList::Rom *UIRomListNew::getSelection() {
    return nullptr;
}

void UIRomListNew::updateRomList() {

    filterRomList();

    delete (rom_items_layer);
    rom_items.clear();
    rom_items_layer = new Rectangle({0, getSize().y / 2, 16, 16});
    rom_items_layer_tween = new TweenPosition({0, 0}, {0, 0}, 0.25f);
    rom_items_layer_tween->setState(TweenState::Stopped);
    rom_items_layer->add(rom_items_layer_tween);
    add(rom_items_layer);

    float start_x = rom_item_size.x / 2 + 32;

    for (unsigned int i = 0; i < roms.size(); i++) {
        UIRomItem *item = new UIRomItem(ui, roms[i], {0, 0, rom_item_size.x, rom_item_size.y});
        item->setOrigin(Origin::Center);
        item->setPosition(((rom_item_size.x + ROM_ITEMS_MARGIN) * i) + start_x, rom_items_layer->getSize().y / 2);
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

    if (keys & Input::Left) {
        rom_index--;
        if (rom_index < 0) {
            rom_index = (int) (roms.size() - 1);
            rom_items_layer->setPosition(rom_index * -(rom_item_size.x - ROM_ITEMS_MARGIN), getSize().y / 2);
        } else if (highlight->getPosition().x < rom_item_size.x) {
            Vector2f pos = rom_items_layer->getPosition();
            Vector2f to = {rom_index * -(rom_item_size.x + ROM_ITEMS_MARGIN), pos.y};
            rom_items_layer_tween->setFromTo(pos, to, ((float) ui->getInput()->getRepeatDelay() * 0.001f) / 2);
            rom_items_layer_tween->play(TweenDirection::Forward, true);
        } else {
            // TODO:
            //rom_items_layer->setPosition(rom_index * -(rom_item_size.x - ROM_ITEMS_MARGIN), getSize().y / 2);
            printf("CRAP\n");
        }
    } else if (keys & Input::Right) {
        rom_index++;
        if (rom_index >= (int) roms.size()) {
            rom_index = 0;
            rom_items_layer->setPosition(0, getSize().y / 2);
        } else if (highlight->getPosition().x > (rom_item_size.x / 2 + rom_item_size.x) * 3) {
            Vector2f pos = rom_items_layer->getPosition();
            Vector2f to = {(rom_index - 4) * -(rom_item_size.x + ROM_ITEMS_MARGIN), pos.y};
            rom_items_layer_tween->setFromTo(pos, to, ((float) ui->getInput()->getRepeatDelay() * 0.001f) / 2);
            rom_items_layer_tween->play(TweenDirection::Forward, true);
        } else {
            rom_items_layer->setPosition(0, getSize().y / 2);
        }
    }

    if (keys & EV_QUIT) {
        ui->done = true;
    }

    return true;
}

void UIRomListNew::onDraw(c2d::Transform &transform, bool draw) {
    RectangleShape::onDraw(transform, draw);
    if (!rom_items.empty()) {
        highlight->setVisibility(Visibility::Visible);
        highlight->setPosition(rom_items.at((unsigned int) rom_index)->getGlobalPosition());
    } else {
        highlight->setVisibility(Visibility::Hidden);
    }
}

UIRomListNew::~UIRomListNew() {
    printf("~UIRomListNew\n");
}
