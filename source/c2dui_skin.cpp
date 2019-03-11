//
// Created by cpasjuste on 23/11/16.
//

#include "c2dui.h"

using namespace c2d;
using namespace c2dui;

Skin::Skin(UIMain *u, const std::vector<Button> &btns) {

    ui = u;
    path = ui->getIo()->getDataWritePath() + "skin/";

    tex_title = new C2DTexture(path + "title.png");

    // load buttons textures
    buttons = btns;
    for (auto &button : buttons) {
        button.path = path + "buttons/" + std::to_string(button.id) + ".png";
    }

    // config file
    config = new config::Config("SKIN_CONFIG", path + "config.cfg");

    ///
    /// FONT
    ///
    config::Group font_group("FONT");
    font_group.addOption({"path", path + "default.ttf"});
    font_group.addOption({"offset", Vector2f{0, -3}});
    font_group.addOption({"filtering", 0});
    config->addGroup(font_group);

    ///
    /// HIGHLIGHT
    ///
    config::Group highlight = createRectangleShapeGroup(
            "HIGHLIGHT", {0, 0, 16, 16},
            Origin::Left, path + "highlight_bg.png", {100, 255, 255, 80}, {0, 255, 255, 80}, 10);
    config->addGroup(highlight);

    ///
    /// MESSAGEBOX
    ///
    config::Group mbox = createRectangleShapeGroup(
            "MESSAGEBOX", {ui->getSize().x / 2, ui->getSize().y / 2,
                           ui->getSize().x / 2, ui->getSize().y / 2},
            Origin::Center, path + "messagebox_bg.png", Color::GrayLight, Color::Red, 2);
    config->addGroup(mbox);

    ///
    /// ROM LIST (START)
    ///
    // rom list bg
    config::Group main = createRectangleShapeGroup(
            "MAIN", ui->getLocalBounds(), Origin::TopLeft,
            path + "romlist_bg.png", Color::GrayDark, Color::Yellow, 2);
    // rom list title + text
    FloatRect titleRect = {ui->getSize().x / 2, 160, ui->getSize().x / 2, 64};
    config::Group title = createRectangleShapeGroup(
            "TITLE", titleRect, Origin::Center,
            path + "romlist_title.png", Color::GrayDark, Color::GrayLight, 2);

    FloatRect titleTextRect = {titleRect.width / 2, titleRect.height / 2, titleRect.width - 64, 0};
    config::Group titleText = createTextGroup(
            "TEXT", ui->getFontSize(), titleTextRect, Origin::Center, Color::White, Color::Black, 2);
    title.addGroup(titleText);
    main.addGroup(title);

    config::Group romList = createRectangleShapeGroup(
            "ROM_LIST", {0, 0, 212, 240}, Origin::Left,
            path + "romlist.png", {255, 255, 255, 150}, Color::GrayLight, 2);
    config::Group romItem = createRectangleShapeGroup(
            "ROM_ITEM", {0, 0, 212, 240}, Origin::Left,
            path + "romlist_item.png", {255, 255, 255, 150}, Color::GrayLight, 2);
    romList.addGroup(romItem);
    main.addGroup(romList);

    config::Group romInfo = createRectangleShapeGroup(
            "ROM_INFO", {654, 378, 608, 322}, Origin::Left,
            path + "romlist_info.png", {150, 150, 150, 255}, {220, 0, 0, 255}, 2);
    config::Group romInfoText = createTextGroup(
            "TEXT", ui->getFontSize(), titleTextRect, Origin::TopLeft, Color::White, Color::Black, 1);
    romInfo.addGroup(romInfoText);
    main.addGroup(romInfo);

    config::Group previewBox = createRectangleShapeGroup(
            "ROM_IMAGE", {654, 378, 608, 322}, Origin::Left,
            path + "romlist_image.png", {150, 150, 150, 255}, {220, 0, 0, 255}, 2);
    config::Group previewBoxText = createTextGroup(
            "TEXT", ui->getFontSize(), titleTextRect, Origin::Center, Color::White, Color::Black, 1);
    previewBox.addGroup(previewBoxText);
    main.addGroup(previewBox);

    config->addGroup(main);
    ///
    /// ROM LIST (END)
    ///

    ///
    /// OPTIONS_MENU
    ///
    config::Group options_menu = createRectangleShapeGroup(
            "OPTIONS_MENU", ui->getLocalBounds(), Origin::TopLeft,
            path + "options_menu_bg.png", Color::GrayDark, Color::Yellow, 2);
    config::Group options_menu_title = createTextGroup(
            "TITLE_TEXT", ui->getFontSize(), titleTextRect, Origin::TopLeft, Color::White, Color::Black, 1);
    options_menu.addGroup(options_menu_title);
    config::Group options_menu_text = createTextGroup(
            "ITEMS_TEXT", ui->getFontSize(), titleTextRect, Origin::TopLeft, Color::White, Color::Black, 1);
    options_menu.addGroup(options_menu_text);
    config->addGroup(options_menu);

    if (!config->load()) {
        // file doesn't exist or is malformed, (re)create it
        config->save();
    }
    ///
    /// OPTIONS_MENU (END)
    ///

    ///
    /// load font from configuration
    ///
    font = new C2DFont();
    c2d::config::Group *fnt = config->getGroup("FONT");
    if (!font->loadFromFile(fnt->getOption("path")->getString())) {
        font_available = false;
        font = c2d_renderer->getFont();
    } else {
        font->setFilter((Texture::Filter) fnt->getOption("filtering")->getInteger());
        font->setOffset(fnt->getOption("offset")->getVector2f());
    }
}

