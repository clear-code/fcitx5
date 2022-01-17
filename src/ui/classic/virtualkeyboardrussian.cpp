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

    setCyrillicTextKeys();
}

void RussianKeyboard::switchMode() {
    if (mode_ == RussianKeyboardMode::Text) {
        mode_ = RussianKeyboardMode::Mark;
    } else {
        mode_ = RussianKeyboardMode::Text;
    }

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

void RussianKeyboard::setCyrillicTextKeys() {
    keys_.clear();
    keys_.emplace_back(new RussianNormalKey("й", 24, "Й", "q", "Q"));
    keys_.emplace_back(new RussianNormalKey("ц", 25, "Ц", "w", "W"));
    keys_.emplace_back(new RussianNormalKey("ч", 26, "Ч", "e", "E"));
    keys_.emplace_back(new RussianNormalKey("к", 27, "К", "r", "R"));
    keys_.emplace_back(new RussianNormalKey("е", 28, "Е", "t", "T"));
    keys_.emplace_back(new RussianNormalKey("н", 29, "Н", "y", "Y"));
    keys_.emplace_back(new RussianNormalKey("г", 30, "Г", "u", "U"));
    keys_.emplace_back(new RussianNormalKey("ш", 31, "Ш", "i", "I"));
    keys_.emplace_back(new RussianNormalKey("щ", 32, "Щ", "o", "O"));
    keys_.emplace_back(new RussianNormalKey("з", 33, "З", "p", "P"));
    keys_.emplace_back(new RussianNormalKey("х", 34, "Х", "at", "grave"));
    keys_.emplace_back(new RussianNormalKey("ъ", 35, "Ъ", "bracketleft", "braceleft"));
    keys_.emplace_back(new BackSpaceKey()); keys_.back()->setCustomLayout(1.0);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("7", 16));
    keys_.emplace_back(new NumberKey("8", 17));
    keys_.emplace_back(new NumberKey("9", 18)); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new RussianDummyKey(20));
    keys_.emplace_back(new RussianNormalKey("ф", 38, "Ф", "a", "A"));
    keys_.emplace_back(new RussianNormalKey("ы", 39, "Ы", "s", "S"));
    keys_.emplace_back(new RussianNormalKey("в", 40, "В", "d", "D"));
    keys_.emplace_back(new RussianNormalKey("а", 41, "А", "f", "F"));
    keys_.emplace_back(new RussianNormalKey("п", 42, "П", "g", "G"));
    keys_.emplace_back(new RussianNormalKey("р", 43, "Р", "h", "H"));
    keys_.emplace_back(new RussianNormalKey("о", 44, "О", "j", "J"));
    keys_.emplace_back(new RussianNormalKey("л", 45, "Л", "k", "K"));
    keys_.emplace_back(new RussianNormalKey("д", 46, "Д", "l", "L"));
    keys_.emplace_back(new RussianNormalKey("ж", 47, "Ж", "semicolon", "plus"));
    keys_.emplace_back(new RussianNormalKey("э", 48, "Э", "colon", "asterisk"));
    keys_.emplace_back(new EnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("4", 13));
    keys_.emplace_back(new NumberKey("5", 14));
    keys_.emplace_back(new NumberKey("6", 15)); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new RussianShiftToggleKey());
    keys_.emplace_back(new RussianNormalKey("я", 52, "Я", "z", "Z"));
    keys_.emplace_back(new RussianNormalKey("у", 53, "У", "x", "X"));
    keys_.emplace_back(new RussianNormalKey("с", 54, "С", "c", "C"));
    keys_.emplace_back(new RussianNormalKey("м", 55, "М", "v", "V"));
    keys_.emplace_back(new RussianNormalKey("и", 56, "И", "b", "B"));
    keys_.emplace_back(new RussianNormalKey("т", 57, "Т", "n", "N"));
    keys_.emplace_back(new RussianNormalKey("ь", 58, "Ь", "m", "M"));
    keys_.emplace_back(new RussianNormalKey("б", 59, "Б", "comma", "less"));
    keys_.emplace_back(new RussianNormalKey("ю", 60, "Ю", "period", "greater"));
    keys_.emplace_back(new RussianNormalKey("ё", 49, "Ё", "Zenkaku_Hankaku", "Zenkaku_Hankaku"));
    keys_.emplace_back(new UpKey());
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("1", 10));
    keys_.emplace_back(new NumberKey("2", 11));
    keys_.emplace_back(new NumberKey("3", 12)); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new RussianModeSwitchKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey(",", "question", 61, true)); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey(".", "slash", 61)); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new SpaceKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey("!", "exclam", 10, true));
    keys_.emplace_back(new MarkKey("?", "apostrophe", 16, true));
    keys_.emplace_back(new LeftKey());
    keys_.emplace_back(new DownKey());
    keys_.emplace_back(new RightKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("0", 19)); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

void RussianKeyboard::setMarkKeys() {
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
    keys_.emplace_back(new BackSpaceKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("7", 16));
    keys_.emplace_back(new NumberKey("8", 17));
    keys_.emplace_back(new NumberKey("9", 18)); keys_.back()->setCustomLayout(1.0, true);

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
    keys_.emplace_back(new EnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("4", 13));
    keys_.emplace_back(new NumberKey("5", 14));
    keys_.emplace_back(new NumberKey("6", 15)); keys_.back()->setCustomLayout(1.0, true);

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
    keys_.emplace_back(new NumberKey("1", 10));
    keys_.emplace_back(new NumberKey("2", 11));
    keys_.emplace_back(new NumberKey("3", 12)); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new RussianModeSwitchKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey(",")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new SpaceKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey("!"));
    keys_.emplace_back(new MarkKey("?"));
    keys_.emplace_back(new LeftKey());
    keys_.emplace_back(new DownKey());
    keys_.emplace_back(new RightKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("0", 19)); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

}
