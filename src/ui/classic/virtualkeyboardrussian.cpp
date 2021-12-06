/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "virtualkeyboardrussian.h"

namespace fcitx::classicui {

void RussianKeyboard::updateKeys() {
    if (mode_ == RussianKeyboardMode::Mark) {
        setMarkKeys();
        return;
    }
    if (isLatinTextOn()) {
        setLatinTextKeys();
    } else {
        setCyrillicTextKeys();
    }
}

void RussianKeyboard::switchMode() {
    if (mode_ == RussianKeyboardMode::Text) {
        mode_ = RussianKeyboardMode::Mark;
    } else {
        mode_ = RussianKeyboardMode::Text;
    }

    updateKeys();
}

void RussianKeyboard::toggleText() {
    isLatinTextOn_ = !isLatinTextOn_;
    updateKeys();
}

void RussianModeSwitchKey::switchState(VirtualKeyboard *keyboard, InputContext *) {
    keyboard->i18nKeyboard<RussianKeyboard>()->switchMode();
}

int RussianModeSwitchKey::currentIndex(VirtualKeyboard *keyboard) {
    if (keyboard->i18nKeyboard<RussianKeyboard>()->mode() == RussianKeyboardMode::Text) {
        return 0;
    }
    return 1;
}

void RussianTextToggleKey::toggle(VirtualKeyboard *keyboard, InputContext *) {
    keyboard->i18nKeyboard<RussianKeyboard>()->toggleText();
}

bool RussianTextToggleKey::isOn(VirtualKeyboard *keyboard) {
    return keyboard->i18nKeyboard<RussianKeyboard>()->isLatinTextOn();
}

void RussianKeyboard::setCyrillicTextKeys() {
    keys_.clear();
    keys_.emplace_back(new RussianTextKey("й", "Й"));
    keys_.emplace_back(new RussianTextKey("ц", "Ц"));
    keys_.emplace_back(new RussianTextKey("ч", "Ч"));
    keys_.emplace_back(new RussianTextKey("к", "К"));
    keys_.emplace_back(new RussianTextKey("е", "Е"));
    keys_.emplace_back(new RussianTextKey("н", "Н"));
    keys_.emplace_back(new RussianTextKey("г", "Г"));
    keys_.emplace_back(new RussianTextKey("ш", "Ш"));
    keys_.emplace_back(new RussianTextKey("щ", "Щ"));
    keys_.emplace_back(new RussianTextKey("з", "З"));
    keys_.emplace_back(new RussianTextKey("х", "Х"));
    keys_.emplace_back(new RussianTextKey("ъ", "Ъ"));
    keys_.emplace_back(new BackSpaceKey(false)); keys_.back()->setCustomLayout(1.0);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("7"));
    keys_.emplace_back(new TextKey("8"));
    keys_.emplace_back(new TextKey("9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new RussianDummyKey(20));
    keys_.emplace_back(new RussianTextKey("ф", "Ф"));
    keys_.emplace_back(new RussianTextKey("ы", "Ы"));
    keys_.emplace_back(new RussianTextKey("в", "В"));
    keys_.emplace_back(new RussianTextKey("а", "А"));
    keys_.emplace_back(new RussianTextKey("п", "П"));
    keys_.emplace_back(new RussianTextKey("р", "Р"));
    keys_.emplace_back(new RussianTextKey("о", "О"));
    keys_.emplace_back(new RussianTextKey("л", "Л"));
    keys_.emplace_back(new RussianTextKey("д", "Д"));
    keys_.emplace_back(new RussianTextKey("ж", "Ж"));
    keys_.emplace_back(new RussianTextKey("э", "Э"));
    keys_.emplace_back(new EnterKey(false)); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("4"));
    keys_.emplace_back(new TextKey("5"));
    keys_.emplace_back(new TextKey("6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new RussianShiftToggleKey());
    keys_.emplace_back(new RussianTextKey("я", "Я"));
    keys_.emplace_back(new RussianTextKey("у", "У"));
    keys_.emplace_back(new RussianTextKey("с", "С"));
    keys_.emplace_back(new RussianTextKey("м", "М"));
    keys_.emplace_back(new RussianTextKey("и", "И"));
    keys_.emplace_back(new RussianTextKey("т", "Т"));
    keys_.emplace_back(new RussianTextKey("ь", "Ь"));
    keys_.emplace_back(new RussianTextKey("б", "Б"));
    keys_.emplace_back(new RussianTextKey("ю", "Ю"));
    keys_.emplace_back(new RussianTextKey("ё", "Ё"));
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191", false));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("1"));
    keys_.emplace_back(new TextKey("2"));
    keys_.emplace_back(new TextKey("3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new RussianModeSwitchKey());
    keys_.emplace_back(new RussianTextToggleKey());
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

void RussianKeyboard::setLatinTextKeys() {
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

    keys_.emplace_back(new RussianModeSwitchKey());
    keys_.emplace_back(new RussianTextToggleKey());
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

void RussianKeyboard::setMarkKeys() {
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

    keys_.emplace_back(new RussianModeSwitchKey()); keys_.back()->setCustomLayout(2.0);
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
