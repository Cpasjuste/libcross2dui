//
// Created by cpasjuste on 30/01/18.
//

#ifdef __SWITCH__
extern "C" {
#include <switch/services/hid.h>
}
#endif

#include "c2dui.h"

using namespace c2d;
using namespace c2dui;

class MenuLine : public c2d::RectangleShape {

public:

    MenuLine(UIMain *ui, FloatRect &rect) : RectangleShape(rect) {

        setFillColor(Color::Transparent);

        this->ui = ui;
        Font *font = ui->getSkin()->font;
        int fontSize = ui->getFontSize();

        name = new Text("OPTION NAME,)'", (unsigned int) fontSize, font);
        name->setOutlineThickness(1);
        name->setOutlineColor(Color::Black);
        name->setOrigin(Origin::Left);
        name->setPosition(16, getSize().y / 2);
        name->setWidth((getSize().x * 0.66f) - 32);
        add(name);

        value = new Text("OPTION VALUE,)'", (unsigned int) fontSize, font);
        value->setOutlineThickness(1);
        value->setOutlineColor(Color::Black);
        value->setOrigin(Origin::Left);
        value->setPosition((getSize().x * 0.66f) + 16, getSize().y / 2);
        value->setWidth((getSize().x * 0.33f) - 32);
        add(value);
    }

    void update(Option *opt) {

        if (texture != nullptr) {
            delete (texture);
            texture = nullptr;
        }

        option = opt;
        if (option) {
            name->setString(option->getName());
        } else {
            value->setVisibility(Visibility::Visible);
            value->setString("GO");
            return;
        }

        if (option->flags == Option::Type::INPUT) {
            Skin::Button *button = ui->getSkin()->getButton(option->value);
            // don't use button textures on keyboard for now
            if (button && option->id < Option::Index::JOY_DEADZONE) {
                if (ui->getIo()->exist(button->path)) {
                    texture = new C2DTexture(button->path);
                    if (texture->available) {
                        value->setVisibility(Visibility::Hidden);
                        float tex_scaling = std::min(
                                ((getSize().x * 0.33f) - 32) / texture->getTextureRect().width,
                                (getSize().y / 2 + 4) / texture->getTextureRect().height);
                        texture->setScale(tex_scaling, tex_scaling);
                        texture->setPosition((getSize().x * 0.66f) + 16, getSize().y / 2 - 3);
                        texture->setOrigin(Origin::Left);
                        add(texture);
                    } else {
                        delete (texture);
                        texture = nullptr;
                        value->setVisibility(Visibility::Visible);
                        value->setString(button->name);
                    }
                } else {
                    value->setVisibility(Visibility::Visible);
                    value->setString(button->name);
                }
            } else {
                char btn[16];
                snprintf(btn, 16, "%i", option->value);
                value->setVisibility(Visibility::Visible);
                value->setString(btn);
            }
        } else {
            value->setVisibility(Visibility::Visible);
            value->setString(option->getValue());
        }
    }

    UIMain *ui = nullptr;
    Text *name = nullptr;
    Text *value = nullptr;
    Texture *texture = nullptr;
    Option *option;
};

