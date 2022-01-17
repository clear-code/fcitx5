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
    i18nKeyboard_.reset(new NullI18nKeyboard());

    syncState();
}

bool VirtualKeyboard::syncState() {
    FCITX_KEYBOARD() << "Try to sync state.";
    auto imItems = instance_->inputMethodManager().currentGroup().inputMethodList();

    auto [newI18nKeyboard, hasFound] = i18nKeyboardSelector_.select(imItems);
    if (hasFound && newI18nKeyboard->type() != i18nKeyboard_->type()) {
        setI18nKeyboard(newI18nKeyboard);
        return true;
    }

    auto curImName = instance_->currentInputMethod();

    i18nKeyboard_->syncState(curImName);
    return false;
}

void VirtualKeyboard::setI18nKeyboard(I18nKeyboard *i18nKeyboard) {
    FCITX_KEYBOARD() << "Set I18nKeyboard:" << imeNames[i18nKeyboard->type()];
    setCurrentInputMethod(imeNames[i18nKeyboard->type()]);
    i18nKeyboard_.reset(i18nKeyboard);
    i18nKeyboard_->updateKeys();
}

void VirtualKeyboard::switchLanguage() {
    if (instance_->inputMethodManager().groupCount() < 2) return;

    pushingKey_ = nullptr;
    isShiftOn_ = false;

    const auto maxTryCount = 10;
    auto tryCount = 0;
    do
    {
        enumerateGroup();

        tryCount ++;
        if (maxTryCount <= tryCount) {
            break;
        }
    } while (!syncState());
}

void VirtualKeyboard::setCurrentInputMethod(const std::string &name) {
    instance_->setCurrentInputMethod(name);
}

void VirtualKeyboard::enumerateGroup() {
    instance_->inputMethodManager().enumerateGroup(true);
}

void VirtualKeyboard::sendShiftModifierToIM(InputContext *inputContext, bool isRelease) {
    const auto keyFromName = fcitx::Key("SHIFT_Shift_R");
    const auto shiftKey = fcitx::Key(keyFromName.sym(), keyFromName.states(), 62);
    auto event = KeyEvent(inputContext, shiftKey, isRelease);
    inputContext->keyEvent(event);
}

bool VirtualKeyboard::isPreediting() {
    auto inputContext = lastInputContext_.get();
    if (!inputContext) {
        return false;
    }

    return !inputContext->inputPanel().empty();
}

void VirtualKeyboard::updateInputPanel() {
    auto inputContext = lastInputContext_.get();
    if (!inputContext) {
        return;
    }

    inputContext->updateUserInterface(UserInterfaceComponent::InputPanel);
}

bool VirtualKeyboard::isSeletingCandidates() {
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
        if (key->visible()) {
            auto highlight = (pushingKey_ == key.get());
            key->paintBackground(cr, highlight);
            key->paintLabel(this, cr);
            key->setRegion(curX, curY);
        }

        if (key->newLine()) {
            curX = offsetX;
            curY += key->height();
            cairo_restore(cr);
            cairo_save(cr);
            cairo_translate(cr, offsetX, curY);
        } else {
            curX += key->width();
            cairo_translate(cr, key->width(), 0);
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
        w += key->width();
        if (height == 0)
            height += key->height();

        if (key->newLine()) {
            width = MAX(width, w);
            height += key->height();
            w = 0;
        }
    }

    return {width, height};
}

bool VirtualKeyboard::click(InputContext *inputContext, int x, int y, bool isRelease) {
    // Need to pay attention to the possibility of invalid poitner of `pushingKey_`,
    // caused by changing the key-layout.

    lastInputContext_ = inputContext->watch();

    auto [clickedKey, hasFound] = findClickedKey(x, y);
    if (!hasFound) {
        if (pushingKey_) {
            // Make sure to send key release.
            pushingKey_->click(this, inputContext, true);
        }
        pushingKey_ = nullptr;
        return false;
    }

    if (isRelease) {
        if (pushingKey_) {
            // Make sure to send key release.
            pushingKey_->click(this, inputContext, true);
        } else {
            clickedKey->click(this, inputContext, true);
        }
        pushingKey_ = nullptr;
    } else {
        if (pushingKey_ && pushingKey_ != clickedKey) {
            // Make sure to send key release.
            pushingKey_->click(this, inputContext, true);
        }
        clickedKey->click(this, inputContext, false);
        pushingKey_ = clickedKey;
    }

    return true;
}

std::tuple<VirtualKey *, bool> VirtualKeyboard::findClickedKey(int x, int y) {
    for (const auto &key : keys())
    {
        if (!(key->visible() && key->contains(x, y))) continue;
        return {key.get(), true};
    }
    return {nullptr, false};
}

} // namespace fcitx::classicui
