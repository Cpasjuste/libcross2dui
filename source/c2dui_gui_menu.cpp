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

class MenuLine : public c2d::Rectangle {

public:

    MenuLine(C2DUIGuiMain *ui, FloatRect &rect) : Rectangle(rect) {

        setFillColor(Color::Transparent);

        this->ui = ui;
        Font *font = ui->getSkin()->font;
        int fontSize = ui->getFontSize();

        name = new Text("OPTION NAME,)'", *font, (unsigned int) fontSize);
        name->setOutlineThickness(1);
        name->setOutlineColor(Color::Black);
        name->setOrigin(0, name->getLocalBounds().height / 2);
        name->setPosition(16, getSize().y / 2);
        name->setSizeMax(Vector2f((getSize().x * 0.66f) - 32, 0));
        add(name);

        value = new Text("OPTION VALUE,)'", *font, (unsigned int) fontSize);
        value->setOutlineThickness(1);
        value->setOutlineColor(Color::Black);
        value->setOrigin(0, value->getLocalBounds().height / 2);
        value->setPosition((getSize().x * 0.66f) + 16, getSize().y / 2);
        value->setSizeMax(Vector2f((getSize().x * 0.33f) - 32, 0));
        add(value);
    }

    void update(C2DUIOption *opt) {

        if (texture != nullptr) {
            delete (texture);
            texture = nullptr;
        }

        option = opt;
        if (option) {
            name->setString(option->getName());
        } else {
            value->setVisibility(Visible);
            value->setString("GO");
            return;
        }

        if (option->flags == C2DUIOption::Type::INPUT) {
            C2DUISkin::Button *button = ui->getSkin()->getButton(option->value);
            // don't use button textures on keyboard for now
            if (button && option->index < C2DUIOption::Index::JOY_DEADZONE) {
                if (ui->getIo()->exist(button->path.c_str())) {
                    texture = new C2DTexture(button->path.c_str());
                    if (texture->available) {
                        value->setVisibility(Hidden);
                        float tex_scaling = std::min(
                                ((getSize().x * 0.33f) - 32) / texture->getSize().x,
                                (getSize().y / 2 + 4) / texture->getSize().y);
                        texture->setScale(tex_scaling, tex_scaling);
                        texture->setPosition((getSize().x * 0.66f) + 16, getSize().y / 2 - 3);
                        texture->setOrigin(0, texture->getSize().y / 2);
                        add(texture);
                    } else {
                        delete (texture);
                        texture = nullptr;
                        value->setVisibility(Visible);
                        value->setString(button->name);
                    }
                } else {
                    value->setVisibility(Visible);
                    value->setString(button->name);
                }
            } else {
                char btn[16];
                snprintf(btn, 16, "%i", option->value);
                value->setVisibility(Visible);
                value->setString(btn);
            }
        } else {
            value->setVisibility(Visible);
            value->setString(option->getValue());
        }
    }

    C2DUIGuiMain *ui = nullptr;
    Text *name = nullptr;
    Text *value = nullptr;
    Texture *texture = nullptr;
    C2DUIOption *option;
};

C2DUIGuiMenu::C2DUIGuiMenu(C2DUIGuiMain *ui) : Rectangle(Vector2f(0, 0)) {

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
    title = new Text("TITLE", *ui->getSkin()->font);
    title->setSizeMax(Vector2f(getSize().x - 16, 0));
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
    highlight = new Rectangle(Vector2f(((getSize().x / 2) * 0.3f) - 4, line_height));
    highlight->setOutlineThickness(1);
    highlight->setOutlineColor(COL_ORANGE);
    highlight->setFillColor(Color(153, 255, 51, 100));
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
    optionMenuGui = new C2DUIOptionMenu(nullptr, ui->getConfig()->getOptions());
    optionMenuGui->addChild("EXIT");
    optionMenuRom = new C2DUIOptionMenu(nullptr, ui->getConfig()->getOptions(true), true);
    optionMenuRom->addChild("STATES");
    optionMenuRom->addChild("RETURN");
    optionMenuRom->addChild("EXIT");

    setVisibility(Hidden);
}

void C2DUIGuiMenu::load(bool isRom, C2DUIOptionMenu *om) {

    isRomMenu = isRom;
    options = isRomMenu ? ui->getConfig()->getOptions(true)
                        : ui->getConfig()->getOptions();

    if (om == nullptr) {
        optionMenu = isRomMenu ? optionMenuRom : optionMenuGui;
    } else {
        optionMenu = om;
    }

    isEmuRunning = ui->getUiEmu()->getVisibility() == Visible;
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
        line->setVisibility(Hidden);
    }

    int line_index = 0;
    for (unsigned int i = 0; i < optionMenu->option_ids.size(); i++) {

        if (i >= lines.size()) {
            // oups
            break;
        }

        // menu types
        C2DUIOption *option = ui->getConfig()->getOption(options, optionMenu->option_ids[i]);
        if (option == nullptr) {
            optionCount--;
            continue;
        }

        // skip rotation option if not needed
        if ((isOptionHidden(option)) || option->flags & C2DUIOption::Type::HIDDEN) {
            optionCount--;
            continue;
        }

        lines[line_index]->update(option);
        lines[line_index]->setVisibility(Visible);

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
        lines[line_index]->setVisibility(Visible);
        line_index++;
    }

    updateHighlight();

    setVisibility(Visible);
    setLayer(1);
}

