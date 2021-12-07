/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "virtualkeyboardhangul.h"

namespace fcitx::classicui {

void HangulKeyboard::updateKeys() {
    if (mode_ == HangulKeyboardMode::Text) {
        setTextKeys();
    } else {
        setMarkKeys();
    }
}

void HangulKeyboard::switchMode() {
    if (mode_ == HangulKeyboardMode::Text) {
        mode_ = HangulKeyboardMode::Mark;
    } else {
        mode_ = HangulKeyboardMode::Text;
    }

    updateKeys();
}

void HangulModeSwitchKey::switchState(VirtualKeyboard *keyboard, InputContext *) {
    keyboard->i18nKeyboard<HangulKeyboard>()->switchMode();
}

int HangulModeSwitchKey::currentIndex(VirtualKeyboard *keyboard) {
    if (keyboard->i18nKeyboard<HangulKeyboard>()->mode() == HangulKeyboardMode::Text) {
        return 0;
    }
    return 1;
}

void HangulTextKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "HangulTextKey pushed: " << label(keyboard);

    auto keyEvent = fcitx::KeyEvent(inputContext, convert(keyboard->isShiftOn_), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;
}

void HangulSpaceKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "HangulSpaceKey pushed: " << label(keyboard);

    // Must complete preedit and input space character.
    // `space` keyevent is not processed in IME,
    // but sending event is necessary to complete preedit.
    auto keyEvent = fcitx::KeyEvent(inputContext, convert(keyboard->isShiftOn_), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;

    if(hasProcessedInIME || isRelease) {
        return;
    }

    inputContext->commitString(" ");
}

void HangulKeyboard::setTextKeys() {
    keys_.clear();
    keys_.emplace_back(new HangulTextKey("ᄇ", "q", "ᄈ", "Q"));
    keys_.emplace_back(new HangulTextKey("ᄌ", "w", "ᄍ", "W"));
    keys_.emplace_back(new HangulTextKey("ᄃ", "e", "ᄄ", "E"));
    keys_.emplace_back(new HangulTextKey("ᄀ", "r", "ᄁ", "R"));
    keys_.emplace_back(new HangulTextKey("ᄉ", "t", "ᄊ", "T"));
    keys_.emplace_back(new HangulTextKey("ᅭ", "y"));
    keys_.emplace_back(new HangulTextKey("ᅧ", "u"));
    keys_.emplace_back(new HangulTextKey("ᅣ", "i"));
    keys_.emplace_back(new HangulTextKey("ᅢ", "o", "ᅤ", "O"));
    keys_.emplace_back(new HangulTextKey("ᅦ", "p", "ᅨ", "P"));
    keys_.emplace_back(new BackSpaceKey()); keys_.back()->setCustomLayout(1.0);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("7"));
    keys_.emplace_back(new TextKey("8"));
    keys_.emplace_back(new TextKey("9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new HangulTextKey("ᄆ", "a"));
    keys_.emplace_back(new HangulTextKey("ᄂ", "s"));
    keys_.emplace_back(new HangulTextKey("ᄋ", "d"));
    keys_.emplace_back(new HangulTextKey("ᄅ", "f"));
    keys_.emplace_back(new HangulTextKey("ᄒ", "g"));
    keys_.emplace_back(new HangulTextKey("ᅩ", "h"));
    keys_.emplace_back(new HangulTextKey("ᅥ", "j"));
    keys_.emplace_back(new HangulTextKey("ᅡ", "k"));
    keys_.emplace_back(new HangulTextKey("ᅵ", "l"));
    keys_.emplace_back(new EnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("4"));
    keys_.emplace_back(new TextKey("5"));
    keys_.emplace_back(new TextKey("6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ShiftToggleKey());
    keys_.emplace_back(new HangulTextKey("ᄏ", "z"));
    keys_.emplace_back(new HangulTextKey("ᄐ", "x"));
    keys_.emplace_back(new HangulTextKey("ᄎ", "c"));
    keys_.emplace_back(new HangulTextKey("ᄑ", "v"));
    keys_.emplace_back(new HangulTextKey("ᅲ", "b"));
    keys_.emplace_back(new HangulTextKey("ᅮ", "n"));
    keys_.emplace_back(new HangulTextKey("ᅳ", "m"));
    keys_.emplace_back(new TextKey("-"));
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191"));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("1"));
    keys_.emplace_back(new TextKey("2"));
    keys_.emplace_back(new TextKey("3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new HangulModeSwitchKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new TextKey(",")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new TextKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new HangulSpaceKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new TextKey("!"));
    keys_.emplace_back(new TextKey("?"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190"));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193"));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new TextKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

void HangulKeyboard::setMarkKeys() {
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

    keys_.emplace_back(new HangulModeSwitchKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new TextKey(",")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new TextKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new HangulSpaceKey()); keys_.back()->setCustomLayout(2.0);
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
