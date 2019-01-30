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
        setOrigin(Origin::Left);
        setFillColor(Color::Cyan);
        setAlpha(100);
        setOutlineThickness(ROM_ITEM_OUTLINE);
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
                                    rom_item_size.x + ROM_ITEM_OUTLINE * 2,
                                    rom_item_size.y + ROM_ITEM_OUTLINE * 2});
    highlight_tween = new TweenPosition({0, 0}, {0, 0}, 0.25f);
    highlight_tween->setState(TweenState::Stopped);
    highlight->add(highlight_tween);
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

    delete (rom_items_layer);
    rom_items.clear();
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

    highlight->setPosition(getHighlightPosition(0));
    highlight->setVisibility(roms.empty() ? Visibility::Hidden : Visibility::Visible);
}

RomList::Rom *UIRomListNew::getSelection() {
    return roms.at((size_t) rom_index);
}

c2d::Vector2f UIRomListNew::getHighlightPosition(int index) {
    int page = index / 5;
    int index_start = page * 5;
    int item_pos = index - index_start;
    return {ROM_ITEM_MARGIN + (item_pos * rom_item_size.x)
            + (item_pos * ROM_ITEM_MARGIN) - ROM_ITEM_OUTLINE, getSize().y / 2};
}



void UIRomListNew::updateHighlight() {

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
            Vector2f pos = rom_items_layer->getPosition();
            rom_items_layer->setPosition((rom_index - 4) * -(rom_item_size.x + ROM_ITEM_MARGIN), pos.y);
            rom_items_layer_tween->reset();
            rom_items_layer_tween->setState(TweenState::Stopped);
            highlight->setPosition(getHighlightPosition(4));
            //} else  if (highlight->getPosition().x < rom_item_size.x) {
        } else if (getHighlightPosition(rom_index).x == getHighlightPosition(0).x /*rom_index > 4 && highlight->getPosition().x > getHighlightPosition(4).x*/) {
            //TODO
            Vector2f pos = rom_items_layer->getPosition();
            Vector2f to = {(rom_index/* - 4*/) * -(rom_item_size.x + ROM_ITEM_MARGIN), pos.y};
            rom_items_layer_tween->setFromTo(pos, to, ((float) ui->getInput()->getRepeatDelay() * 0.001f) / 3);
            rom_items_layer_tween->play(TweenDirection::Forward, true);
        } else {
            Vector2f to = getHighlightPosition(rom_index);
            if (highlight->getPosition().x != getHighlightPosition(0).x) {
                highlight_tween->setFromTo(highlight->getPosition(), to,
                                           ((float) ui->getInput()->getRepeatDelay() * 0.001f) / 3);
                highlight_tween->play(TweenDirection::Forward, true);
            }
        }
        /*
        //updateHighlight();
        int page = rom_index / 5;
        int index_start = page * 5;
        int item_pos = rom_index - index_start;
        printf("index: %i, page: %i, index_start: %i, item_pos: %i\n", rom_index, page, index_start, item_pos);
        Vector2f to = getHighlightPosition(rom_index);
        if (item_pos >= 0) {
            highlight_tween->setFromTo(highlight->getPosition(), to,
                                       ((float) ui->getInput()->getRepeatDelay() * 0.001f) / 3);
            highlight_tween->play(TweenDirection::Forward, true);
        }
        */
        title->setString(getSelection()->name);
    } else if (keys & Input::Right) {
        rom_index++;
        if (rom_index >= (int) roms.size()) {
            rom_index = 0;
            rom_items_layer->setPosition(0, getSize().y / 2);
            rom_items_layer_tween->reset();
            rom_items_layer_tween->setState(TweenState::Stopped);
            highlight->setPosition(getHighlightPosition(0));
        } else if (rom_index > 4) {
            Vector2f pos = rom_items_layer->getPosition();
            Vector2f to = {(rom_index - 4) * -(rom_item_size.x + ROM_ITEM_MARGIN), pos.y};
            rom_items_layer_tween->setFromTo(pos, to, ((float) ui->getInput()->getRepeatDelay() * 0.001f) / 3);
            rom_items_layer_tween->play(TweenDirection::Forward, true);
        } else {
            Vector2f to = getHighlightPosition(rom_index);
            highlight_tween->setFromTo(highlight->getPosition(), to,
                                       ((float) ui->getInput()->getRepeatDelay() * 0.001f) / 3);
            highlight_tween->play(TweenDirection::Forward, true);
        }
        title->setString(getSelection()->name);
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
