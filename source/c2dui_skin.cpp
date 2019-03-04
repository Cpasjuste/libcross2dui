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

    config::Group romList("ROM_LIST");
    romList.addOption({"position_y", ui->getSize().y / 2});
    config::Group romItem = createRectangleShapeGroup(
            "ROM_ITEM", {0, 0, 212, 240}, Origin::Left,
            path + "romlist_item.png", {255, 255, 255, 150}, Color::GrayLight, 5);
    romList.addGroup(romItem);
    main.addGroup(romList);

    config->addGroup(main);
    ///
    /// ROM LIST (END)
    ///

    if (!config->load()) {
        // file doesn't exist or is malformed, (re)create it
        config->save();
    }

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

void Skin::loadRectangleShape(c2d::RectangleShape *shape, const std::vector<std::string> &tree) {

    c2d::config::Group *group = config->getGroup(tree[0]);
    if (!group) {
        printf("Skin::loadRectangleShape: config group not found: \"%s\"\n", tree[0].c_str());
        return;
    }

    if (tree.size() > 1) {
        for (unsigned int i = 1; i < tree.size(); i++) {
            group = group->getGroup(tree[i]);
            if (!group) {
                printf("Skin::loadRectangleShape: config group not found: \"%s\"\n", tree[i].c_str());
                return;
            }
        }
    }

    FloatRect rect = group->getOption("rectangle")->getFloatRect();
    if (rect.width > 0 && rect.height > 0) {
        shape->setPosition(rect.left, rect.top);
        shape->setSize(rect.width, rect.height);
    }

    shape->setOrigin((Origin) group->getOption("origin")->getInteger());

    std::string bg_path = group->getOption("texture")->getString();
    if (ui->getIo()->exist(bg_path)) {
        shape->add(new C2DTexture(bg_path));
    } else {
        shape->setFillColor(group->getOption("color")->getColor());
        shape->setOutlineColor(group->getOption("outline_color")->getColor());
        int thickness = group->getOption("outline_size")->getInteger();
        shape->setOutlineThickness(thickness);
    }
}

config::Group Skin::createTextGroup(const std::string &name, int size, const c2d::FloatRect &rect,
                                    const c2d::Origin &origin, const c2d::Color &color,
                                    const c2d::Color &outlineColor, int outlineSize) {
    config::Group group(name);
    group.addOption({"size", size});
    group.addOption({"color", color});
    group.addOption({"outline_color", outlineColor});
    group.addOption({"outline_size", outlineSize});
    group.addOption({"rectangle", rect});
    group.addOption({"origin", (int) origin});
    return group;
}

void Skin::loadText(c2d::Text *text, const std::vector<std::string> &tree) {

    c2d::config::Group *group = config->getGroup(tree[0]);
    if (!group) {
        printf("Skin::loadRectangleShape: config group not found: \"%s\"\n", tree[0].c_str());
        return;
    }

    if (tree.size() > 1) {
        for (unsigned int i = 1; i < tree.size(); i++) {
            group = group->getGroup(tree[i]);
            if (!group) {
                printf("Skin::loadRectangleShape: config group not found: \"%s\"\n", tree[i].c_str());
                return;
            }
        }
    }

    text->setCharacterSize((unsigned int) group->getOption("size")->getInteger());
    text->setFillColor(group->getOption("color")->getColor());
    text->setOutlineColor(group->getOption("outline_color")->getColor());
    text->setOutlineThickness(group->getOption("outline_size")->getInteger());
    text->setOrigin((Origin) group->getOption("origin")->getInteger());
    FloatRect rect = group->getOption("rectangle")->getFloatRect();
    text->setPosition(rect.left, rect.top);
    if (rect.width > 0) {
        text->setWidth(rect.width);
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

