//
// Created by cpasjuste on 23/11/16.
//

#include <minizip/unzip.h>

#include "c2dui.h"

using namespace c2d;
using namespace c2dui;

Skin::Skin(UIMain *u, const std::vector<Button> &btns) {

    ui = u;
    path = ui->getIo()->getDataWritePath() + "skins/";

    // extract config file from zipped skin but create a default one
    config = new config::Config("SKIN_CONFIG", path + "config.cfg");

    std::string skinName = ui->getConfig()->get(Option::GUI_SKIN)->getValueString() + std::string(".zip");
    int configLen = 0;
    char *configData = getZippedData(path + skinName, "config.cfg", &configLen);
    if (!configData) {
        ui->getConfig()->get(Option::GUI_SKIN)->setIndex(0);
        skinName = "default.zip";
        configData = getZippedData(path + skinName, "config.cfg", &configLen);
    }
    if (configData) {
        configData[configLen - 1] = '\0';
        path += skinName;
        useZippedSkin = true;
        printf("Skin: zipped skin found: %s\n", path.c_str());
    }

    // TODO: cleanup this
    // load buttons textures
    buttons = btns;
    for (auto &button : buttons) {
        button.path = ui->getIo()->getDataWritePath() + "skins/buttons/" + std::to_string(button.id) + ".png";
    }

    ///
    /// FONT
    ///
    config::Group font_group("FONT");
    font_group.addOption({"path", "default.ttf"});
    font_group.addOption({"offset", Vector2f{0, -3}});
    font_group.addOption({"filtering", 0});
    config->addGroup(font_group);

    ///
    /// HIGHLIGHT
    ///
    config::Group highlight = createRectangleShapeGroup(
            "HIGHLIGHT", {0, 0, 16, 16},
            Origin::Left, "highlight_bg.png", {100, 255, 255, 80}, {0, 255, 255, 80}, 10);
    config->addGroup(highlight);

    ///
    /// MESSAGEBOX
    ///
    config::Group mbox = createRectangleShapeGroup(
            "MESSAGEBOX", {ui->getSize().x / 2, ui->getSize().y / 2,
                           ui->getSize().x / 2, ui->getSize().y / 2},
            Origin::Center, "messagebox_bg.png", Color::GrayLight, Color::Red, 2);
    config->addGroup(mbox);

    ///
    /// MAIN (ROM LIST)
    ///
    config::Group main = createRectangleShapeGroup(
            "MAIN", ui->getLocalBounds(), Origin::TopLeft,
            "romlist_bg.png", Color::GrayDark, Color::Yellow, 2);
    // rom list title + text
    FloatRect titleRect = {ui->getSize().x / 2, 160, ui->getSize().x / 2, 64};
    config::Group title = createRectangleShapeGroup(
            "TITLE", titleRect, Origin::Center,
            "romlist_title.png", Color::GrayDark, Color::GrayLight, 2);
    FloatRect titleTextRect = {titleRect.width / 2, titleRect.height / 2, titleRect.width - 64, 0};
    config::Group titleText = createTextGroup(
            "TEXT", ui->getFontSize(), titleTextRect, Origin::Center, Color::White, Color::Black, 2);
    title.addGroup(titleText);
    main.addGroup(title);
    config::Group help = createRectangleShapeGroup(
            "HELP", titleRect, Origin::Center,
            "romlist_help.png", Color::GrayDark, Color::GrayLight, 2);
    main.addGroup(help);
    config::Group romList = createRectangleShapeGroup(
            "ROM_LIST", {0, 0, 212, 240}, Origin::Left,
            "romlist.png", {255, 255, 255, 150}, Color::GrayLight, 2);
    config::Group romItem = createTextGroup(
            "TEXT", ui->getFontSize(), titleTextRect, Origin::TopLeft, Color::White, Color::Black, 1);
    romList.addGroup(romItem);
    main.addGroup(romList);
    config::Group romInfo = createRectangleShapeGroup(
            "ROM_INFO", {654, 378, 608, 322}, Origin::Left,
            "romlist_info.png", {150, 150, 150, 255}, {220, 0, 0, 255}, 2);
    config::Group romInfoText = createTextGroup(
            "TEXT", ui->getFontSize(), titleTextRect, Origin::TopLeft, Color::White, Color::Black, 1);
    romInfo.addGroup(romInfoText);
    main.addGroup(romInfo);
    config::Group previewBox = createRectangleShapeGroup(
            "ROM_IMAGE", {654, 378, 608, 322}, Origin::Left,
            "romlist_image.png", {150, 150, 150, 255}, {220, 0, 0, 255}, 2);
    config::Group previewBoxText = createTextGroup(
            "TEXT", ui->getFontSize(), titleTextRect, Origin::Center, Color::White, Color::Black, 1);
    previewBox.addGroup(previewBoxText);
    main.addGroup(previewBox);
    config->addGroup(main);
    ///
    /// MAIN (ROM LIST) (END)
    ///

    ///
    /// OPTIONS_MENU
    ///
    config::Group options_menu = createRectangleShapeGroup(
            "OPTIONS_MENU", ui->getLocalBounds(), Origin::TopLeft,
            "options_menu_bg.png", Color::GrayDark, Color::Yellow, 2);
    config::Group options_menu_title = createTextGroup(
            "TITLE_TEXT", ui->getFontSize(), titleTextRect, Origin::TopLeft, Color::White, Color::Black, 1);
    options_menu.addGroup(options_menu_title);
    config::Group options_menu_text = createTextGroup(
            "ITEMS_TEXT", ui->getFontSize(), titleTextRect, Origin::TopLeft, Color::White, Color::Black, 1);
    options_menu.addGroup(options_menu_text);
    config->addGroup(options_menu);
    ///
    /// OPTIONS_MENU (END)
    ///

    ///
    /// STATES_MENU
    ///
    config::Group states_menu = createRectangleShapeGroup(
            "STATES_MENU", ui->getLocalBounds(), Origin::TopLeft,
            "states_menu_bg.png", Color::GrayDark, Color::Yellow, 2);
    config::Group states_menu_title = createTextGroup(
            "TITLE_TEXT", ui->getFontSize(), titleTextRect, Origin::TopLeft, Color::White, Color::Black, 1);
    states_menu.addGroup(states_menu_title);
    config::Group states_item = createRectangleShapeGroup(
            "STATES_ITEM", ui->getLocalBounds(), Origin::TopLeft,
            "states_item_bg.png", Color::GrayDark, Color::Yellow, 2);
    config::Group states_item_text = createTextGroup(
            "STATES_TEXT", ui->getFontSize(), titleTextRect, Origin::TopLeft, Color::White, Color::Black, 1);
    states_item.addGroup(states_item_text);
    states_menu.addGroup(states_item);
    config->addGroup(states_menu);
    ///
    /// STATES_MENU (END
    ///

    // LOAD/SAVE
    if (useZippedSkin) {
        config->loadFromString(configData);
        free(configData);
    } else {
        if (!config->load()) {
            // file doesn't exist or is malformed, (re)create it
            config->save();
        }
    }

    ///
    /// load skin font from loaded configuration
    ///
    font_available = false;
    font = new C2DFont();
    c2d::config::Group *fntGroup = config->getGroup("FONT");
    if (useZippedSkin) {
        int size = 0;
        font_data = getZippedData(path, fntGroup->getOption("path")->getString(), &size);
        if (font_data) {
            if (font->loadFromMemory(font_data, (size_t) size)) {
                font_available = true;
            }
        }
    } else {
        std::string fontPath = path + fntGroup->getOption("path")->getString();
        printf("font path: %s\n", fontPath.c_str());
        if (font->loadFromFile(fontPath)) {
            font_available = true;
        }
    }
    if (!font_available) {
        delete (font);
        font = ui->getFont();
    }
    font->setFilter((Texture::Filter) fntGroup->getOption("filtering")->getInteger());
    font->setOffset(fntGroup->getOption("offset")->getVector2f());
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

    config::Option *option;
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

    option = group->getOption("texture");
    if (option) {
        rectangleShapeGroup.texture = option->getString();
    }
    option = group->getOption("color");
    if (option) {
        rectangleShapeGroup.color = option->getColor();
    }
    option = group->getOption("outline_color");
    if (option) {
        rectangleShapeGroup.outlineColor = option->getColor();
    }
    option = group->getOption("outline_size");
    if (option) {
        rectangleShapeGroup.outlineSize = option->getInteger();
    }
    option = group->getOption("rectangle");
    if (option) {
        rectangleShapeGroup.rect = option->getFloatRect();
    }
    option = group->getOption("origin");
    if (option) {
        rectangleShapeGroup.origin = (Origin) option->getInteger();
    }

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

    C2DTexture *tex = nullptr;
    if (useZippedSkin) {
        int size = 0;
        char *data = getZippedData(path, rectangleShapeGroup.texture, &size);
        if (data) {
            tex = new C2DTexture((const unsigned char *) data, size);
        }
    } else {
        std::string bg_path = path + rectangleShapeGroup.texture;
        if (ui->getIo()->exist(bg_path)) {
            tex = new C2DTexture(bg_path);
        }
    }

    if (tex && tex->available) {
        tex->setScale(rectangleShapeGroup.rect.width / tex->getSize().x,
                      rectangleShapeGroup.rect.height / tex->getSize().y);
        tex->setFillColor(rectangleShapeGroup.color);
        tex->setOutlineColor(rectangleShapeGroup.outlineColor);
        tex->setOutlineThickness(rectangleShapeGroup.outlineSize);
        shape->setFillColor(Color::Transparent);
        shape->setOutlineColor(Color::Transparent);
        shape->setOutlineThickness(0);
        shape->add(tex);
    } else {
        if (tex) {
            delete (tex);
        }
        shape->setFillColor(rectangleShapeGroup.color);
        shape->setOutlineColor(rectangleShapeGroup.outlineColor);
        shape->setOutlineThickness(rectangleShapeGroup.outlineSize);
    }
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

    config::Option *option;
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

    option = group->getOption("string");
    if (option) {
        textGroup.text = option->getString();
    }
    option = group->getOption("size");
    if (option) {
        textGroup.size = (unsigned int) option->getInteger();
    }
    option = group->getOption("color");
    if (option) {
        textGroup.color = option->getColor();
    }
    option = group->getOption("outline_color");
    if (option) {
        textGroup.outlineColor = option->getColor();
    }
    option = group->getOption("outline_size");
    if (option) {
        textGroup.outlineSize = option->getInteger();
    }
    option = group->getOption("origin");
    if (option) {
        textGroup.origin = (Origin) option->getInteger();
    }
    option = group->getOption("rectangle");
    if (option) {
        textGroup.rect = option->getFloatRect();
    }

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

c2d::Font *Skin::getFont() {
    return font;
}

char *Skin::getZippedData(const std::string &path, const std::string &name, int *size) {

    char *data = nullptr;

    unzFile zip = unzOpen(path.c_str());
    if (!zip) {
        return data;
    }

    if (unzGoToFirstFile(zip) == UNZ_OK) {
        do {
            if (unzOpenCurrentFile(zip) == UNZ_OK) {
                unz_file_info fileInfo;
                memset(&fileInfo, 0, sizeof(unz_file_info));
                if (unzGetCurrentFileInfo(zip, &fileInfo, nullptr, 0, nullptr, 0, nullptr, 0) == UNZ_OK) {
                    char *zipFileName = (char *) malloc(fileInfo.size_filename + 1);
                    unzGetCurrentFileInfo(zip, &fileInfo, zipFileName, fileInfo.size_filename + 1,
                                          nullptr, 0, nullptr, 0);
                    zipFileName[fileInfo.size_filename] = '\0';
                    if (name == zipFileName) {
                        data = (char *) malloc(fileInfo.uncompressed_size);
                        if (size) {
                            *size = (int) fileInfo.uncompressed_size;
                        }
                        printf("getZippedData: %s, %s, %i\n", path.c_str(), name.c_str(), size ? *size : 0);
                        unzReadCurrentFile(zip, data, (unsigned int) fileInfo.uncompressed_size);
                        free(zipFileName);
                        unzClose(zip);
                        return data;
                    }
                    free(zipFileName);
                }
                unzCloseCurrentFile(zip);
            }
        } while (unzGoToNextFile(zip) == UNZ_OK);
    }

    unzClose(zip);

    return data;
}

Skin::~Skin() {

    buttons.clear();

    if (font) {
        delete (font);
    }

    // delete font data if loaded from zipped skin
    if (font_data) {
        free(font_data);
    }

    if (config) {
        delete (config);
    }
}
