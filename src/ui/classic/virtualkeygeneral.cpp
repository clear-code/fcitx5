/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "virtualkeygeneral.h"

namespace fcitx::classicui {

const char* TextKey::label(VirtualKeyboard *keyboard) const {
    if (!keyboard->isShiftOn_ || upperText_.empty()) {
        return text_.c_str();
    }
    return upperText_.c_str();
}

void TextKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "TextKey pushed: " << label(keyboard);

    if (isRelease) {
        return;
    }

    inputContext->commitString(label(keyboard));
}

void ForwardKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "ForwardKey pushed: " << label(keyboard);

    if (!tryToSendKeyEventFirst_) {
        inputContext->forwardKey(convert(keyboard->isShiftOn_), isRelease);
        return;
    }

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

void ToggleKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "ToggleKey pushed: " << label(keyboard);
    if (isRelease) {
        return;
    }
    toggle(keyboard, inputContext);
}

void ToggleKey::paintLabel(VirtualKeyboard *keyboard, cairo_t *cr) {
    cairo_save(cr);

    if (isOn(keyboard)) {
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

void ShiftToggleKey::toggle(VirtualKeyboard *keyboard, InputContext *) {
    keyboard->isShiftOn_ = !keyboard->isShiftOn_;
}

bool ShiftToggleKey::isOn(VirtualKeyboard *keyboard) {
    return keyboard->isShiftOn_;
}

void SwitchKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "SwitchKey pushed: " << label(keyboard);

    if (isRelease) {
        return;
    }

    switchState(keyboard, inputContext);
}

void SwitchKey::paintLabel(VirtualKeyboard *keyboard, cairo_t *cr) {
    cairo_save(cr);

    cairo_set_font_size(cr, fontSize_);
    cairo_text_extents_t extents;
    cairo_text_extents(cr, label(keyboard), &extents);
    cairo_translate(cr, labelOffsetX(extents), labelOffsetY(extents));

    for (int i = 0; i < numberOfStates(); i++)
    {
        if (i == currentIndex(keyboard)) {
            cairo_set_source_rgb(cr, 0.2, 0.7, 0.6);
        } else {
            cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
        }
        cairo_show_text(cr, stateLabel(i));
    }

    cairo_restore(cr);
}

const char *LanguageSwitchKey::label(VirtualKeyboard *keyboard) const {
    return keyboard->i18nKeyboard()->label();
}

void LanguageSwitchKey::click(VirtualKeyboard *keyboard, InputContext *, bool isRelease) {
    FCITX_KEYBOARD() << "LanguageSwitchKey pushed";
    if (!isRelease) {
        return;
    }
    keyboard->switchLanguage();
}

}
