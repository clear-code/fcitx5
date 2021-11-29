/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "virtualkeyboard.h"

FCITX_DEFINE_LOG_CATEGORY(keyboard, "keyboard")

namespace fcitx::classicui {

const char* TextKey::label(Keyboard *keyboard) const {
    if (!keyboard->useUpperHankakuText_ || upperText_.empty()) {
        return text_.c_str();
    }
    return upperText_.c_str();
}

void TextKey::click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) const {
    FCITX_KEYBOARD() << "TextKey pushed: " << label(keyboard);

    if (keyboard->mode_ == KeyboardMode::HankakuText) {
        if (!isRelease) {
            inputContext->commitString(label(keyboard));
        }
        return;
    }

    auto keyEvent = fcitx::KeyEvent(inputContext, convert(), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;
}

const char* MarkKey::label(Keyboard *keyboard) const {
    if (keyboard->useZenkakuMark_) {
        return zenkakuMark_.c_str();
    }
    return hankakuMark_.c_str();
}

void MarkKey::click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) const {
    FCITX_KEYBOARD() << "MarkKey pushed: " << label(keyboard);

    if (!keyboard->useZenkakuMark_) {
        if (!isRelease) {
            inputContext->commitString(label(keyboard));
        }
        return;
    }

    auto keyEvent = fcitx::KeyEvent(inputContext, convert(), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;
}

void ForwardKey::click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) const {
    FCITX_KEYBOARD() << "ForwardKey pushed: " << label(keyboard);

    auto keyEvent = fcitx::KeyEvent(inputContext, convert(), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;

    if (!hasProcessedInIME) {
        inputContext->forwardKey(convert(), isRelease);
    }
}

const char* ZenkakuHankakuKey::label(Keyboard *keyboard) const {
    if (keyboard->useZenkakuMark_) {
        return "半角    [ 全角 ]";
    }
    return "[ 半角 ]    全角";
}

void ZenkakuHankakuKey::click(Keyboard *keyboard, InputContext *, bool isRelease) const {
    FCITX_KEYBOARD() << "ZenkakuHankakuKey pushed: " << label(keyboard);
    if (isRelease) {
        return;
    }
    keyboard->useZenkakuMark_ = !keyboard->useZenkakuMark_;
}

const char* UpperToggleKey::label(Keyboard *keyboard) const {
    if (keyboard->useUpperHankakuText_) {
        return "abc    [ ABC ]";
    }
    return "[ abc ]    ABC";
}

void UpperToggleKey::click(Keyboard *keyboard, InputContext *, bool isRelease) const {
    FCITX_KEYBOARD() << "UpperToggleKey pushed: " << label(keyboard);
    if (isRelease) {
        return;
    }
    keyboard->useUpperHankakuText_ = !keyboard->useUpperHankakuText_;
}

const char* ModeSwitchKey::label(Keyboard *keyboard) const {
    if (keyboard->mode_ == KeyboardMode::ZenkakuText) {
        return "[あ]  A  1";
    } else if (keyboard->mode_ == KeyboardMode::HankakuText) {
        return "あ  [A]  1";
    } else {
        return "あ  A  [1]";
    }
}

void ModeSwitchKey::click(Keyboard *keyboard, InputContext *, bool isRelease) const {
    FCITX_KEYBOARD() << "ModeSwitchKey pushed";

    if (isRelease) {
        return;
    }

    if (keyboard->mode_ == KeyboardMode::ZenkakuText) {
        keyboard->mode_ = KeyboardMode::HankakuText;
        keyboard->setTextKeys(false);
    } else if (keyboard->mode_ == KeyboardMode::HankakuText) {
        keyboard->mode_ = KeyboardMode::Mark;
        keyboard->useUpperHankakuText_ = false;
        keyboard->setMarkKeys();
    } else {
        keyboard->mode_ = KeyboardMode::ZenkakuText;
        keyboard->useZenkakuMark_ = false;
        keyboard->setTextKeys(true);
    }
}

Keyboard::Keyboard() {
    setTextKeys(true);
}

void Keyboard::setTextKeys(bool isZenkakuMode) {
    keys_.clear();
    keys_.emplace_back(new TextKey("q", "q", "Q"));
    keys_.emplace_back(new TextKey("w", "w", "W"));
    keys_.emplace_back(new TextKey("e", "e", "E"));
    keys_.emplace_back(new TextKey("r", "r", "R"));
    keys_.emplace_back(new TextKey("t", "t", "T"));
    keys_.emplace_back(new TextKey("y", "y", "Y"));
    keys_.emplace_back(new TextKey("u", "u", "U"));
    keys_.emplace_back(new TextKey("i", "i", "I"));
    keys_.emplace_back(new TextKey("o", "o", "O"));
    keys_.emplace_back(new TextKey("p", "p", "P"));
    keys_.emplace_back(new ForwardKey("BackSpace", "Back")); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("7", "7", "７"));
    keys_.emplace_back(new MarkKey("8", "8", "８"));
    keys_.emplace_back(new MarkKey("9", "9", "９")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("a", "a", "A"));
    keys_.emplace_back(new TextKey("s", "s", "S"));
    keys_.emplace_back(new TextKey("d", "d", "D"));
    keys_.emplace_back(new TextKey("f", "f", "F"));
    keys_.emplace_back(new TextKey("g", "g", "G"));
    keys_.emplace_back(new TextKey("h", "h", "H"));
    keys_.emplace_back(new TextKey("j", "j", "J"));
    keys_.emplace_back(new TextKey("k", "k", "K"));
    keys_.emplace_back(new TextKey("l", "l", "L"));
    keys_.emplace_back(new ForwardKey("Return", "Enter")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("4", "4", "４"));
    keys_.emplace_back(new MarkKey("5", "5", "５"));
    keys_.emplace_back(new MarkKey("6", "6", "６")); keys_.back()->setCustomLayout(1.0, true);

    if (isZenkakuMode) {
        keys_.emplace_back(new EmptyKey()); keys_.back()->setCustomLayout(1.5);
    } else {
        keys_.emplace_back(new UpperToggleKey()); keys_.back()->setCustomLayout(1.5);
    }
    keys_.emplace_back(new TextKey("z", "z", "Z"));
    keys_.emplace_back(new TextKey("x", "x", "X"));
    keys_.emplace_back(new TextKey("c", "c", "C"));
    keys_.emplace_back(new TextKey("v", "v", "V"));
    keys_.emplace_back(new TextKey("b", "b", "B"));
    keys_.emplace_back(new TextKey("n", "n", "N"));
    keys_.emplace_back(new TextKey("m", "m", "M"));
    keys_.emplace_back(new TextKey("question", isZenkakuMode ? "？" : "?"));
    keys_.emplace_back(new ForwardKey("Up", u8"\u25B2")); keys_.back()->setCustomLayout(1.0); // ▲
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new MarkKey("1", "1", "１"));
    keys_.emplace_back(new MarkKey("2", "2", "２"));
    keys_.emplace_back(new MarkKey("3", "3", "３")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ModeSwitchKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new TextKey("comma", isZenkakuMode ? "、" : ","));
    keys_.emplace_back(new TextKey("space", isZenkakuMode ? "" : " ")); keys_.back()->setCustomLayout(4.0);
    keys_.emplace_back(new TextKey("period", isZenkakuMode ? "。" : "."));
    keys_.emplace_back(new TextKey("exclam", isZenkakuMode ? "！" : "!"));
    keys_.emplace_back(new ForwardKey("Left", u8"\u25C0")); // ◀
    keys_.emplace_back(new ForwardKey("Down", u8"\u25BC")); // ▼
    keys_.emplace_back(new ForwardKey("Right", u8"\u25B6")); // ▶
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("0", "0", "０")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey("period", ".", "。"));
}

