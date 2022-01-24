/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "virtualkeygeneral.h"

namespace fcitx::classicui {

const char* NormalKey::label(VirtualKeyboard *keyboard) const {
    if (!keyboard->isShiftOn() || upperLabel_.empty()) {
        return label_.c_str();
    }
    return upperLabel_.c_str();
}

void NormalKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "NormalKey pushed: " << label(keyboard);

    if (keyboard->isShiftOn()) {
        keyboard->sendShiftModifierToIM(inputContext, false);
    }

    auto event = KeyEvent(inputContext, convert(keyboard->isShiftOn()), isRelease);
    inputContext->virtualKeyEvent(event);

    if (keyboard->isShiftOn()) {
        keyboard->sendShiftModifierToIM(inputContext, true);
    }
}

const char* MarkKey::label(VirtualKeyboard *) const {
    return label_.c_str();
}

void MarkKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "MarkKey pushed: " << label(keyboard);

    if (sendKeyEventFirst()) {
        if (withShift_) {
            keyboard->sendShiftModifierToIM(inputContext, false);
        }

        auto event = KeyEvent(inputContext, convert(withShift_), isRelease);
        const auto hasProcessedInIME = inputContext->keyEvent(event);

        if (withShift_) {
            keyboard->sendShiftModifierToIM(inputContext, true);
        }

        if (hasProcessedInIME) return;
    }

    if (isRelease) {
        return;
    }

    inputContext->commitString(label(keyboard));
}

const char* NumberKey::label(VirtualKeyboard *) const {
    return name_.c_str();
}

void NumberKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "NumberKey pushed: " << label(keyboard);

    auto event = fcitx::KeyEvent(inputContext, convert(false), isRelease);
    const auto hasProcessedInIME = inputContext->keyEvent(event);
    if (hasProcessedInIME || isRelease) return;

    inputContext->commitString(label(keyboard));
}

void ToggleKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "ToggleKey pushed: " << label(keyboard);
    // This may be used for changing key layouts.
    // Changin key layouts must be executed on key-release,
    // because VirtualKeyboard has `pushingKey_` pointer.
    if (!isRelease) {
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
    keyboard->toggleShift();
}

bool ShiftToggleKey::isOn(VirtualKeyboard *keyboard) {
    return keyboard->isShiftOn();
}

void SwitchKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "SwitchKey pushed: " << label(keyboard);

    // This may be used for changing key layouts.
    // Changin key layouts must be executed on key-release,
    // because VirtualKeyboard has `pushingKey_` pointer.
    if (!isRelease) {
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

    // check this key has been pushed and released in order to prevent miss click this key.
    if (dynamic_cast<LanguageSwitchKey *>(keyboard->pushingKey())) {
        keyboard->switchLanguage();
    }

    keyboard->updateInputPanel();
}

}
