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

void AnthyKeyboard::syncState(const std::string &currentInputMethodName) {
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

    auto event = KeyEvent(inputContext, fcitx::Key(name_), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(event);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;
}

void ZenkakuHankakuKey::toggle(VirtualKeyboard *keyboard, InputContext *) {
    keyboard->i18nKeyboard<AnthyKeyboard>()->toggleZenkakuHankaku(keyboard);
}

bool ZenkakuHankakuKey::isOn(VirtualKeyboard *keyboard) {
    return keyboard->i18nKeyboard<AnthyKeyboard>()->isZenkakuOn();
}

void AnthyModeSwitchKey::switchState(VirtualKeyboard *keyboard, InputContext *) {
    keyboard->i18nKeyboard<AnthyKeyboard>()->switchMode();
}

int AnthyModeSwitchKey::currentIndex(VirtualKeyboard *keyboard) {
    if (keyboard->i18nKeyboard<AnthyKeyboard>()->mode() == AnthyKeyboardMode::Text) {
        return 0;
    }
    return 1;
}

void AnthyKeyboard::setTextKeys() {
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
    keys_.emplace_back(new BackSpaceKey()); keys_.back()->setCustomLayout(1.0);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new AnthyMarkKey("7", "7", "７"));
    keys_.emplace_back(new AnthyMarkKey("8", "8", "８"));
    keys_.emplace_back(new AnthyMarkKey("9", "9", "９")); keys_.back()->setCustomLayout(1.0, true);

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
    keys_.emplace_back(new EnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new AnthyMarkKey("4", "4", "４"));
    keys_.emplace_back(new AnthyMarkKey("5", "5", "５"));
    keys_.emplace_back(new AnthyMarkKey("6", "6", "６")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ShiftToggleKey());
    keys_.emplace_back(new NormalKey("z", 52, "Z"));
    keys_.emplace_back(new NormalKey("x", 53, "X"));
    keys_.emplace_back(new NormalKey("c", 54, "C"));
    keys_.emplace_back(new NormalKey("v", 55, "V"));
    keys_.emplace_back(new NormalKey("b", 56, "B"));
    keys_.emplace_back(new NormalKey("n", 57, "N"));
    keys_.emplace_back(new NormalKey("m", 58, "M"));
    keys_.emplace_back(new AnthyMarkKey("minus", "-", "ー"));
    keys_.emplace_back(new UpKey());
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new AnthyMarkKey("1", "1", "１"));
    keys_.emplace_back(new AnthyMarkKey("2", "2", "２"));
    keys_.emplace_back(new AnthyMarkKey("3", "3", "３")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new AnthyModeSwitchKey());
    keys_.emplace_back(new ZenkakuHankakuKey());
    keys_.emplace_back(new AnthyMarkKey("comma", ",", "、")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new AnthyMarkKey("period", ".", "。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new SpaceKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new AnthyMarkKey("exclam", "!", "！"));
    keys_.emplace_back(new AnthyMarkKey("question", "?", "？"));
    keys_.emplace_back(new LeftKey());
    keys_.emplace_back(new DownKey());
    keys_.emplace_back(new RightKey());
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
    keys_.emplace_back(new UpKey());
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new AnthyMarkKey("1", "1", "１"));
    keys_.emplace_back(new AnthyMarkKey("2", "2", "２"));
    keys_.emplace_back(new AnthyMarkKey("3", "3", "３")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new AnthyModeSwitchKey());
    keys_.emplace_back(new ZenkakuHankakuKey());
    keys_.emplace_back(new AnthyMarkKey("comma", ",", "、")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new AnthyMarkKey("period", ".", "。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new SpaceKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new AnthyMarkKey("exclam", "!", "！"));
    keys_.emplace_back(new AnthyMarkKey("question", "?", "？"));
    keys_.emplace_back(new LeftKey());
    keys_.emplace_back(new DownKey());
    keys_.emplace_back(new RightKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new AnthyMarkKey("0", "0", "０")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new AnthyMarkKey("period", ".", "。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

}