void Keyboard::setMarkKeys() {
    keys_.clear();
    keys_.emplace_back(new MarkKey("bracketleft", "[", "「"));
    keys_.emplace_back(new MarkKey("bracketright", "]", "」"));
    keys_.emplace_back(new MarkKey("braceleft", "{", "｛"));
    keys_.emplace_back(new MarkKey("braceright", "}", "｝"));
    keys_.emplace_back(new MarkKey("numbersign", "#", "＃"));
    keys_.emplace_back(new MarkKey("percent", "%", "％"));
    keys_.emplace_back(new MarkKey("asciicircum", "^", "＾"));
    keys_.emplace_back(new MarkKey("asterisk", "*", "＊"));
    keys_.emplace_back(new MarkKey("plus", "+", "＋"));
    keys_.emplace_back(new MarkKey("equal", "=", "＝"));
    keys_.emplace_back(new ForwardKey("BackSpace", "Back")); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("7", "7", "７"));
    keys_.emplace_back(new MarkKey("8", "8", "８"));
    keys_.emplace_back(new MarkKey("9", "9", "９")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new MarkKey("minus", "-", "ー"));
    keys_.emplace_back(new MarkKey("slash", "/", "・"));
    keys_.emplace_back(new MarkKey("backslash", "\\", "＼"));
    keys_.emplace_back(new MarkKey("colon", ":", "："));
    keys_.emplace_back(new MarkKey("semicolon", ";", "；"));
    keys_.emplace_back(new MarkKey("parenleft", "(", "（"));
    keys_.emplace_back(new MarkKey("parenright", ")", "）"));
    keys_.emplace_back(new MarkKey("ampersand", "&", "＆"));
    keys_.emplace_back(new MarkKey("at", "@", "＠"));
    keys_.emplace_back(new MarkKey("yen", u8"\u00A5", "")); // `yen` does not work in Zenkaku
    keys_.emplace_back(new ForwardKey("Return", "Enter")); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("4", "4", "４"));
    keys_.emplace_back(new MarkKey("5", "5", "５"));
    keys_.emplace_back(new MarkKey("6", "6", "６")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ZenkakuHankakuKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new MarkKey("quotedbl", "\"", "”"));
    keys_.emplace_back(new MarkKey("apostrophe", "\'", "’"));
    keys_.emplace_back(new MarkKey("underscore", "_", "＿"));
    keys_.emplace_back(new MarkKey("bar", "|", "｜"));
    keys_.emplace_back(new MarkKey("asciitilde", "~", "〜"));
    keys_.emplace_back(new MarkKey("less", "<", "＜"));
    keys_.emplace_back(new MarkKey("greater", ">", "＞"));
    keys_.emplace_back(new MarkKey("question", "?", "？"));
    keys_.emplace_back(new ForwardKey("Up", u8"\u25B2")); keys_.back()->setCustomLayout(1.0); // ▲
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new MarkKey("1", "1", "１"));
    keys_.emplace_back(new MarkKey("2", "2", "２"));
    keys_.emplace_back(new MarkKey("3", "3", "３")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ModeSwitchKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new MarkKey("comma", ",", "、"));
    keys_.emplace_back(new MarkKey("space", " ", "")); keys_.back()->setCustomLayout(4.0);
    keys_.emplace_back(new MarkKey("period", ".", "。"));
    keys_.emplace_back(new MarkKey("exclam", "!", "！"));
    keys_.emplace_back(new ForwardKey("Left", u8"\u25C0")); // ◀
    keys_.emplace_back(new ForwardKey("Down", u8"\u25BC")); // ▼
    keys_.emplace_back(new ForwardKey("Right", u8"\u25B6")); // ▶
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("0", "0", "０")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey("period", ".", "。"));
}

