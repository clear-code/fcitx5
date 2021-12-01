/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "virtualkeyboard.h"

FCITX_DEFINE_LOG_CATEGORY(keyboard, "keyboard")

namespace fcitx::classicui {

void Key::paintLabel(Keyboard *keyboard, cairo_t *cr) {
    cairo_save(cr);

    auto [r, g, b] = fontColorRgb_;
    cairo_set_source_rgb(cr, r, g, b);
    cairo_set_font_size(cr, fontSize_);
    cairo_text_extents_t extents;
    cairo_text_extents(cr, label(keyboard), &extents);
    cairo_translate(cr, labelOffsetX(extents), labelOffsetY(extents));
    cairo_show_text(cr, label(keyboard));

    cairo_restore(cr);
}

void Key::paintBackground(cairo_t *cr, bool highlight) {
    cairo_save(cr);

    cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
    cairo_rectangle(cr, 0, 0, width_, height_);
    cairo_stroke(cr);

    if (highlight) {
        cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
        cairo_rectangle(cr, 1, 1, width_ - 2, height_ - 2);
        cairo_fill(cr);
    } else if (useCustomBackgroundColor_) {
        auto [r, g, b] = customBackgroundColorRgb_;
        cairo_set_source_rgb(cr, r, g, b);
        cairo_rectangle(cr, 1, 1, width_ - 2, height_ - 2);
        cairo_fill(cr);
    }

    cairo_restore(cr);
}

const char* TextKey::label(Keyboard *keyboard) const {
    if (!keyboard->isShiftOn_ || upperText_.empty()) {
        return text_.c_str();
    }
    return upperText_.c_str();
}

void TextKey::click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "TextKey pushed: " << label(keyboard);

    if (!keyboard->isZenkakuOn_) {
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
    if (keyboard->isZenkakuOn_) {
        return zenkakuMark_.c_str();
    }
    return hankakuMark_.c_str();
}

void MarkKey::click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "MarkKey pushed: " << label(keyboard);

    if (!keyboard->isZenkakuOn_) {
        if (!isRelease) {
            inputContext->commitString(label(keyboard));
        }
        return;
    }

    auto keyEvent = fcitx::KeyEvent(inputContext, convert(), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;
}

void ForwardKey::click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "ForwardKey pushed: " << label(keyboard);

