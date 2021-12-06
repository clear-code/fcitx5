/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "virtualkeyboardanthy.h"

namespace fcitx::classicui {

void AnthyKeyboard::updateKeys() {
    if (mode_ == AnthyKeyboardMode::Text) {
        setTextKeys();
    } else {
        setMarkKeys();
    }
}

void AnthyKeyboard::syncState(std::string currentInputMethodName) {
    isZenkakuOn_ = currentInputMethodName != hankakuImeName;
}

void AnthyKeyboard::switchMode() {
    if (mode_ == AnthyKeyboardMode::Text) {
        mode_ = AnthyKeyboardMode::Mark;
    } else {
        mode_ = AnthyKeyboardMode::Text;
    }

    updateKeys();
}

void AnthyKeyboard::toggleZenkakuHankaku(VirtualKeyboard *keyboard) {
    isZenkakuOn_ = !isZenkakuOn_;
    if (isZenkakuOn_) {
        keyboard->setCurrentInputMethod(imeNames[type()]);
    } else {
        keyboard->setCurrentInputMethod(hankakuImeName);
    }
}

void AnthyTextKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "AnthyTextKey pushed: " << label(keyboard);

    if (!keyboard->i18nKeyboard<AnthyKeyboard>()->isZenkakuOn()) {
        if (!isRelease) {
            inputContext->commitString(label(keyboard));
        }
        return;
    }

    auto keyEvent = fcitx::KeyEvent(inputContext, convert(keyboard->isShiftOn_), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;
}

const char* AnthyMarkKey::label(VirtualKeyboard *keyboard) const {
    if (keyboard->i18nKeyboard<AnthyKeyboard>()->isZenkakuOn()) {
        return zenkakuMark_.c_str();
    }
    return hankakuMark_.c_str();
}

void AnthyMarkKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "AnthyMarkKey pushed: " << label(keyboard);

    if (!keyboard->i18nKeyboard<AnthyKeyboard>()->isZenkakuOn()) {
        if (!isRelease) {
            inputContext->commitString(label(keyboard));
        }
        return;
    }