config::Config *Skin::getConfig() {
    return config;
}

config::Group Skin::createRectangleShapeGroup(const std::string &name,
                                              const c2d::FloatRect &rect,
                                              const c2d::Origin &origin,
                                              const std::string &texture,
                                              const c2d::Color &color,
                                              const c2d::Color &outlineColor, int outlineSize) {
    config::Group group(name);
    group.addOption({"texture", texture});
    group.addOption({"color", color});
    group.addOption({"outline_color", outlineColor});
    group.addOption({"outline_size", outlineSize});
    group.addOption({"rectangle", rect});
    group.addOption({"origin", (int) origin});
    return group;
}

Skin::RectangleShapeGroup Skin::getRectangleShape(const std::vector<std::string> &tree) {

    RectangleShapeGroup rectangleShapeGroup{};

    c2d::config::Group *group = config->getGroup(tree[0]);
    if (!group) {
        printf("Skin::getRectangleShape: config group not found: \"%s\"\n", tree[0].c_str());
        return rectangleShapeGroup;
    }

    if (tree.size() > 1) {
        for (unsigned int i = 1; i < tree.size(); i++) {
            group = group->getGroup(tree[i]);
            if (!group) {
                printf("Skin::getRectangleShape: config group not found: \"%s\"\n", tree[i].c_str());
                return rectangleShapeGroup;
            }
        }
    }

    rectangleShapeGroup.rect = group->getOption("rectangle")->getFloatRect();
    rectangleShapeGroup.origin = (Origin) group->getOption("origin")->getInteger();
    rectangleShapeGroup.texture = group->getOption("texture")->getString();
    rectangleShapeGroup.color = group->getOption("color")->getColor();
    rectangleShapeGroup.outlineColor = group->getOption("outline_color")->getColor();
    rectangleShapeGroup.outlineSize = group->getOption("outline_size")->getInteger();
    rectangleShapeGroup.available = true;

    return rectangleShapeGroup;
}

