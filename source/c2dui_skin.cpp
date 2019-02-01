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

    // font
    config->addOption({"font", path + "default.ttf"});

    // highlight
    config::Group highlight = createRectangleShapeGroup(
            "HIGHLIGHT", path + "highlight_bg.png", {0, 255, 255, 80}, Color::Cyan, 5);
    config->addGroup(highlight);

    // rom list
    config::Group rom_list("ROM_LIST");
    config::Group bg = createRectangleShapeGroup(
            "BACKGROUND", path + "romlist_bg.png", Color::GrayDark, Color::Yellow, 2);
    rom_list.addGroup(bg);
    config::Group title = createRectangleShapeGroup(
            "TITLE", path + "romlist_title.png", Color::GrayDark, Color::GrayLight, 2);
    rom_list.addGroup(title);
    config->addGroup(rom_list);

    if (!config->load()) {
        // file doesn't exist or is malformed, (re)create it
        config->save();
    }

    font = new C2DFont();
    if (!font->loadFromFile(config->getOption("font")->getString())) {
        font_available = false;
        font = c2d_renderer->getFont();
    }
    font->setFilter(Texture::Filter::Point);
    font->setOffset({0, -3});

}

config::Config *Skin::getConfig() {
    return config;
}

config::Group Skin::createRectangleShapeGroup(const std::string &name,
                                              const std::string &texture,
                                              const c2d::Color &color,
                                              const c2d::Color &outlineColor, int outlineSize) {
    config::Group group(name);
    group.addOption({"texture", texture});
    group.addOption({"color", color});
    group.addOption({"outline_color", outlineColor});
    group.addOption({"outline_size", outlineSize});
    return group;
}

void Skin::loadRectangleShape(c2d::RectangleShape *shape, const std::string &groupName) {

    c2d::config::Group *bg = nullptr;
    c2d::config::Group *group = config->getGroup(groupName);
    if (!group) {
        printf("Skin::loadRectangleShape: config group \"%s\" doesn't exist\n", groupName.c_str());
        return;
    }

    bg = group->getGroup("BACKGROUND");
    if (!bg) {
        bg = group;
    }

    std::string bg_path = bg->getOption("texture")->getString();
    if (ui->getIo()->exist(bg_path)) {
        shape->add(new C2DTexture(bg_path));
    } else {
        shape->setFillColor(bg->getOption("color")->getColor());
        shape->setOutlineColor(bg->getOption("outline_color")->getColor());
        int thickness = bg->getOption("outline_size")->getInteger();
        shape->setOutlineThickness(thickness);
        shape->setPosition(thickness, thickness);
        shape->setSize(shape->getSize().x - thickness * 2, shape->getSize().y - thickness * 2);
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