UIMenu::UIMenu(UIMain *ui) : RectangleShape(Vector2f(0, 0)) {

    printf("GuiMenu (%p)\n", this);
    this->ui = ui;

    setFillColor(fillColor[0]);
    setOutlineColor(COL_RED);
    setOutlineThickness(2);
    if (ui->getRenderer()->getSize().y < 544) {
        setPosition(UI_MARGIN * ui->getScaling(), UI_MARGIN * ui->getScaling());
        setSize(ui->getRenderer()->getSize().x - (UI_MARGIN * ui->getScaling() * 2),
                ui->getRenderer()->getSize().y - (UI_MARGIN * ui->getScaling() * 2));
    } else {
        setPosition(UI_MARGIN * ui->getScaling() * 4, UI_MARGIN * ui->getScaling() * 4);
        setSize(ui->getRenderer()->getSize().x - (UI_MARGIN * ui->getScaling() * 8),
                ui->getRenderer()->getSize().y - (UI_MARGIN * ui->getScaling() * 8));
    }

    // menu title
    title = new Text("TITLE", C2D_DEFAULT_CHAR_SIZE, ui->getSkin()->font);
    title->setWidth(getSize().x - 16);
    title->setFillColor(Color::White);
    title->setOutlineThickness(2);
    title->setOutlineColor(COL_RED);
    title->setStyle(c2d::Text::Underlined);
    title->setPosition(20 * ui->getScaling(), 20 * ui->getScaling());
    int start_y = (int) (title->getGlobalBounds().top + title->getGlobalBounds().height + 16 * ui->getScaling());
    add(title);

    // calculate lines per menu
    float font_size = ui->getFontSize();
    float line_height = ui->getFont()->getLineSpacing((unsigned int) font_size) + 4;
    int max_lines = (int) ((getSize().y - start_y) / line_height) * 2;

    // add selection rectangle (highlight)
    highlight = new RectangleShape(Vector2f(((getSize().x / 2) * 0.3f) - 4, line_height));
    highlight->setOutlineThickness(1);
    highlight->setOutlineColor(COL_ORANGE);
    highlight->setFillColor(Color(153, 255, 51, 100));
    auto *tween = new TweenAlpha(50, 150, 1.0f, TweenLoop::PingPong);
    highlight->add(tween);
    add(highlight);

    // add lines of text
    for (int i = 0; i < max_lines; i++) {

        FloatRect rect = {0, start_y + (i * line_height), getSize().x / 2, line_height};
        if (i >= max_lines / 2) {
            rect.left = getSize().x / 2;
            rect.top = start_y + ((i - ((float) max_lines / 2)) * line_height);
        }

        lines.push_back(new MenuLine(ui, rect));
        add(lines[i]);
    }

    // build menus
    optionMenuGui = new OptionMenu(nullptr, ui->getConfig()->get());
    optionMenuGui->addChild("EXIT");
    optionMenuRom = new OptionMenu(nullptr, ui->getConfig()->get(true), true);
    optionMenuRom->addChild("STATES");
    optionMenuRom->addChild("RETURN");
    optionMenuRom->addChild("EXIT");

    tweenPosition = new TweenPosition({getPosition().x, -getSize().y}, getPosition(), 0.3f);
    tweenPosition->setState(TweenState::Stopped);
    add(tweenPosition);

    setVisibility(Visibility::Hidden);
}

void UIMenu::load(bool isRom, OptionMenu *om) {

    isRomMenu = isRom;
    options = isRomMenu ? ui->getConfig()->get(true)
                        : ui->getConfig()->get();

    if (om == nullptr) {
        optionMenu = isRomMenu ? optionMenuRom : optionMenuGui;
    } else {
        optionMenu = om;
    }

    isEmuRunning = ui->getUiEmu()->isVisible();
    setFillColor(fillColor[isEmuRunning]);

    optionIndex = 0;
    optionCount = (int) (optionMenu->childs.size() + optionMenu->option_ids.size());

    if (isEmuRunning) {
        // if frameskip is enabled, we may get a black buffer,
        // force a frame to be drawn
        ui->getUiEmu()->updateFb();
    }

    if (isRomMenu) {
        title->setString(ui->getUiRomList()->getSelection()->name);
    } else {
        title->setString(optionMenu->title);
    }

    for (auto &line : lines) {
        line->setVisibility(Visibility::Hidden);
    }

    int line_index = 0;
    for (unsigned int i = 0; i < optionMenu->option_ids.size(); i++) {

        if (i >= lines.size()) {
            // oups
            break;
        }

        // menu types
        Option *option = ui->getConfig()->get(optionMenu->option_ids[i], isRomMenu);
        if (option == nullptr) {
            optionCount--;
            continue;
        }

        // skip rotation option if not needed
        if ((isOptionHidden(option)) || option->flags & Option::Type::HIDDEN) {
            optionCount--;
            continue;
        }

        lines[line_index]->update(option);
        lines[line_index]->setVisibility(Visibility::Visible);

        line_index++;
    }

    for (unsigned i = 0; i < optionMenu->childs.size(); i++) {

        if (i >= lines.size()) {
            // oups
            break;
        }

        // don't show custom in-game options when a game is not running
        if (isRomMenu && !isEmuRunning) {
            std::string title = optionMenu->childs[i]->title;
            if (title == "EXIT" || title == "RETURN" || title == "STATES") {
                optionCount--;
                continue;
            }
        }

        lines[line_index]->update(nullptr);
        lines[line_index]->name->setString(optionMenu->childs[i]->title);
        lines[line_index]->setVisibility(Visibility::Visible);
        line_index++;
    }

    updateHighlight();

    if (!isVisible()) {
        setVisibility(Visibility::Visible);
        setLayer(1);
    }
}

void UIMenu::updateHighlight() {

    Vector2f pos = {lines[optionIndex]->value->getPosition().x - 2,
                    lines[optionIndex]->getPosition().y};
    highlight->setPosition(pos);
}

