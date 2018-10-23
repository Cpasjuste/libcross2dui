//
// Created by cpasjuste on 04/02/18.
//

#include "c2dui.h"

using namespace c2d;
using namespace c2dui;

UIProgressBox::UIProgressBox(UIMain *gui)
        : RectangleShape(Vector2f(gui->getRenderer()->getSize().x / 2,
                                  gui->getRenderer()->getSize().y / 2)) {

    float w = getSize().x;
    float h = getSize().y;

    setSize(w, h);
    setPosition(w, h);
    setOrigin(Origin::Center);
    setFillColor(Color::Gray);
    setOutlineColor(COL_ORANGE);
    setOutlineThickness(2);

    float margin = UI_MARGIN * gui->getScaling();

    title = new Text("TITLE", C2D_DEFAULT_CHAR_SIZE, gui->getFont());
    title->setPosition(margin, margin + 16);
    title->setWidth(w - (margin * 2));
    title->setOutlineThickness(2);
    title->setOutlineColor(Color::Black);
    add(title);

    progress_bg = new RectangleShape(
            FloatRect(margin, h - margin - (h / 6),
                      w - (margin * 2), h / 6));
    progress_bg->setFillColor(Color::Gray);
    progress_bg->setOutlineColor(COL_GREEN);
    progress_bg->setOutlineThickness(2);
    add(progress_bg);

    progress_fg = new RectangleShape(
            FloatRect(progress_bg->getPosition().x + 1,
                      progress_bg->getPosition().y + 1,
                      2,
                      progress_bg->getSize().y - 2));
    progress_fg->setFillColor(COL_YELLOW);
    add(progress_fg);

    message = new Text("MESSAGE", (unsigned int) gui->getFontSize(), gui->getFont());
    message->setPosition(margin, progress_bg->getPosition().y - C2D_DEFAULT_CHAR_SIZE - 16);
    message->setWidth(w - (margin * 2));
    message->setOutlineThickness(2);
    message->setOutlineColor(Color::Black);
    add(message);

    setVisibility(Visibility::Hidden);
}

void UIProgressBox::setTitle(std::string title) {

    this->title->setString(title);
}

void UIProgressBox::setProgress(float progress) {

    float width = progress * (progress_bg->getSize().x - 2);
    progress_fg->setSize(
            std::min(width, progress_bg->getSize().x - 2),
            progress_fg->getSize().y);
}

void UIProgressBox::setMessage(std::string message) {

    this->message->setString(message);
}