void C2DUIGuiMenu::updateHighlight() {

    highlight->setPosition(lines[optionIndex]->value->getGlobalBounds().left - 2,
                           lines[optionIndex]->getGlobalBounds().top);
}

int C2DUIGuiMenu::update() {

    int ret = 0;
    bool option_changed = false;
    unsigned int key = ui->getInput()->update()[0].state;

    if (key > 0) {

        // UP
        if (key & Input::Key::KEY_UP) {
            optionIndex--;
            if (optionIndex < 0)
                optionIndex = optionCount - 1;
            updateHighlight();
        }
        // DOWN
        if (key & Input::Key::KEY_DOWN) {
            optionIndex++;
            if (optionIndex >= optionCount)
                optionIndex = 0;
            updateHighlight();
        }
        // LEFT /RIGHT
        if ((key & Input::Key::KEY_LEFT || key & Input::Key::KEY_RIGHT)
            && (unsigned int) optionIndex < optionMenu->option_ids.size()) {
            C2DUIOption *option = lines[optionIndex]->option;
            if (!option) {
                return ret;
            }
            option_changed = true;
            if (option->flags == C2DUIOption::Type::INTEGER) {
                if (key & Input::Key::KEY_LEFT) {
                    option->prev();
                } else {
                    option->next();
                }
                lines[optionIndex]->update(option);

                switch (option->index) {
                    case C2DUIOption::Index::GUI_SHOW_CLONES:
                    case C2DUIOption::Index::GUI_SHOW_ALL:
                    case C2DUIOption::Index::GUI_SHOW_HARDWARE:
                        ret = UI_KEY_FILTER_ROMS;
                        break;

                    case C2DUIOption::ROM_ROTATION:
                    case C2DUIOption::Index::ROM_SCALING:
                        if (isEmuRunning) {
//#ifndef __SWITCH__
                            ui->getUiEmu()->getVideo()->updateScaling();
//#endif
                        }
                        break;
                    case C2DUIOption::Index::ROM_FILTER:
                        if (isEmuRunning) {
                            ui->getUiEmu()->getVideo()->setFiltering(option->value);
                        }
                        break;
                    case C2DUIOption::Index::ROM_SHADER:
                        if (isEmuRunning) {
                            ui->getUiEmu()->getVideo()->setShader(option->value);
                        }
                        break;
#ifdef __SWITCH__
                    case C2DUIOption::Index::JOY_SINGLEJOYCON:
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
        if (key & Input::Key::KEY_FIRE1) {
            if ((unsigned int) optionIndex < optionMenu->option_ids.size()) {
                C2DUIOption *option = lines[optionIndex]->option;
                if (option->flags == C2DUIOption::Type::INPUT) {
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
                C2DUIOptionMenu *menu = optionMenu->childs[optionIndex - optionMenu->option_ids.size()];
                if (menu->title == "EXIT") {
                    setVisibility(Hidden);
                    ret = isRomMenu ? UI_KEY_STOP_ROM : EV_QUIT;
                } else if (menu->title == "STATES") {
                    setVisibility(Hidden);
                    ret = UI_KEY_SHOW_MEMU_STATE;
                } else if (menu->title == "RETURN") {
                    setVisibility(Hidden);
                    ret = UI_KEY_RESUME_ROM;
                } else {
                    load(isRomMenu, menu);
                }
            }
        }

        // FIRE2
        if (key & Input::Key::KEY_FIRE2
            || (key & Input::Key::KEY_START && !isRomMenu)
            || (key & Input::Key::KEY_COIN && isRomMenu)) {
            if (optionMenu->parent == nullptr) {
                if (isEmuRunning) {
                    setVisibility(Hidden);
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

bool C2DUIGuiMenu::isOptionHidden(C2DUIOption *option) {
    return false;
}

bool C2DUIGuiMenu::isRom() {
    return isRomMenu;
}

C2DUIGuiMain *C2DUIGuiMenu::getUi() {
    return ui;
}

C2DUIGuiMenu::~C2DUIGuiMenu() {
    printf("~GuiMenu\n");
    delete (optionMenuGui);
    delete (optionMenuRom);
}