void Skin::loadRectangleShape(c2d::RectangleShape *shape, const std::vector<std::string> &tree) {

    RectangleShapeGroup rectangleShapeGroup = getRectangleShape(tree);
    if (!rectangleShapeGroup.available) {
        return;
    }

    shape->setOrigin(rectangleShapeGroup.origin);
    if (rectangleShapeGroup.rect.width > 0 && rectangleShapeGroup.rect.height > 0) {
        shape->setPosition(rectangleShapeGroup.rect.left, rectangleShapeGroup.rect.top);
        shape->setSize(rectangleShapeGroup.rect.width, rectangleShapeGroup.rect.height);
    }

    std::string bg_path = rectangleShapeGroup.texture;
    if (ui->getIo()->exist(bg_path)) {
        auto *tex = new C2DTexture(bg_path);
        tex->setScale(rectangleShapeGroup.rect.width / tex->getSize().x,
                      rectangleShapeGroup.rect.height / tex->getSize().y);
        shape->add(tex);
    }
    shape->setFillColor(rectangleShapeGroup.color);
    shape->setOutlineColor(rectangleShapeGroup.outlineColor);
    shape->setOutlineThickness(rectangleShapeGroup.outlineSize);
}

config::Group Skin::createTextGroup(const std::string &name, int size, const c2d::FloatRect &rect,
                                    const c2d::Origin &origin, const c2d::Color &color,
                                    const c2d::Color &outlineColor, int outlineSize) {
    config::Group group(name);
    group.addOption({"string", ""});
    group.addOption({"size", size});
    group.addOption({"color", color});
    group.addOption({"outline_color", outlineColor});
    group.addOption({"outline_size", outlineSize});
    group.addOption({"rectangle", rect});
    group.addOption({"origin", (int) origin});
    return group;
}

Skin::TextGroup Skin::getText(const std::vector<std::string> &tree) {

    TextGroup textGroup{};

    c2d::config::Group *group = config->getGroup(tree[0]);
    if (!group) {
        printf("Skin::getText: config group not found: \"%s\"\n", tree[0].c_str());
        return textGroup;
    }

    if (tree.size() > 1) {
        for (unsigned int i = 1; i < tree.size(); i++) {
            group = group->getGroup(tree[i]);
            if (!group) {
                printf("Skin::getText: config group not found: \"%s\"\n", tree[i].c_str());
                return textGroup;
            }
        }
    }

    std::string str = group->getOption("string")->getString();
    if (!str.empty()) {
        textGroup.text = str;
    }
    textGroup.size = (unsigned int) group->getOption("size")->getInteger();
    textGroup.color = group->getOption("color")->getColor();
    textGroup.outlineColor = group->getOption("outline_color")->getColor();
    textGroup.outlineSize = group->getOption("outline_size")->getInteger();
    textGroup.origin = (Origin) group->getOption("origin")->getInteger();
    textGroup.rect = group->getOption("rectangle")->getFloatRect();
    textGroup.available = true;

    return textGroup;
}

void Skin::loadText(c2d::Text *text, const std::vector<std::string> &tree) {

    TextGroup textGroup = getText(tree);
    if (!textGroup.available) {
        return;
    }

    if (!textGroup.text.empty()) {
        text->setString(textGroup.text);
    }
    text->setCharacterSize(textGroup.size);
    text->setFillColor(textGroup.color);
    text->setOutlineColor(textGroup.outlineColor);
    text->setOutlineThickness(textGroup.outlineSize);
    text->setOrigin(textGroup.origin);
    text->setPosition(textGroup.rect.left, textGroup.rect.top);
    if (textGroup.rect.width > 0) {
        text->setWidth(textGroup.rect.width);
    }
}

Skin::Button *Skin::getButton(int id) {

    for (unsigned int i = 0; i < buttons.size(); i++) {
        if (buttons[i].id == id) {
            return &buttons[i];
        }
    }
    return nullptr;
}

Skin::~Skin() {

    buttons.clear();

    if (font && font_available) {
        delete (font);
    }

    if (config) {
        delete (config);
    }
}


