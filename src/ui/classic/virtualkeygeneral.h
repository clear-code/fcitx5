/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_UI_CLASSIC_VIRTUALKEYGENERAL_H_
#define _FCITX_UI_CLASSIC_VIRTUALKEYGENERAL_H_

#include "virtualkeyboard.h"

namespace fcitx {
namespace classicui {

/*
 * For making empty space in keyboard layout.
 */
class DummyKey : public Key {
public:
    DummyKey() {
        visible_ = false;
    }
    const char* label(Keyboard *) const override { return ""; }
    void click(Keyboard *, InputContext *, bool) override {}
};

/*
 * Key that is visible, but do not work.
 */
class EmptyKey : public Key {
public:
    const char* label(Keyboard *) const override { return ""; }
    void click(Keyboard *, InputContext *, bool) override {}
};

/*
 * Base class that provides function to convert to fcitx::key by keyname in keynametable.h.
 * Keyname corresponds to keysym, but not to keycode.
 */
class KeyByName : public Key {
protected:
    KeyByName(std::string keyName) : keyName_(keyName) {}

    const char* keyName(bool withShift = false) const {
        if (withShift) {
            return ("SHIFT_" + keyName_).c_str();
        }
        return keyName_.c_str();
    };

    fcitx::Key convert(bool withShift = false) const {
        return fcitx::Key(keyName(withShift));
    }

    /*
     * Be used in converting to Fcitx::Key.
     * Corresponding to keyNameList in keynametable.h.
     */
    const std::string keyName_;
};

class TextKey : public KeyByName {
public:
    TextKey(std::string keyName, std::string text, std::string upperText = "")
            : KeyByName(keyName), text_(text), upperText_(upperText) {};
    virtual const char* label(Keyboard *keyboard) const override;
    virtual void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) override;

private:
    /*
     * Text for display, and commit-string.
     */
    const std::string text_;
    const std::string upperText_;
};

/*
 * Keys like enter and arrow keys that can not use commit-string and need to forward.
 */
class ForwardKey : public KeyByName {
public:
    ForwardKey(std::string keyName, std::string label) : KeyByName(keyName), label_(label) {}
    const char* label(Keyboard *) const override { return label_.c_str(); }
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) override;

private:
    const std::string label_;

    /*
     * Key release must be forwarded only when key push had been forwarded in advance.
     */
    bool canForwardKeyRelease_ = false;
};

class EnterKey : public ForwardKey {
public:
    EnterKey() : ForwardKey("Return", "Enter") {
        setCustomBackgroundColor({0.2, 0.7, 0.6});
        setFontColor({1.0, 1.0, 1.0});
    };
};

class BackSpaceKey : public ForwardKey {
public:
    BackSpaceKey() : ForwardKey("BackSpace", "Back") {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontColor({1.0, 1.0, 1.0});
    };
};

class ArrowKey : public ForwardKey {
public:
    ArrowKey(std::string keyName, std::string label) : ForwardKey(keyName, label) {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontColor({1.0, 1.0, 1.0});
    };
};

class ShiftToggleKey : public Key {
public:
    ShiftToggleKey() {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
    }
    const char* label(Keyboard *) const override { return u8"\u21E7"; }
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) override;
    void paintLabel(Keyboard *keyboard, cairo_t *cr) override;
};

class LanguageSwitchKey : public Key {
public:
    LanguageSwitchKey() {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontColor({1.0, 1.0, 1.0});
    }
    const char* label(Keyboard *keyboard) const override;
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) override;
};

} // namespace classicui
} // namespace fcitx

#endif // _FCITX_UI_CLASSIC_VIRTUALKEYGENERAL_H_
