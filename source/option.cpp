//
// Created by cpasjuste on 05/12/16.
//

#include "option.h"

Option::Option(const std::string &text,
               const std::vector<std::string> &options,
               int defaultValue, Index idx, int flags) {
    this->flags = flags;
    this->text = text;
    this->options = options;
    this->value = defaultValue;
    this->index = idx;
}

const char *Option::getName() {
    return text.c_str();
}

const char *Option::getValue() {
    if(value >= options.size()) {
        return "NONE";
    }
    return options[value].c_str();
}

void Option::next() {
    value++;
    if (value >= options.size())
        value = 0;
}

void Option::prev() {
    value--;
    if (value < 0)
        value = (int) (options.size() - 1);
}
