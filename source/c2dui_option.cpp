//
// Created by cpasjuste on 05/12/16.
//

#include "c2dui_option.h"

using namespace c2dui;

Option::Option(const std::string &text,
               const std::vector<std::string> &options,
               int defaultValue, int id, unsigned int flags) {
    this->flags = flags;
    this->text = text;
    this->options = options;
    this->value = defaultValue;
    this->id = id;
}

const char *Option::getName() {
    return text.c_str();
}

std::string Option::getInfo() const {
    return info;
}

void Option::setInfo(const std::string &inf) {
    info = inf;
}

const char *Option::getValue() {
    if ((size_t) value >= options.size()) {
        return "NONE";
    }
    return options[value].c_str();
}

std::vector<std::string> *Option::getValues() {
    return &options;
}

void Option::next() {
    value++;
    if ((size_t) value >= options.size())
        value = 0;
}

void Option::prev() {
    value--;
    if (value < 0)
        value = (int) (options.size() - 1);
}
