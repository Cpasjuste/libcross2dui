//
// Created by cpasjuste on 22/11/16.
//

#include "c2dui.h"

#define ROM_ITEM_OUTLINE 5
#define ROM_ITEM_MARGIN 32

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
        setFillColor({255, 255, 255, 150});
        setOutlineThickness(ROM_ITEM_OUTLINE);
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
        if (bounds.left > -(getSize().x + ROM_ITEM_MARGIN) &&
            bounds.left < ui->getSize().x + getSize().x + ROM_ITEM_MARGIN) {
            if (!previewLoaded) {
                setPreview(ui->getUiRomList()->getPreviewTexture(rom, false));
                previewLoaded = true;
            }
            Shape::onDraw(transform, draw);
        } else {
            if (previewLoaded) {
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

    explicit UIRomHighlight(const Vector2f &size) : RoundedRectangleShape(size) {
        setOrigin(Origin::Left);
        setFillColor(Color::Cyan);
        setAlpha(100);
        setOutlineThickness(ROM_ITEM_OUTLINE);
        setOutlineColor(Color::Cyan);
        tween = new TweenPosition({0, 0}, {0, 0}, 0.25f);
        add(tween);
    }

    void tweenPosition(Vector2f pos, int ms) {
        Vector2i newPos = {(int) pos.x, (int) pos.y};
        if (currentPosition.x != newPos.x || currentPosition.y != newPos.y) {
            currentPosition = newPos;
            tween->setFromTo(getPosition(), {(float) newPos.x, (float) newPos.y}, (float) ms * 0.001f);
            tween->play(TweenDirection::Forward, true);
        }
    }

    Vector2i currentPosition;
    TweenPosition *tween;
};

UIRomListNew::UIRomListNew(UIMain *u, RomList *romList, const c2d::Vector2f &size)
        : UIRomList(u, romList, size) {

    printf("UIRomListNew\n");

    ui = u;
    rom_list = romList;

    add(new C2DTexture("skin/romlist_bg.png"));

    rom_item_size = {getSize().x / 6, getSize().y / 3};
    highlight = new UIRomHighlight({rom_item_size.x + ROM_ITEM_OUTLINE * 2,
                                    rom_item_size.y + ROM_ITEM_OUTLINE * 2});
    add(highlight);

    RoundedRectangleShape *titleRect = new RoundedRectangleShape({512, 64});
    titleRect->setFillColor(Color::GrayDark);
    titleRect->setOrigin(Origin::Center);
    titleRect->setPosition(getSize().x / 2, 160);
    title = new Text("NO ROM INFORMATION", (unsigned int) ui->getFontSize(), ui->getFont());
    title->setOutlineThickness(1);
    title->setOrigin(Origin::Center);
    title->setPosition(titleRect->getSize().x / 2, titleRect->getSize().y / 2);
    titleRect->add(title);
    add(titleRect);

    updateRomList();
}

void UIRomListNew::updateRomList() {

    filterRomList();

    rom_items.clear();
    delete (rom_items_layer);
    rom_items_layer = new Rectangle({0, getSize().y / 2, 16, 16});
    rom_items_layer->setOrigin(Origin::Left);
    rom_items_layer_tween = new TweenPosition({0, 0}, {0, 0}, 0.25f);
    rom_items_layer_tween->setState(TweenState::Stopped);
    rom_items_layer->add(rom_items_layer_tween);
    add(rom_items_layer);

    float start_x = ROM_ITEM_MARGIN;

    for (unsigned int i = 0; i < roms.size(); i++) {
        UIRomItem *item = new UIRomItem(ui, roms[i], {0, 0, rom_item_size.x, rom_item_size.y});
        item->setOrigin(Origin::Left);
        item->setPosition(((rom_item_size.x + ROM_ITEM_MARGIN) * i) + start_x, rom_items_layer->getSize().y / 2);
        rom_items_layer->add(item);
        rom_items.push_back(item);
    }

    rom_index = 0;
    highlight->tweenPosition(getHighlightPosition(0), 1);
    highlight->setVisibility(roms.empty() ? Visibility::Hidden : Visibility::Visible);
}

RomList::Rom *UIRomListNew::getSelection() {
    return roms.at((size_t) rom_index);
}

c2d::Vector2f UIRomListNew::getHighlightPosition(int index) {
    return {ROM_ITEM_MARGIN + (index * rom_item_size.x)
            + (index * ROM_ITEM_MARGIN) - ROM_ITEM_OUTLINE, getSize().y / 2};
}

bool UIRomListNew::onInput(c2d::Input::Player *players) {

    if (ui->getUiMenu()->isVisible()) {
        return false;
    }

    unsigned int keys = players[0].keys;

    if (keys & Input::Left) {
        if (roms.empty()) {
            return true;
        }
        rom_index--;
        if (rom_index < 0) {
            rom_index = (int) (roms.size() - 1);
            int index = roms.size() < 5 ? (int) roms.size() - 1 : 4;
            highlight_index = index;
            if (roms.size() > 4) {
                rom_items_layer->setPosition(
                        -getHighlightPosition(rom_index - index).x + ROM_ITEM_MARGIN - ROM_ITEM_OUTLINE,
                        getSize().y / 2);
                rom_items_layer_tween->reset();
                rom_items_layer_tween->setState(TweenState::Stopped);
                Vector2f pos = rom_items_layer->getPosition();
                rom_items_layer_tween->setFromTo(pos, pos, ((float) ui->getInput()->getRepeatDelay() * 0.001f) / 3);
            }
            int ms = ui->getInput()->getRepeatDelay() / 3;
            highlight->tweenPosition(getHighlightPosition(highlight_index), ms);
        } else if (highlight_index != 0) {
            highlight_index--;
            int ms = ui->getInput()->getRepeatDelay() / 3;
            highlight->tweenPosition(getHighlightPosition(highlight_index), ms);
        } else {
            Vector2f pos = rom_items_layer->getPosition();
            Vector2f to = {rom_items_layer_tween->getTo().x + (ROM_ITEM_MARGIN + rom_item_size.x), pos.y};
            rom_items_layer_tween->setFromTo(pos, to, ((float) ui->getInput()->getRepeatDelay() * 0.001f) / 3);
            rom_items_layer_tween->play(TweenDirection::Forward, true);
        }
        title->setString(getSelection()->name);
    } else if (keys & Input::Right) {
        if (roms.empty()) {
            return true;
        }
        rom_index++;
        if (rom_index >= (int) roms.size()) {
            rom_index = 0;
            highlight_index = 0;
            rom_items_layer->setPosition(0, getSize().y / 2);
            rom_items_layer_tween->reset();
            rom_items_layer_tween->setState(TweenState::Stopped);
            Vector2f pos = rom_items_layer->getPosition();
            rom_items_layer_tween->setFromTo(pos, pos, ((float) ui->getInput()->getRepeatDelay() * 0.001f) / 3);
            int ms = ui->getInput()->getRepeatDelay() / 3;
            highlight->tweenPosition(getHighlightPosition(highlight_index), ms);
        } else if (highlight_index != 4) {
            highlight_index++;
            int ms = ui->getInput()->getRepeatDelay() / 3;
            highlight->tweenPosition(getHighlightPosition(highlight_index), ms);
        } else {
            Vector2f pos = rom_items_layer->getPosition();
            Vector2f to = {rom_items_layer_tween->getTo().x - (ROM_ITEM_MARGIN + rom_item_size.x), pos.y};
            rom_items_layer_tween->setFromTo(pos, to, ((float) ui->getInput()->getRepeatDelay() * 0.001f) / 3);
            rom_items_layer_tween->play(TweenDirection::Forward, true);
        }
        title->setString(getSelection()->name);
    } else if (keys & Input::Key::Fire1) {
        RomList::Rom *rom = getSelection();
        if (rom && rom->state != RomList::RomState::MISSING) {
            ui->getConfig()->load(rom);
            ui->getUiEmu()->load(rom);
            return true;
        }
    }

    if (keys & EV_QUIT) {
        ui->done = true;
    }

    return true;
}

void UIRomListNew::onDraw(c2d::Transform &transform, bool draw) {
    RectangleShape::onDraw(transform, draw);
}

UIRomListNew::~UIRomListNew() {
    printf("~UIRomListNew\n");
}
