//
// Created by cpasjuste on 05/12/16.
//

#include "c2dui_option.h"

using namespace c2dui;

C2DUIOption::C2DUIOption(const std::string &text,
                         const std::vector<std::string> &options,
                         int defaultValue, int id, unsigned int flags) {
    this->flags = flags;
    this->text = text;
    this->options = options;
    this->value = defaultValue;
    this->id = id;
}

const char *C2DUIOption::getName() {
    return text.c_str();
}

std::string C2DUIOption::getInfo() const {
    return info;
}

void C2DUIOption::setInfo(const std::string &inf) {
    info = inf;
}

const char *C2DUIOption::getValue() {
    if ((size_t) value >= options.size()) {
        return "NONE";
    }
    return options[value].c_str();
}

void C2DUIOption::next() {
    value++;
    if ((size_t) value >= options.size())
        value = 0;
}

void C2DUIOption::prev() {
    value--;
    if (value < 0)
        value = (int) (options.size() - 1);
}