    auto keyEvent = fcitx::KeyEvent(inputContext, convert(keyboard->isShiftOn_), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;

    if(hasProcessedInIME) {
        canForwardKeyRelease_ = false;
        return;
    }

    if (!isRelease) {
        inputContext->forwardKey(convert(keyboard->isShiftOn_), false);
        canForwardKeyRelease_ = true;
        return;
    }

    if (!canForwardKeyRelease_) {
        return;
    }

    inputContext->forwardKey(convert(keyboard->isShiftOn_), true);
}

void ZenkakuHankakuKey::click(Keyboard *keyboard, InputContext *, bool isRelease) {
    FCITX_KEYBOARD() << "ZenkakuHankakuKey pushed: " << label(keyboard);
    if (isRelease) {
        return;
    }
    keyboard->isZenkakuOn_ = !keyboard->isZenkakuOn_;
}

void ZenkakuHankakuKey::paintLabel(Keyboard *keyboard, cairo_t *cr) {
    cairo_save(cr);

    if (keyboard->isZenkakuOn_) {
        cairo_set_source_rgb(cr, 0.2, 0.7, 0.6);
    } else {
        cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
    }
    cairo_set_font_size(cr, fontSize_);
    cairo_text_extents_t extents;
    cairo_text_extents(cr, label(keyboard), &extents);
    cairo_translate(cr, labelOffsetX(extents), labelOffsetY(extents));
    cairo_show_text(cr, label(keyboard));

    cairo_restore(cr);
}

void ShiftToggleKey::click(Keyboard *keyboard, InputContext *, bool isRelease) {
    FCITX_KEYBOARD() << "ShiftToggleKey pushed: " << label(keyboard);
    if (isRelease) {
        return;
    }
    keyboard->isShiftOn_ = !keyboard->isShiftOn_;
}

void ShiftToggleKey::paintLabel(Keyboard *keyboard, cairo_t *cr) {
    cairo_save(cr);

    if (keyboard->isShiftOn_) {
        cairo_set_source_rgb(cr, 0.2, 0.7, 0.6);
    } else {
        cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
    }
    cairo_set_font_size(cr, fontSize_);
    cairo_text_extents_t extents;
    cairo_text_extents(cr, label(keyboard), &extents);
    cairo_translate(cr, labelOffsetX(extents), labelOffsetY(extents));
    cairo_show_text(cr, label(keyboard));

    cairo_restore(cr);
}

void ModeSwitchKey::click(Keyboard *keyboard, InputContext *, bool isRelease) {
    FCITX_KEYBOARD() << "ModeSwitchKey pushed";

    if (isRelease) {
        return;
    }

    if (keyboard->mode_ == KeyboardMode::Text) {
        keyboard->mode_ = KeyboardMode::Mark;
        keyboard->setMarkKeys();
    } else {
        keyboard->mode_ = KeyboardMode::Text;
        keyboard->setTextKeys();
    }
}

void ModeSwitchKey::paintLabel(Keyboard *keyboard, cairo_t *cr) {
    cairo_save(cr);

    cairo_set_font_size(cr, fontSize_);
    cairo_text_extents_t extents;
    cairo_text_extents(cr, label(keyboard), &extents);
    cairo_translate(cr, labelOffsetX(extents), labelOffsetY(extents));

    if (keyboard->mode_ == KeyboardMode::Text) {
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

Keyboard::Keyboard(Instance *instance) : instance_(instance) {
    setTextKeys();

    repeatKeyTimer_ = instance_->eventLoop().addTimeEvent(
        CLOCK_MONOTONIC, now(CLOCK_MONOTONIC), 0,
        [this](EventSourceTime *, uint64_t) {
            onKeyRepeat();
            return true;
        });
    repeatKeyTimer_->setEnabled(false);
}

void Keyboard::setTextKeys() {
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
    keys_.emplace_back(new BackSpaceKey()); keys_.back()->setCustomLayout(1.0);
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
    keys_.emplace_back(new EnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("4", "4", "４"));
    keys_.emplace_back(new MarkKey("5", "5", "５"));
    keys_.emplace_back(new MarkKey("6", "6", "６")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ShiftToggleKey());
    keys_.emplace_back(new TextKey("z", "z", "Z"));
    keys_.emplace_back(new TextKey("x", "x", "X"));
    keys_.emplace_back(new TextKey("c", "c", "C"));
    keys_.emplace_back(new TextKey("v", "v", "V"));
    keys_.emplace_back(new TextKey("b", "b", "B"));
    keys_.emplace_back(new TextKey("n", "n", "N"));
    keys_.emplace_back(new TextKey("m", "m", "M"));
    keys_.emplace_back(new MarkKey("minus", "-", "ー"));
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new MarkKey("1", "1", "１"));
    keys_.emplace_back(new MarkKey("2", "2", "２"));
    keys_.emplace_back(new MarkKey("3", "3", "３")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ModeSwitchKey());
    keys_.emplace_back(new ZenkakuHankakuKey());
    keys_.emplace_back(new MarkKey("comma", ",", "、")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey("period", ".", "。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey("space", " ",  "")); keys_.back()->setCustomLayout(2.0); keys_.back()->setCustomBackgroundColor({0.3, 0.3, 0.3});
    keys_.emplace_back(new MarkKey("exclam", "!", "！"));
    keys_.emplace_back(new MarkKey("question", "?", "？"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190"));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193"));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("0", "0", "０")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey("period", ".", "。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
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
    keys_.emplace_back(new BackSpaceKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("7", "7", "７"));
    keys_.emplace_back(new MarkKey("8", "8", "８"));
    keys_.emplace_back(new MarkKey("9", "9", "９")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("slash", "/", "・"));
    keys_.emplace_back(new MarkKey("backslash", "\\", "＼"));
    keys_.emplace_back(new MarkKey("colon", ":", "："));
    keys_.emplace_back(new MarkKey("semicolon", ";", "；"));
    keys_.emplace_back(new MarkKey("parenleft", "(", "（"));
    keys_.emplace_back(new MarkKey("parenright", ")", "）"));
    keys_.emplace_back(new MarkKey("ampersand", "&", "＆"));
    keys_.emplace_back(new MarkKey("at", "@", "＠"));
    keys_.emplace_back(new MarkKey("yen", u8"\u00A5", "")); // `yen` does not work in Zenkaku
    keys_.emplace_back(new EnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("4", "4", "４"));
    keys_.emplace_back(new MarkKey("5", "5", "５"));
    keys_.emplace_back(new MarkKey("6", "6", "６")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ShiftToggleKey());
    keys_.emplace_back(new MarkKey("quotedbl", "\"", "”"));
    keys_.emplace_back(new MarkKey("apostrophe", "\'", "’"));
    keys_.emplace_back(new MarkKey("underscore", "_", "＿")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey("bar", "|", "｜"));
    keys_.emplace_back(new MarkKey("asciitilde", "~", "〜"));
    keys_.emplace_back(new MarkKey("less", "<", "＜"));
    keys_.emplace_back(new MarkKey("greater", ">", "＞"));
    keys_.emplace_back(new MarkKey("minus", "-", "ー"));
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new MarkKey("1", "1", "１"));
    keys_.emplace_back(new MarkKey("2", "2", "２"));
    keys_.emplace_back(new MarkKey("3", "3", "３")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ModeSwitchKey());
    keys_.emplace_back(new ZenkakuHankakuKey());
    keys_.emplace_back(new MarkKey("comma", ",", "、")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey("period", ".", "。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new MarkKey("space", " ", "")); keys_.back()->setCustomLayout(2.0); keys_.back()->setCustomBackgroundColor({0.3, 0.3, 0.3});
    keys_.emplace_back(new MarkKey("exclam", "!", "！"));
    keys_.emplace_back(new MarkKey("question", "?", "？"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190"));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193"));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new MarkKey("0", "0", "０")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new MarkKey("period", ".", "。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

void Keyboard::paint(cairo_t *cr, unsigned int offsetX, unsigned int offsetY) {
    int curX = offsetX;
    int curY = offsetY;

    cairo_save(cr);
    cairo_translate(cr, curX, curY);

    paintBackground(cr);

    for (const auto &key : keys_)
    {
        if (key->visible_) {
            auto highlight = (pushingKey_ == key.get());
            key->paintBackground(cr, highlight);
            key->paintLabel(this, cr);
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

void Keyboard::paintBackground(cairo_t *cr) {
    auto [keyboardWidth, keyboardHeight] = size();
    cairo_set_source_rgb(cr, 0.95, 0.95, 0.95);
    cairo_rectangle(cr, -marginX(), -marginY(),
        keyboardWidth  + 2 * marginX(), keyboardHeight + 2 * marginY());
    cairo_fill(cr);
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

void Keyboard::onKeyRepeat() {
    if (!pushingKey_) {
        return;
    }

    auto *inputContext = lastInputContext_.get();
    if (!inputContext) {
        return;
    }

    repeatKeyTimer_->setNextInterval(1000000 / repeatRate_);
    repeatKeyTimer_->setOneShot();
    pushingKey_->click(this, inputContext, false);
}

bool Keyboard::click(InputContext *inputContext, int x, int y, bool isRelease) {
    lastInputContext_ = inputContext->watch();

    if (isRelease) {
        repeatKeyTimer_->setEnabled(false);
    }

    auto [clickedKey, hasFound] = findClickedKey(x, y);
    if (!hasFound) {
        pushingKey_ = nullptr;
        return false;
    }

    clickedKey->click(this, inputContext, isRelease);

    pushingKey_ = isRelease ? nullptr : clickedKey;
    if (pushingKey_) {
        repeatKeyTimer_->setNextInterval(repeatDelay_ * 1000);
        repeatKeyTimer_->setOneShot();
    }

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
