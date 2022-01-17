/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "virtualkeyboardpinyin.h"

namespace fcitx::classicui {

void PinyinKeyboard::updateKeys() {
    if (mode_ == PinyinKeyboardMode::Text) {
        setTextKeys();
        return;
    }
    if (isAdditionalMarkOn()) {
        setAdditionalMarkKeys();
    } else {
        setMarkKeys();
    }
}

void PinyinKeyboard::switchMode() {
    if (mode_ == PinyinKeyboardMode::Text) {
        mode_ = PinyinKeyboardMode::Mark;
    } else {
        mode_ = PinyinKeyboardMode::Text;
    }

    updateKeys();
}

void PinyinKeyboard::toggleMark() {
    isAdditionalMarkOn_ = !isAdditionalMarkOn_;
    updateKeys();
}

const char* PinyinSpaceKey::label(VirtualKeyboard *keyboard) const {
    return keyboard->isPreediting() ? "选定" : "空格";
}

const char* PinyinEnterKey::label(VirtualKeyboard *keyboard) const {
    return keyboard->isPreediting() ? "确认" : "换行";
}

const char* PinyinMarkToggleKey::label(VirtualKeyboard *keyboard) const {
    if (keyboard->i18nKeyboard<PinyinKeyboard>()->isAdditionalMarkOn()) {
        return "返回";
    }
    return "更多";
}

void PinyinMarkToggleKey::click(VirtualKeyboard *keyboard, InputContext *, bool isRelease) {
    FCITX_KEYBOARD() << "PinyinMarkToggleKey pushed: " << label(keyboard);
    if (isRelease) {
        return;
    }
    keyboard->i18nKeyboard<PinyinKeyboard>()->toggleMark();
}

void PinyinModeSwitchKey::switchState(VirtualKeyboard *keyboard, InputContext *) {
    keyboard->i18nKeyboard<PinyinKeyboard>()->switchMode();
}

int PinyinModeSwitchKey::currentIndex(VirtualKeyboard *keyboard) {
    if (keyboard->i18nKeyboard<PinyinKeyboard>()->mode() == PinyinKeyboardMode::Text) {
        return 0;
    }
    return 1;
}

void PinyinKeyboard::setTextKeys() {
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
    keys_.emplace_back(new NumberKey("7"));
    keys_.emplace_back(new NumberKey("8"));
    keys_.emplace_back(new NumberKey("9")); keys_.back()->setCustomLayout(1.0, true);

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
    keys_.emplace_back(new PinyinEnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("4"));
    keys_.emplace_back(new NumberKey("5"));
    keys_.emplace_back(new NumberKey("6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ShiftToggleKey());
    keys_.emplace_back(new NormalKey("z", 52, "Z"));
    keys_.emplace_back(new NormalKey("x", 53, "X"));
    keys_.emplace_back(new NormalKey("c", 54, "C"));
    keys_.emplace_back(new NormalKey("v", 55, "V"));
    keys_.emplace_back(new NormalKey("b", 56, "B"));
    keys_.emplace_back(new NormalKey("n", 57, "N"));
    keys_.emplace_back(new NormalKey("m", 58, "M"));
    keys_.emplace_back(new NormalKey("、", 132, "", "backslash"));
    keys_.emplace_back(new UpKey());
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("1"));
    keys_.emplace_back(new NumberKey("2"));
    keys_.emplace_back(new NumberKey("3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new PinyinModeSwitchKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new NormalKey("，", 59, "", "comma")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new NormalKey("。", 60, "", "period")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinSpaceKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new NormalKey("！", 10, "", "exclam"));
    keys_.emplace_back(new NormalKey("？", 61, "", "question"));
    keys_.emplace_back(new LeftKey());
    keys_.emplace_back(new DownKey());
    keys_.emplace_back(new RightKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

void PinyinKeyboard::setMarkKeys() {
    keys_.clear();
    keys_.emplace_back(new MarkKey("“")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new MarkKey("”")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new MarkKey("‘")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new MarkKey("’")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new MarkKey("（"));
    keys_.emplace_back(new MarkKey("）"));
    keys_.emplace_back(new MarkKey("「"));
    keys_.emplace_back(new MarkKey("」"));
    keys_.emplace_back(new MarkKey("『"));
    keys_.emplace_back(new MarkKey("』"));
    keys_.emplace_back(new BackSpaceKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("7"));
    keys_.emplace_back(new NumberKey("8"));
    keys_.emplace_back(new NumberKey("9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("《"));
    keys_.emplace_back(new MarkKey("》"));
    keys_.emplace_back(new MarkKey("〈"));
    keys_.emplace_back(new MarkKey("〉"));
    keys_.emplace_back(new MarkKey("："));
    keys_.emplace_back(new MarkKey("；"));
    keys_.emplace_back(new MarkKey("—"));
    keys_.emplace_back(new MarkKey("…")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey("@"));
    keys_.emplace_back(new PinyinEnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("4"));
    keys_.emplace_back(new NumberKey("5"));
    keys_.emplace_back(new NumberKey("6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new PinyinMarkToggleKey());
    keys_.emplace_back(new MarkKey("·"));
    keys_.emplace_back(new MarkKey("～"));
    keys_.emplace_back(new MarkKey("￥"));
    keys_.emplace_back(new MarkKey("$"));
    keys_.emplace_back(new MarkKey("&"));
    keys_.emplace_back(new MarkKey("|"));
    keys_.emplace_back(new MarkKey("_")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey("、")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191"));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("1"));
    keys_.emplace_back(new NumberKey("2"));
    keys_.emplace_back(new NumberKey("3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new PinyinModeSwitchKey());  keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey("，")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey("。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinSpaceKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey("！"));
    keys_.emplace_back(new MarkKey("？"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190"));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193"));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

void PinyinKeyboard::setAdditionalMarkKeys() {
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
    keys_.emplace_back(new NumberKey("7"));
    keys_.emplace_back(new NumberKey("8"));
    keys_.emplace_back(new NumberKey("9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("/"));
    keys_.emplace_back(new MarkKey("\\"));
    keys_.emplace_back(new MarkKey(":"));
    keys_.emplace_back(new MarkKey(";"));
    keys_.emplace_back(new MarkKey("("));
    keys_.emplace_back(new MarkKey(")"));
    keys_.emplace_back(new MarkKey("-"));
    keys_.emplace_back(new MarkKey("~"));
    keys_.emplace_back(new MarkKey("@"));
    keys_.emplace_back(new PinyinEnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("4"));
    keys_.emplace_back(new NumberKey("5"));
    keys_.emplace_back(new NumberKey("6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new PinyinMarkToggleKey());
    keys_.emplace_back(new MarkKey("\"")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new MarkKey("\'")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new MarkKey(u8"\u00A5"));
    keys_.emplace_back(new MarkKey("!"));
    keys_.emplace_back(new MarkKey("?"));
    keys_.emplace_back(new MarkKey("<"));
    keys_.emplace_back(new MarkKey(">"));
    keys_.emplace_back(new MarkKey("、")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191"));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("1"));
    keys_.emplace_back(new NumberKey("2"));
    keys_.emplace_back(new NumberKey("3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new PinyinModeSwitchKey());  keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey("，")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey("。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinSpaceKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey("！"));
    keys_.emplace_back(new MarkKey("？"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190"));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193"));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new NumberKey("0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

}
