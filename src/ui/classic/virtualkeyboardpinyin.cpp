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

void PinyinTextKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "PinyinTextKey pushed: " << label(keyboard);

    auto keyEvent = fcitx::KeyEvent(inputContext, convert(), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;
}

const char* PinyinMarkKey::label(VirtualKeyboard *) const {
    return mark_.c_str();
}

void PinyinMarkKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "PinyinMarkKey pushed: " << label(keyboard);

    auto keyEvent = fcitx::KeyEvent(inputContext, convert(), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;
}

void PinyinModeSwitchKey::click(VirtualKeyboard *keyboard, InputContext *, bool isRelease) {
    FCITX_KEYBOARD() << "PinyinModeSwitchKey pushed";

    if (isRelease) {
        return;
    }

    keyboard->i18nKeyboard<PinyinKeyboard>()->switchMode();
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
    // TODO fix for pinyin
    keys_.clear();
    keys_.emplace_back(new PinyinTextKey("q", "q", "Q"));
    keys_.emplace_back(new PinyinTextKey("w", "w", "W"));
    keys_.emplace_back(new PinyinTextKey("e", "e", "E"));
    keys_.emplace_back(new PinyinTextKey("r", "r", "R"));
    keys_.emplace_back(new PinyinTextKey("t", "t", "T"));
    keys_.emplace_back(new PinyinTextKey("y", "y", "Y"));
    keys_.emplace_back(new PinyinTextKey("u", "u", "U"));
    keys_.emplace_back(new PinyinTextKey("i", "i", "I"));
    keys_.emplace_back(new PinyinTextKey("o", "o", "O"));
    keys_.emplace_back(new PinyinTextKey("p", "p", "P"));
    keys_.emplace_back(new BackSpaceKey()); keys_.back()->setCustomLayout(1.0);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("7", "7"));
    keys_.emplace_back(new PinyinMarkKey("8", "8"));
    keys_.emplace_back(new PinyinMarkKey("9", "9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinTextKey("a", "a", "A"));
    keys_.emplace_back(new PinyinTextKey("s", "s", "S"));
    keys_.emplace_back(new PinyinTextKey("d", "d", "D"));
    keys_.emplace_back(new PinyinTextKey("f", "f", "F"));
    keys_.emplace_back(new PinyinTextKey("g", "g", "G"));
    keys_.emplace_back(new PinyinTextKey("h", "h", "H"));
    keys_.emplace_back(new PinyinTextKey("j", "j", "J"));
    keys_.emplace_back(new PinyinTextKey("k", "k", "K"));
    keys_.emplace_back(new PinyinTextKey("l", "l", "L"));
    keys_.emplace_back(new EnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("4", "4"));
    keys_.emplace_back(new PinyinMarkKey("5", "5"));
    keys_.emplace_back(new PinyinMarkKey("6", "6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ShiftToggleKey());
    keys_.emplace_back(new PinyinTextKey("z", "z", "Z"));
    keys_.emplace_back(new PinyinTextKey("x", "x", "X"));
    keys_.emplace_back(new PinyinTextKey("c", "c", "C"));
    keys_.emplace_back(new PinyinTextKey("v", "v", "V"));
    keys_.emplace_back(new PinyinTextKey("b", "b", "B"));
    keys_.emplace_back(new PinyinTextKey("n", "n", "N"));
    keys_.emplace_back(new PinyinTextKey("m", "m", "M"));
    keys_.emplace_back(new PinyinMarkKey("minus", "-"));
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191"));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("1", "1"));
    keys_.emplace_back(new PinyinMarkKey("2", "2"));
    keys_.emplace_back(new PinyinMarkKey("3", "3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new PinyinModeSwitchKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new PinyinMarkKey("comma", ",")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinMarkKey("period", ".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinMarkKey("space", "")); keys_.back()->setCustomLayout(2.0); keys_.back()->setCustomBackgroundColor({0.3, 0.3, 0.3});
    keys_.emplace_back(new PinyinMarkKey("exclam", "!"));
    keys_.emplace_back(new PinyinMarkKey("question", "?"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190"));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193"));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("0", "0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new PinyinMarkKey("period", ".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

void PinyinKeyboard::setMarkKeys() {
    // TODO fix for pinyin
    keys_.clear();
    keys_.emplace_back(new PinyinMarkKey("bracketleft", "["));
    keys_.emplace_back(new PinyinMarkKey("bracketright", "]"));
    keys_.emplace_back(new PinyinMarkKey("braceleft", "{"));
    keys_.emplace_back(new PinyinMarkKey("braceright", "}"));
    keys_.emplace_back(new PinyinMarkKey("numbersign", "#"));
    keys_.emplace_back(new PinyinMarkKey("percent", "%"));
    keys_.emplace_back(new PinyinMarkKey("asciicircum", "^"));
    keys_.emplace_back(new PinyinMarkKey("asterisk", "*"));
    keys_.emplace_back(new PinyinMarkKey("plus", "+"));
    keys_.emplace_back(new PinyinMarkKey("equal", "="));
    keys_.emplace_back(new BackSpaceKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("7", "7"));
    keys_.emplace_back(new PinyinMarkKey("8", "8"));
    keys_.emplace_back(new PinyinMarkKey("9", "9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("slash", "/"));
    keys_.emplace_back(new PinyinMarkKey("backslash", "\\"));
    keys_.emplace_back(new PinyinMarkKey("colon", ":"));
    keys_.emplace_back(new PinyinMarkKey("semicolon", ";"));
    keys_.emplace_back(new PinyinMarkKey("parenleft", "("));
    keys_.emplace_back(new PinyinMarkKey("parenright", ")"));
    keys_.emplace_back(new PinyinMarkKey("ampersand", "&"));
    keys_.emplace_back(new PinyinMarkKey("at", "@"));
    keys_.emplace_back(new PinyinMarkKey("yen", u8"\u00A5"));
    keys_.emplace_back(new EnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("4", "4"));
    keys_.emplace_back(new PinyinMarkKey("5", "5"));
    keys_.emplace_back(new PinyinMarkKey("6", "6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ShiftToggleKey());
    keys_.emplace_back(new PinyinMarkKey("quotedbl", "\""));
    keys_.emplace_back(new PinyinMarkKey("apostrophe", "\'"));
    keys_.emplace_back(new PinyinMarkKey("underscore", "_")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinMarkKey("bar", "|"));
    keys_.emplace_back(new PinyinMarkKey("asciitilde", "~"));
    keys_.emplace_back(new PinyinMarkKey("less", "<"));
    keys_.emplace_back(new PinyinMarkKey("greater", ">"));
    keys_.emplace_back(new PinyinMarkKey("minus", "-"));
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191"));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("1", "1"));
    keys_.emplace_back(new PinyinMarkKey("2", "2"));
    keys_.emplace_back(new PinyinMarkKey("3", "3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new PinyinModeSwitchKey());  keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new PinyinMarkKey("comma", ",")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinMarkKey("period", ".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new PinyinMarkKey("space", "")); keys_.back()->setCustomLayout(2.0); keys_.back()->setCustomBackgroundColor({0.3, 0.3, 0.3});
    keys_.emplace_back(new PinyinMarkKey("exclam", "!"));
    keys_.emplace_back(new PinyinMarkKey("question", "?"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190"));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193"));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new PinyinMarkKey("0", "0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new PinyinMarkKey("period", ".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

}