void Keyboard::paint(cairo_t *cr, unsigned int offsetX, unsigned int offsetY) {
    int curX = offsetX;
    int curY = offsetY;

    cairo_save(cr);
    cairo_translate(cr, curX, curY);

    for (const auto &key : keys_)
    {
        if (key->visible_) {
            paintOneKey(cr, key.get());
            key->setRegion(curX, curY);
        }

        if (key->newLine_) {
            curX = offsetX;
            curY += key->height_;
            cairo_restore(cr);
            cairo_save(cr);
            cairo_translate(cr, offsetX, curY);
        } else {
            curX += key->width_;
            cairo_translate(cr, key->width_, 0);
        }
    }

    cairo_restore(cr);
}

std::pair<unsigned int, unsigned int> Keyboard::size() {
    unsigned int width = 0, height = 0, w = 0;

    for (const auto &key : keys_)
    {
        w += key->width_;
        if (height == 0)
            height += key->height_;

        if (key->newLine_) {
            width = MAX(width, w);
            height += key->height_;
            w = 0;
        }
    }

    return {width, height};
}

void Keyboard::paintOneKey(cairo_t *cr, Key *key) {
    auto highlight = isAnyKeyPushing_ && pushingKey_ == key;

    cairo_save(cr);

    if (highlight) {
        cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
        cairo_rectangle(cr, 0, 0, key->width_, key->height_);
        cairo_fill(cr);
        cairo_set_source_rgb(cr, 0, 0, 0);
    }
    cairo_rectangle(cr, 0, 0, key->width_, key->height_);
    cairo_set_line_width(cr, 2);
    cairo_stroke(cr);

    cairo_set_font_size(cr, key->fontSize_);
    cairo_text_extents_t extents;
    cairo_text_extents(cr, key->label(this), &extents);
    cairo_translate(cr, (key->width_ - extents.width) / 2, (key->height_ - extents.y_bearing) / 2);
    cairo_show_text(cr, key->label(this));

    cairo_restore(cr);
}

bool Keyboard::click(InputContext *inputContext, int x, int y, bool isRelease) {
    auto [clickedKey, hasFound] = findClickedKey(x, y);
    if (!hasFound) {
        isAnyKeyPushing_ = false;
        pushingKey_ = nullptr;
        return false;
    }

    clickedKey->click(this, inputContext, isRelease);

    isAnyKeyPushing_ = !isRelease;
    pushingKey_ = !isRelease ? clickedKey : nullptr;

    return true;
}

std::tuple<Key *, bool> Keyboard::findClickedKey(int x, int y) {
    for (const auto &key : keys_)
    {
        if (!(key->visible_ && key->contains(x, y))) continue;
        return {key.get(), true}; 
    }
    return {nullptr, false};
}

} // namespace fcitx::classicui