int UIMenu::loop() {

    int ret = 0;
    bool option_changed = false;
    unsigned int key = ui->getInput()->getKeys();

    if (key > 0) {

        // UP
        if (key & Input::Key::Up) {
            optionIndex--;
            if (optionIndex < 0)
                optionIndex = optionCount - 1;
            updateHighlight();
        }
        // DOWN
        if (key & Input::Key::Down) {
            optionIndex++;
            if (optionIndex >= optionCount)
                optionIndex = 0;
            updateHighlight();
        }
        // LEFT /RIGHT
        if ((key & Input::Key::Left || key & Input::Key::Right)
            && (unsigned int) optionIndex < optionMenu->option_ids.size()) {
            Option *option = lines[optionIndex]->option;
            if (!option) {
                return ret;
            }
            option_changed = true;
            if (option->flags == Option::Type::INTEGER) {
                if (key & Input::Key::Left) {
                    option->prev();
                } else {
                    option->next();
                }
                lines[optionIndex]->update(option);

                if (!option->getInfo().empty()) {
                    ui->getUiMessageBox()->show("WARNING", option->getInfo(), "OK");
                }

                switch (option->id) {
                    case Option::Index::GUI_SHOW_CLONES:
                    case Option::Index::GUI_SHOW_ALL:
                    case Option::Index::GUI_SHOW_HARDWARE:
                        ret = UI_KEY_FILTER_ROMS;
                        break;

                    case Option::ROM_ROTATION:
                    case Option::Index::ROM_SCALING:
                        if (isEmuRunning) {
                            ui->getUiEmu()->getVideo()->updateScaling();
                        }
                        break;
                    case Option::Index::ROM_FILTER:
                        if (isEmuRunning) {
                            ui->getUiEmu()->getVideo()->setFilter((Texture::Filter) option->value);
                        }
                        break;
                    case Option::Index::ROM_SHADER:
                        if (isEmuRunning) {
                            ui->getUiEmu()->getVideo()->setShader(option->value);
                        }
                        break;
#ifdef __SWITCH__
                    case Option::Index::JOY_SINGLEJOYCON:
                        for (int i = 0; i < PLAYER_COUNT; i++) {
                            if (option->value > 0) {
                                hidSetNpadJoyAssignmentModeSingleByDefault((HidControllerID) i);
                            } else {
                                hidSetNpadJoyAssignmentModeDual((HidControllerID) i);
                            }
                        }
                        break;
#endif
                    default:
                        break;
                }
            }
        }

        // FIRE1
        if (key & Input::Key::Fire1) {
            if ((unsigned int) optionIndex < optionMenu->option_ids.size()) {
                Option *option = lines[optionIndex]->option;
                if (option->flags == Option::Type::INPUT) {
                    int new_key = 0;
                    int res = ui->getUiMessageBox()->show("NEW INPUT", "PRESS A BUTTON", "", "", &new_key, 5);
                    if (res != MessageBox::TIMEOUT) {
                        option->value = new_key;
                        option_changed = true;
                        lines[optionIndex]->update(option);
                    }
                }
            } else {
                // extra options in menu (manually added)
                OptionMenu *menu = optionMenu->childs[optionIndex - optionMenu->option_ids.size()];
                if (menu->title == "EXIT") {
                    setVisibility(Visibility::Hidden);
                    ret = isRomMenu ? UI_KEY_STOP_ROM : EV_QUIT;
                } else if (menu->title == "STATES") {
                    setVisibility(Visibility::Hidden);
                    ret = UI_KEY_SHOW_MEMU_STATE;
                } else if (menu->title == "RETURN") {
                    setVisibility(Visibility::Hidden);
                    ret = UI_KEY_RESUME_ROM;
                } else {
                    load(isRomMenu, menu);
                }
            }
        }

        // FIRE2
        if (key & Input::Key::Fire2
            || (key & Input::Key::Start && !isRomMenu)
            || (key & Input::Key::Select && isRomMenu)) {
            if (optionMenu->parent == nullptr) {
                if (isEmuRunning) {
                    setVisibility(Visibility::Hidden);
                    ret = UI_KEY_RESUME_ROM;
                } else {
                    ret = UI_KEY_SHOW_ROMLIST;
                }
            } else {
                load(isRomMenu, optionMenu->parent);
            }
        }

        if (key & EV_QUIT) {
            return EV_QUIT;
        }
    }

    if (option_changed) {
        if (isRomMenu) {
            ui->getConfig()->save(ui->getUiRomList()->getSelection());
        } else {
            ui->getConfig()->save();
        }
    }

    ui->getRenderer()->flip();

    return ret;
}

bool UIMenu::isOptionHidden(Option *option) {
    return false;
}

bool UIMenu::isRom() {
    return isRomMenu;
}

UIMain *UIMenu::getUi() {
    return ui;
}

UIMenu::~UIMenu() {
    printf("~GuiMenu\n");
    delete (optionMenuGui);
    delete (optionMenuRom);
}
