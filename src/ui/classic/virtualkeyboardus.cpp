/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "virtualkeyboardus.h"

namespace fcitx::classicui {

void UsKeyboard::updateKeys() {
    if (mode_ == UsKeyboardMode::Mark) {
        setMarkKeys();
        return;
    }

    setTextKeys();
}

void UsKeyboard::switchMode() {
    if (mode_ == UsKeyboardMode::Text) {
        mode_ = UsKeyboardMode::Mark;
    } else {
        mode_ = UsKeyboardMode::Text;
    }

    updateKeys();
}

void UsModeSwitchKey::switchState(VirtualKeyboard *keyboard, InputContext *) {
    keyboard->i18nKeyboard<UsKeyboard>()->switchMode();
}

int UsModeSwitchKey::currentIndex(VirtualKeyboard *keyboard) {
    if (keyboard->i18nKeyboard<UsKeyboard>()->mode() == UsKeyboardMode::Text) {
        return 0;
    }
    return 1;
}

void UsKeyboard::setTextKeys() {
    keys_.clear();
    keys_.emplace_back(new NormalKey("q", 24, "Q"));
    keys_.emplace_back(new NormalKey("w", 25, "W"));
    keys_.emplace_back(new NormalKey("e", 26, "E"));
    keys_.emplace_back(new NormalKey("r", 27, "R"));
    keys_.emplace_back(new NormalKey("t", 28, "T"));
    keys_.emplace_back(new NormalKey("y", 29, "Y"));
    keys_.emplace_back(new NormalKey("u", 30, "U"));
    keys_.emplace_back(new NormalKey("i", 31, "I"));
    keys_.emplace_back(new NormalKey("o", 32, "O"));
    keys_.emplace_back(new NormalKey("p", 33, "P"));
    keys_.emplace_back(new NormalBackSpaceKey()); keys_.back()->setCustomLayout(1.0);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("7"));
    keys_.emplace_back(new MarkKey("8"));
    keys_.emplace_back(new MarkKey("9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NormalKey("a", 38, "A"));
    keys_.emplace_back(new NormalKey("s", 39, "S"));
    keys_.emplace_back(new NormalKey("d", 40, "D"));
    keys_.emplace_back(new NormalKey("f", 41, "F"));
    keys_.emplace_back(new NormalKey("g", 42, "G"));
    keys_.emplace_back(new NormalKey("h", 43, "H"));
    keys_.emplace_back(new NormalKey("j", 44, "J"));
    keys_.emplace_back(new NormalKey("k", 45, "K"));
    keys_.emplace_back(new NormalKey("l", 46, "L"));
    keys_.emplace_back(new NormalEnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("4"));
    keys_.emplace_back(new MarkKey("5"));
    keys_.emplace_back(new MarkKey("6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ShiftToggleKey());
    keys_.emplace_back(new NormalKey("z", 52, "Z"));
    keys_.emplace_back(new NormalKey("x", 53, "X"));
    keys_.emplace_back(new NormalKey("c", 54, "C"));
    keys_.emplace_back(new NormalKey("v", 55, "V"));
    keys_.emplace_back(new NormalKey("b", 56, "B"));
    keys_.emplace_back(new NormalKey("n", 57, "N"));
    keys_.emplace_back(new NormalKey("m", 58, "M"));
    keys_.emplace_back(new MarkKey("-"));
    keys_.emplace_back(new UpKey());
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("1"));
    keys_.emplace_back(new MarkKey("2"));
    keys_.emplace_back(new MarkKey("3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new UsModeSwitchKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey(",")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey(" ")); keys_.back()->setCustomLayout(2.0); keys_.back()->setCustomBackgroundColor({0.3, 0.3, 0.3});
    keys_.emplace_back(new MarkKey("!"));
    keys_.emplace_back(new MarkKey("?"));
    keys_.emplace_back(new LeftKey());
    keys_.emplace_back(new DownKey());
    keys_.emplace_back(new RightKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

void UsKeyboard::setMarkKeys() {
    keys_.clear();
    keys_.emplace_back(new MarkKey("["));
    keys_.emplace_back(new MarkKey("]"));
    keys_.emplace_back(new MarkKey("{"));
    keys_.emplace_back(new MarkKey("}"));
    keys_.emplace_back(new MarkKey("#"));
    keys_.emplace_back(new MarkKey("%"));
    keys_.emplace_back(new MarkKey("^"));
    keys_.emplace_back(new MarkKey("*"));
    keys_.emplace_back(new MarkKey("+"));
    keys_.emplace_back(new MarkKey("="));
    keys_.emplace_back(new NormalBackSpaceKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("7"));
    keys_.emplace_back(new MarkKey("8"));
    keys_.emplace_back(new MarkKey("9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("/"));
    keys_.emplace_back(new MarkKey("\\"));
    keys_.emplace_back(new MarkKey(":"));
    keys_.emplace_back(new MarkKey(";"));
    keys_.emplace_back(new MarkKey("("));
    keys_.emplace_back(new MarkKey(")"));
    keys_.emplace_back(new MarkKey("&"));
    keys_.emplace_back(new MarkKey("@"));
    keys_.emplace_back(new MarkKey("￥"));
    keys_.emplace_back(new NormalEnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("4"));
    keys_.emplace_back(new MarkKey("5"));
    keys_.emplace_back(new MarkKey("6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey());
    keys_.emplace_back(new MarkKey("\"")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new MarkKey("\'")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new MarkKey("_")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey("|"));
    keys_.emplace_back(new MarkKey("~"));
    keys_.emplace_back(new MarkKey("<"));
    keys_.emplace_back(new MarkKey(">"));
    keys_.emplace_back(new MarkKey("-"));
    keys_.emplace_back(new UpKey());
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("1"));
    keys_.emplace_back(new MarkKey("2"));
    keys_.emplace_back(new MarkKey("3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new UsModeSwitchKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey(",")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey(" ")); keys_.back()->setCustomLayout(2.0); keys_.back()->setCustomBackgroundColor({0.3, 0.3, 0.3});
    keys_.emplace_back(new MarkKey("!"));
    keys_.emplace_back(new MarkKey("?"));
    keys_.emplace_back(new LeftKey());
    keys_.emplace_back(new DownKey());
    keys_.emplace_back(new RightKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

}
