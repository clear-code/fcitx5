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
    keys_.emplace_back(new TextKey("q", "Q"));
    keys_.emplace_back(new TextKey("w", "W"));
    keys_.emplace_back(new TextKey("e", "E"));
    keys_.emplace_back(new TextKey("r", "R"));
    keys_.emplace_back(new TextKey("t", "T"));
    keys_.emplace_back(new TextKey("y", "Y"));
    keys_.emplace_back(new TextKey("u", "U"));
    keys_.emplace_back(new TextKey("i", "I"));
    keys_.emplace_back(new TextKey("o", "O"));
    keys_.emplace_back(new TextKey("p", "P"));
    keys_.emplace_back(new BackSpaceKey(false)); keys_.back()->setCustomLayout(1.0);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("7"));
    keys_.emplace_back(new TextKey("8"));
    keys_.emplace_back(new TextKey("9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("a", "A"));
    keys_.emplace_back(new TextKey("s", "S"));
    keys_.emplace_back(new TextKey("d", "D"));
    keys_.emplace_back(new TextKey("f", "F"));
    keys_.emplace_back(new TextKey("g", "G"));
    keys_.emplace_back(new TextKey("h", "H"));
    keys_.emplace_back(new TextKey("j", "J"));
    keys_.emplace_back(new TextKey("k", "K"));
    keys_.emplace_back(new TextKey("l", "L"));
    keys_.emplace_back(new EnterKey(false)); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("4"));
    keys_.emplace_back(new TextKey("5"));
    keys_.emplace_back(new TextKey("6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ShiftToggleKey());
    keys_.emplace_back(new TextKey("z", "Z"));
    keys_.emplace_back(new TextKey("x", "X"));
    keys_.emplace_back(new TextKey("c", "C"));
    keys_.emplace_back(new TextKey("v", "V"));
    keys_.emplace_back(new TextKey("b", "B"));
    keys_.emplace_back(new TextKey("n", "N"));
    keys_.emplace_back(new TextKey("m", "M"));
    keys_.emplace_back(new TextKey("-"));
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191", false));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("1"));
    keys_.emplace_back(new TextKey("2"));
    keys_.emplace_back(new TextKey("3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new UsModeSwitchKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new TextKey(",")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new TextKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new TextKey(" ")); keys_.back()->setCustomLayout(2.0); keys_.back()->setCustomBackgroundColor({0.3, 0.3, 0.3});
    keys_.emplace_back(new TextKey("!"));
    keys_.emplace_back(new TextKey("?"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190", false));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193", false));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192", false));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new TextKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

void UsKeyboard::setMarkKeys() {
    keys_.clear();
    keys_.emplace_back(new TextKey("["));
    keys_.emplace_back(new TextKey("]"));
    keys_.emplace_back(new TextKey("{"));
    keys_.emplace_back(new TextKey("}"));
    keys_.emplace_back(new TextKey("#"));
    keys_.emplace_back(new TextKey("%"));
    keys_.emplace_back(new TextKey("^"));
    keys_.emplace_back(new TextKey("*"));
    keys_.emplace_back(new TextKey("+"));
    keys_.emplace_back(new TextKey("="));
    keys_.emplace_back(new BackSpaceKey(false));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("7"));
    keys_.emplace_back(new TextKey("8"));
    keys_.emplace_back(new TextKey("9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("/"));
    keys_.emplace_back(new TextKey("\\"));
    keys_.emplace_back(new TextKey(":"));
    keys_.emplace_back(new TextKey(";"));
    keys_.emplace_back(new TextKey("("));
    keys_.emplace_back(new TextKey(")"));
    keys_.emplace_back(new TextKey("&"));
    keys_.emplace_back(new TextKey("@"));
    keys_.emplace_back(new TextKey("￥"));
    keys_.emplace_back(new EnterKey(false)); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("4"));
    keys_.emplace_back(new TextKey("5"));
    keys_.emplace_back(new TextKey("6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey());
    keys_.emplace_back(new TextKey("\"")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new TextKey("\'")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new TextKey("_")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new TextKey("|"));
    keys_.emplace_back(new TextKey("~"));
    keys_.emplace_back(new TextKey("<"));
    keys_.emplace_back(new TextKey(">"));
    keys_.emplace_back(new TextKey("-"));
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191", false));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("1"));
    keys_.emplace_back(new TextKey("2"));
    keys_.emplace_back(new TextKey("3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new UsModeSwitchKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new TextKey(",")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new TextKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new TextKey(" ")); keys_.back()->setCustomLayout(2.0); keys_.back()->setCustomBackgroundColor({0.3, 0.3, 0.3});
    keys_.emplace_back(new TextKey("!"));
    keys_.emplace_back(new TextKey("?"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190", false));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193", false));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192", false));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new TextKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

}
