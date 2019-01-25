//
// Created by cpasjuste on 05/12/16.
//

#include <c2dui_option.h>

#include "c2dui_option.h"

using namespace c2dui;

Option::Option(const std::string &text,
               const std::vector<std::string> &options,
               int defaultValue, int id, unsigned int flags) {
    this->flags = flags;
    this->text = text;
    this->options = options;
    this->idx = defaultValue;
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

std::vector<std::string> *Option::getValues() {
    return &options;
}

const char *Option::getValueString() {
    if ((size_t) idx >= options.size()) {
        return "NONE";
    }
    return options[idx].c_str();
}


void Option::setValueString(const std::string &value) {
    if ((size_t) idx < options.size()) {
        options[idx] = value;
    }
}

int Option::getValueInt() {
    if ((size_t) idx >= options.size()) {
        return 0;
    }
    return std::stoi(options[idx]);
}

void Option::setValueInt(int value) {
    if ((size_t) idx < options.size()) {
        options[idx] = std::to_string(value);
    }
}

bool Option::getValueBool() {
    if ((size_t) idx >= options.size()) {
        return false;
    }
    return idx == 1;
}

void Option::setValueBool(bool value) {
    idx = value;
}

int Option::getId() {
    return id;
}

void Option::setId(int _id) {
    id = _id;
}

unsigned int Option::getFlags() {
    return flags;
}

void Option::setFlags(unsigned int _flags) {
    flags = _flags;
}

int Option::getIndex() {
    return idx;
}

void Option::setIndex(int _index) {
    idx = _index;
}

void Option::next() {
    idx++;
    if ((size_t) idx >= options.size())
        idx = 0;
}

void Option::prev() {
    idx--;
    if (idx < 0)
        idx = (int) (options.size() - 1);
}


