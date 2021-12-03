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

void PinyinTextKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "PinyinTextKey pushed: " << label(keyboard);

    auto keyEvent = fcitx::KeyEvent(inputContext, convert(keyboard->isShiftOn_), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;
}

const char* PinyinMarkKey::label(VirtualKeyboard *) const {
    return mark_.c_str();
}

void PinyinMarkKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "PinyinMarkKey pushed: " << label(keyboard);

    if (isRelease) {
        return;
    }

    inputContext->commitString(label(keyboard));
}

const char* PinyinSpaceKey::label(VirtualKeyboard *keyboard) const {
    return keyboard->isPreediting() ? "选定" : "空格";
}

void PinyinSpaceKey::click(VirtualKeyboard *, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "PinyinSpaceKey pushed";

    if (isRelease) {
        return;
    }

    auto keyEvent = fcitx::KeyEvent(inputContext, convert(), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;

    if(hasProcessedInIME) {
        return;
    }

    inputContext->commitString(" ");
}

const char* PinyinEnterKey::label(VirtualKeyboard *keyboard) const {
    return keyboard->isPreediting() ? "确认" : "换行";
}

void PinyinModeSwitchKey::click(VirtualKeyboard *keyboard, InputContext *, bool isRelease) {
    FCITX_KEYBOARD() << "PinyinModeSwitchKey pushed";

    if (isRelease) {
        return;
    }

    keyboard->i18nKeyboard<PinyinKeyboard>()->switchMode();
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

void PinyinModeSwitchKey::paintLabel(VirtualKeyboard *keyboard, cairo_t *cr) {
    cairo_save(cr);

    cairo_set_font_size(cr, fontSize_);
    cairo_text_extents_t extents;
    cairo_text_extents(cr, label(keyboard), &extents);
    cairo_translate(cr, labelOffsetX(extents), labelOffsetY(extents));

    if (keyboard->i18nKeyboard<PinyinKeyboard>()->mode() == PinyinKeyboardMode::Text) {
        cairo_set_source_rgb(cr, 0.2, 0.7, 0.6);
        cairo_show_text(cr, "A");
        cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
        cairo_show_text(cr, "#");
    } else {
        cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
        cairo_show_text(cr, "A");
        cairo_set_source_rgb(cr, 0.2, 0.7, 0.6);
        cairo_show_text(cr, "#");
    }

    cairo_restore(cr);
}

void PinyinKeyboard::setTextKeys() {
    keys_.clear();
    keys_.emplace_back(new PinyinTextKey("q", "Q"));
    keys_.emplace_back(new PinyinTextKey("w", "W"));
    keys_.emplace_back(new PinyinTextKey("e", "E"));
    keys_.emplace_back(new PinyinTextKey("r", "R"));
    keys_.emplace_back(new PinyinTextKey("t", "T"));
    keys_.emplace_back(new PinyinTextKey("y", "Y"));
    keys_.emplace_back(new PinyinTextKey("u", "U"));
    keys_.emplace_back(new PinyinTextKey("i", "I"));
    keys_.emplace_back(new PinyinTextKey("o", "O"));
    keys_.emplace_back(new PinyinTextKey("p", "P"));
    keys_.emplace_back(new BackSpaceKey()); keys_.back()->setCustomLayout(1.0);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("7"));
    keys_.emplace_back(new PinyinMarkKey("8"));
    keys_.emplace_back(new PinyinMarkKey("9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinTextKey("a", "A"));
    keys_.emplace_back(new PinyinTextKey("s", "S"));
    keys_.emplace_back(new PinyinTextKey("d", "D"));
    keys_.emplace_back(new PinyinTextKey("f", "F"));
    keys_.emplace_back(new PinyinTextKey("g", "G"));
    keys_.emplace_back(new PinyinTextKey("h", "H"));
    keys_.emplace_back(new PinyinTextKey("j", "J"));
    keys_.emplace_back(new PinyinTextKey("k", "K"));
    keys_.emplace_back(new PinyinTextKey("l", "L"));
    keys_.emplace_back(new PinyinEnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("4"));
    keys_.emplace_back(new PinyinMarkKey("5"));
    keys_.emplace_back(new PinyinMarkKey("6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ShiftToggleKey());
    keys_.emplace_back(new PinyinTextKey("z", "Z"));
    keys_.emplace_back(new PinyinTextKey("x", "X"));
    keys_.emplace_back(new PinyinTextKey("c", "C"));
    keys_.emplace_back(new PinyinTextKey("v", "V"));
    keys_.emplace_back(new PinyinTextKey("b", "B"));
    keys_.emplace_back(new PinyinTextKey("n", "N"));
    keys_.emplace_back(new PinyinTextKey("m", "M"));
    keys_.emplace_back(new PinyinMarkKey("、")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191"));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("1"));
    keys_.emplace_back(new PinyinMarkKey("2"));
    keys_.emplace_back(new PinyinMarkKey("3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new PinyinModeSwitchKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new PinyinMarkKey("。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinMarkKey("，")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinSpaceKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new PinyinMarkKey("！"));
    keys_.emplace_back(new PinyinMarkKey("？"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190"));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193"));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new PinyinMarkKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

void PinyinKeyboard::setMarkKeys() {
    keys_.clear();
    keys_.emplace_back(new PinyinMarkKey("“")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new PinyinMarkKey("”")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new PinyinMarkKey("‘")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new PinyinMarkKey("’")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new PinyinMarkKey("（"));
    keys_.emplace_back(new PinyinMarkKey("）"));
    keys_.emplace_back(new PinyinMarkKey("「"));
    keys_.emplace_back(new PinyinMarkKey("」"));
    keys_.emplace_back(new PinyinMarkKey("『"));
    keys_.emplace_back(new PinyinMarkKey("』"));
    keys_.emplace_back(new BackSpaceKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("7"));
    keys_.emplace_back(new PinyinMarkKey("8"));
    keys_.emplace_back(new PinyinMarkKey("9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("《"));
    keys_.emplace_back(new PinyinMarkKey("》"));
    keys_.emplace_back(new PinyinMarkKey("〈"));
    keys_.emplace_back(new PinyinMarkKey("〉"));
    keys_.emplace_back(new PinyinMarkKey("："));
    keys_.emplace_back(new PinyinMarkKey("；"));
    keys_.emplace_back(new PinyinMarkKey("—"));
    keys_.emplace_back(new PinyinMarkKey("…")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinMarkKey("@"));
    keys_.emplace_back(new PinyinEnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("4"));
    keys_.emplace_back(new PinyinMarkKey("5"));
    keys_.emplace_back(new PinyinMarkKey("6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new PinyinMarkToggleKey());
    keys_.emplace_back(new PinyinMarkKey("·"));
    keys_.emplace_back(new PinyinMarkKey("～"));
    keys_.emplace_back(new PinyinMarkKey("￥"));
    keys_.emplace_back(new PinyinMarkKey("$"));
    keys_.emplace_back(new PinyinMarkKey("&"));
    keys_.emplace_back(new PinyinMarkKey("|"));
    keys_.emplace_back(new PinyinMarkKey("_")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinMarkKey("、")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191"));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("1"));
    keys_.emplace_back(new PinyinMarkKey("2"));
    keys_.emplace_back(new PinyinMarkKey("3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new PinyinModeSwitchKey());  keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new PinyinMarkKey("。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinMarkKey("，")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinSpaceKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new PinyinMarkKey("！"));
    keys_.emplace_back(new PinyinMarkKey("？"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190"));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193"));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new PinyinMarkKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

void PinyinKeyboard::setAdditionalMarkKeys() {
    keys_.clear();
    keys_.emplace_back(new PinyinMarkKey("["));
    keys_.emplace_back(new PinyinMarkKey("]"));
    keys_.emplace_back(new PinyinMarkKey("{"));
    keys_.emplace_back(new PinyinMarkKey("}"));
    keys_.emplace_back(new PinyinMarkKey("#"));
    keys_.emplace_back(new PinyinMarkKey("%"));
    keys_.emplace_back(new PinyinMarkKey("^"));
    keys_.emplace_back(new PinyinMarkKey("*"));
    keys_.emplace_back(new PinyinMarkKey("+"));
    keys_.emplace_back(new PinyinMarkKey("="));
    keys_.emplace_back(new BackSpaceKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("7"));
    keys_.emplace_back(new PinyinMarkKey("8"));
    keys_.emplace_back(new PinyinMarkKey("9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("/"));
    keys_.emplace_back(new PinyinMarkKey("\\"));
    keys_.emplace_back(new PinyinMarkKey(":"));
    keys_.emplace_back(new PinyinMarkKey(";"));
    keys_.emplace_back(new PinyinMarkKey("("));
    keys_.emplace_back(new PinyinMarkKey(")"));
    keys_.emplace_back(new PinyinMarkKey("-"));
    keys_.emplace_back(new PinyinMarkKey("~"));
    keys_.emplace_back(new PinyinMarkKey("@"));
    keys_.emplace_back(new PinyinEnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("4"));
    keys_.emplace_back(new PinyinMarkKey("5"));
    keys_.emplace_back(new PinyinMarkKey("6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new PinyinMarkToggleKey());
    keys_.emplace_back(new PinyinMarkKey("\"")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new PinyinMarkKey("\'")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new PinyinMarkKey(u8"\u00A5"));
    keys_.emplace_back(new PinyinMarkKey("!"));
    keys_.emplace_back(new PinyinMarkKey("?"));
    keys_.emplace_back(new PinyinMarkKey("<"));
    keys_.emplace_back(new PinyinMarkKey(">"));
    keys_.emplace_back(new PinyinMarkKey("、")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191"));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("1"));
    keys_.emplace_back(new PinyinMarkKey("2"));
    keys_.emplace_back(new PinyinMarkKey("3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new PinyinModeSwitchKey());  keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new PinyinMarkKey("。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinMarkKey("，")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinSpaceKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new PinyinMarkKey("！"));
    keys_.emplace_back(new PinyinMarkKey("？"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190"));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193"));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new PinyinMarkKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

}
