/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "virtualkeyboard.h"

FCITX_DEFINE_LOG_CATEGORY(keyboard, "keyboard")

namespace fcitx::classicui {

void VirtualKey::paintLabel(VirtualKeyboard *keyboard, cairo_t *cr) {
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

void VirtualKey::paintBackground(cairo_t *cr, bool highlight) {
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

VirtualKeyboard::VirtualKeyboard(Instance *instance) : instance_(instance) {
    repeatKeyTimer_ = instance_->eventLoop().addTimeEvent(
        CLOCK_MONOTONIC, now(CLOCK_MONOTONIC), 0,
        [this](EventSourceTime *, uint64_t) {
            onKeyRepeat();
            return true;
        });
    repeatKeyTimer_->setEnabled(false);

    syncState();
}

bool VirtualKeyboard::syncState() {
    auto curImName = instance_->currentInputMethod();
    auto imItems = instance_->inputMethodManager().currentGroup().inputMethodList();

    FCITX_KEYBOARD() << "Try to sync state. IM: " << curImName;

    auto newI18nKeyboard = i18nKeyboardSelector_.select(curImName, imItems);

    if (!i18nKeyboard_) {
        if (!newI18nKeyboard) {
            return false;
        }
        setI18nKeyboard(newI18nKeyboard);
        return true;
    }

    i18nKeyboard_->syncState(curImName);

    if (!newI18nKeyboard || newI18nKeyboard->type() == i18nKeyboard_->type()) {
        return false;
    }

    setI18nKeyboard(newI18nKeyboard);
    return true;
}

void VirtualKeyboard::setI18nKeyboard(I18nKeyboard *i18nKeyboard) {
    FCITX_KEYBOARD() << "Set I18nKeyboard:" << imeNames[i18nKeyboard->type()];
    i18nKeyboard_.reset(i18nKeyboard);
    i18nKeyboard_->updateKeys();
}

void VirtualKeyboard::switchLanguage() {
    const auto maxTryCount = 10;
    auto tryCount = 0;
    do
    {
        if (instance_->inputMethodManager().groupCount() > 1) {
            instance_->enumerateGroup(true);
        } else {
            instance_->enumerate(true);
        }
        tryCount ++;
        if (maxTryCount <= tryCount) {
            break;
        }
    } while (!syncState());
}

void VirtualKeyboard::setCurrentInputMethod(std::string name) {
    instance_->setCurrentInputMethod(name);
}

bool VirtualKeyboard::isPreediting() {
    auto inputContext = lastInputContext_.get();
    if (!inputContext) {
        return false;
    }

    auto candidateList = inputContext->inputPanel().candidateList();
    return !!candidateList;
}

void VirtualKeyboard::paint(cairo_t *cr, unsigned int offsetX, unsigned int offsetY) {
    int curX = offsetX;
    int curY = offsetY;

    cairo_save(cr);
    cairo_translate(cr, curX, curY);

    paintBackground(cr);

    for (const auto &key : keys())
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

void VirtualKeyboard::paintBackground(cairo_t *cr) {
    auto [keyboardWidth, keyboardHeight] = size();
    cairo_set_source_rgb(cr, 0.95, 0.95, 0.95);
    cairo_rectangle(cr, -marginX(), -marginY(),
        keyboardWidth  + 2 * marginX(), keyboardHeight + 2 * marginY());
    cairo_fill(cr);
}

std::pair<unsigned int, unsigned int> VirtualKeyboard::size() {
    unsigned int width = 0, height = 0, w = 0;

    for (const auto &key : keys())
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

void VirtualKeyboard::onKeyRepeat() {
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

bool VirtualKeyboard::click(InputContext *inputContext, int x, int y, bool isRelease) {
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

std::tuple<VirtualKey *, bool> VirtualKeyboard::findClickedKey(int x, int y) {
    for (const auto &key : keys())
    {
        if (!(key->visible_ && key->contains(x, y))) continue;
        return {key.get(), true};
    }
    return {nullptr, false};
}

} // namespace fcitx::classicui