    auto keyEvent = fcitx::KeyEvent(inputContext, convert(), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;
}

void ZenkakuHankakuKey::toggle(VirtualKeyboard *keyboard, InputContext *) {
    keyboard->i18nKeyboard<AnthyKeyboard>()->toggleZenkakuHankaku(keyboard);
}

bool ZenkakuHankakuKey::isOn(VirtualKeyboard *keyboard) {
    return keyboard->i18nKeyboard<AnthyKeyboard>()->isZenkakuOn();
}

void AnthyModeSwitchKey::click(VirtualKeyboard *keyboard, InputContext *, bool isRelease) {
    FCITX_KEYBOARD() << "AnthyModeSwitchKey pushed";

    if (isRelease) {
        return;
    }

    keyboard->i18nKeyboard<AnthyKeyboard>()->switchMode();
}

void AnthyModeSwitchKey::paintLabel(VirtualKeyboard *keyboard, cairo_t *cr) {
    cairo_save(cr);

    cairo_set_font_size(cr, fontSize_);
    cairo_text_extents_t extents;
    cairo_text_extents(cr, label(keyboard), &extents);
    cairo_translate(cr, labelOffsetX(extents), labelOffsetY(extents));

    if (keyboard->i18nKeyboard<AnthyKeyboard>()->mode() == AnthyKeyboardMode::Text) {
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

void AnthyKeyboard::setTextKeys() {
    keys_.clear();
    keys_.emplace_back(new AnthyTextKey("q", "Q"));
    keys_.emplace_back(new AnthyTextKey("w", "W"));
    keys_.emplace_back(new AnthyTextKey("e", "E"));
    keys_.emplace_back(new AnthyTextKey("r", "R"));
    keys_.emplace_back(new AnthyTextKey("t", "T"));
    keys_.emplace_back(new AnthyTextKey("y", "Y"));
    keys_.emplace_back(new AnthyTextKey("u", "U"));
    keys_.emplace_back(new AnthyTextKey("i", "I"));
    keys_.emplace_back(new AnthyTextKey("o", "O"));
    keys_.emplace_back(new AnthyTextKey("p", "P"));
    keys_.emplace_back(new BackSpaceKey()); keys_.back()->setCustomLayout(1.0);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new AnthyMarkKey("7", "7", "７"));
    keys_.emplace_back(new AnthyMarkKey("8", "8", "８"));
    keys_.emplace_back(new AnthyMarkKey("9", "9", "９")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new AnthyTextKey("a", "A"));
    keys_.emplace_back(new AnthyTextKey("s", "S"));
    keys_.emplace_back(new AnthyTextKey("d", "D"));
    keys_.emplace_back(new AnthyTextKey("f", "F"));
    keys_.emplace_back(new AnthyTextKey("g", "G"));
    keys_.emplace_back(new AnthyTextKey("h", "H"));
    keys_.emplace_back(new AnthyTextKey("j", "J"));
    keys_.emplace_back(new AnthyTextKey("k", "K"));
    keys_.emplace_back(new AnthyTextKey("l", "L"));
    keys_.emplace_back(new EnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new AnthyMarkKey("4", "4", "４"));
    keys_.emplace_back(new AnthyMarkKey("5", "5", "５"));
    keys_.emplace_back(new AnthyMarkKey("6", "6", "６")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ShiftToggleKey());
    keys_.emplace_back(new AnthyTextKey("z", "Z"));
    keys_.emplace_back(new AnthyTextKey("x", "X"));
    keys_.emplace_back(new AnthyTextKey("c", "C"));
    keys_.emplace_back(new AnthyTextKey("v", "V"));
    keys_.emplace_back(new AnthyTextKey("b", "B"));
    keys_.emplace_back(new AnthyTextKey("n", "N"));
    keys_.emplace_back(new AnthyTextKey("m", "M"));
    keys_.emplace_back(new AnthyMarkKey("minus", "-", "ー"));
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191"));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new AnthyMarkKey("1", "1", "１"));
    keys_.emplace_back(new AnthyMarkKey("2", "2", "２"));
    keys_.emplace_back(new AnthyMarkKey("3", "3", "３")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new AnthyModeSwitchKey());
    keys_.emplace_back(new ZenkakuHankakuKey());
    keys_.emplace_back(new AnthyMarkKey("comma", ",", "、")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new AnthyMarkKey("period", ".", "。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new AnthyMarkKey("space", " ",  "")); keys_.back()->setCustomLayout(2.0); keys_.back()->setCustomBackgroundColor({0.3, 0.3, 0.3});
    keys_.emplace_back(new AnthyMarkKey("exclam", "!", "！"));
    keys_.emplace_back(new AnthyMarkKey("question", "?", "？"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190"));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193"));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new AnthyMarkKey("0", "0", "０")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new AnthyMarkKey("period", ".", "。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

void AnthyKeyboard::setMarkKeys() {
    keys_.clear();
    keys_.emplace_back(new AnthyMarkKey("bracketleft", "[", "「"));
    keys_.emplace_back(new AnthyMarkKey("bracketright", "]", "」"));
    keys_.emplace_back(new AnthyMarkKey("braceleft", "{", "｛"));
    keys_.emplace_back(new AnthyMarkKey("braceright", "}", "｝"));
    keys_.emplace_back(new AnthyMarkKey("numbersign", "#", "＃"));
    keys_.emplace_back(new AnthyMarkKey("percent", "%", "％"));
    keys_.emplace_back(new AnthyMarkKey("asciicircum", "^", "＾"));
    keys_.emplace_back(new AnthyMarkKey("asterisk", "*", "＊"));
    keys_.emplace_back(new AnthyMarkKey("plus", "+", "＋"));
    keys_.emplace_back(new AnthyMarkKey("equal", "=", "＝"));
    keys_.emplace_back(new BackSpaceKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new AnthyMarkKey("7", "7", "７"));
    keys_.emplace_back(new AnthyMarkKey("8", "8", "８"));
    keys_.emplace_back(new AnthyMarkKey("9", "9", "９")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new AnthyMarkKey("slash", "/", "・"));
    keys_.emplace_back(new AnthyMarkKey("backslash", "\\", "＼"));
    keys_.emplace_back(new AnthyMarkKey("colon", ":", "："));
    keys_.emplace_back(new AnthyMarkKey("semicolon", ";", "；"));
    keys_.emplace_back(new AnthyMarkKey("parenleft", "(", "（"));
    keys_.emplace_back(new AnthyMarkKey("parenright", ")", "）"));
    keys_.emplace_back(new AnthyMarkKey("ampersand", "&", "＆"));
    keys_.emplace_back(new AnthyMarkKey("at", "@", "＠"));
    keys_.emplace_back(new AnthyMarkKey("yen", u8"\u00A5", "")); // `yen` does not work in Zenkaku
    keys_.emplace_back(new EnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new AnthyMarkKey("4", "4", "４"));
    keys_.emplace_back(new AnthyMarkKey("5", "5", "５"));
    keys_.emplace_back(new AnthyMarkKey("6", "6", "６")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ShiftToggleKey());
    keys_.emplace_back(new AnthyMarkKey("quotedbl", "\"", "”")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new AnthyMarkKey("apostrophe", "\'", "’")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new AnthyMarkKey("underscore", "_", "＿")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new AnthyMarkKey("bar", "|", "｜"));
    keys_.emplace_back(new AnthyMarkKey("asciitilde", "~", "〜"));
    keys_.emplace_back(new AnthyMarkKey("less", "<", "＜"));
    keys_.emplace_back(new AnthyMarkKey("greater", ">", "＞"));
    keys_.emplace_back(new AnthyMarkKey("minus", "-", "ー"));
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191"));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new AnthyMarkKey("1", "1", "１"));
    keys_.emplace_back(new AnthyMarkKey("2", "2", "２"));
    keys_.emplace_back(new AnthyMarkKey("3", "3", "３")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new AnthyModeSwitchKey());
    keys_.emplace_back(new ZenkakuHankakuKey());
    keys_.emplace_back(new AnthyMarkKey("comma", ",", "、")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new AnthyMarkKey("period", ".", "。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new AnthyMarkKey("space", " ", "")); keys_.back()->setCustomLayout(2.0); keys_.back()->setCustomBackgroundColor({0.3, 0.3, 0.3});
    keys_.emplace_back(new AnthyMarkKey("exclam", "!", "！"));
    keys_.emplace_back(new AnthyMarkKey("question", "?", "？"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190"));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193"));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new AnthyMarkKey("0", "0", "０")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new AnthyMarkKey("period", ".", "。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

}
