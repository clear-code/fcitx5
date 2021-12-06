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
class DummyKey : public VirtualKey {
public:
    DummyKey() {
        visible_ = false;
    }
    const char* label(VirtualKeyboard *) const override { return ""; }
    void click(VirtualKeyboard *, InputContext *, bool) override {}
};

/*
 * Key that is visible, but do not work.
 */
class EmptyKey : public VirtualKey {
public:
    const char* label(VirtualKeyboard *) const override { return ""; }
    void click(VirtualKeyboard *, InputContext *, bool) override {}
};

/*
 * Base class that provides function to convert to fcitx::key by keyname in keynametable.h.
 * Keyname corresponds to keysym, but not to keycode.
 */
class KeyByName : public VirtualKey {
protected:
    KeyByName(std::string keyName, std::string upperKeyName = "")
        : keyName_(keyName), upperKeyName_(upperKeyName) {}

    const char* keyName(bool withShift = false) const {
        if (withShift) {
            if (!upperKeyName_.empty()) {
                return upperKeyName_.c_str();
            }
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
    const std::string upperKeyName_;
};

class TextKey : public KeyByName {
public:
    TextKey(std::string text, std::string upperText = "", std::string keyName = "",
        std::string upperKeyName = "")
        : KeyByName(keyName.empty() ? text : keyName,
            upperKeyName.empty() ? upperText : upperKeyName),
          text_(text), upperText_(upperText) {}
    virtual const char* label(VirtualKeyboard *keyboard) const override;
    virtual void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;

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
    ForwardKey(std::string keyName, std::string label, bool tryToSendKeyEventFirst = true)
        : KeyByName(keyName), label_(label), tryToSendKeyEventFirst_(tryToSendKeyEventFirst) {}
    virtual const char* label(VirtualKeyboard *) const override { return label_.c_str(); }
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;

private:
    const std::string label_;
    bool tryToSendKeyEventFirst_ = true;

    /*
     * Key release must be forwarded only when key push had been forwarded in advance.
     */
    bool canForwardKeyRelease_ = false;
};

class EnterKey : public ForwardKey {
public:
    EnterKey(bool tryToSendKeyEventFirst = true)
        : ForwardKey("Return", "Enter", tryToSendKeyEventFirst) {
        setCustomBackgroundColor({0.2, 0.7, 0.6});
        setFontColor({1.0, 1.0, 1.0});
    };
};

class BackSpaceKey : public ForwardKey {
public:
    BackSpaceKey(bool tryToSendKeyEventFirst = true)
        : ForwardKey("BackSpace", "Back", tryToSendKeyEventFirst) {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontColor({1.0, 1.0, 1.0});
    };
};

class ArrowKey : public ForwardKey {
public:
    ArrowKey(std::string keyName, std::string label, bool tryToSendKeyEventFirst = true)
        : ForwardKey(keyName, label, tryToSendKeyEventFirst) {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontColor({1.0, 1.0, 1.0});
    };
};

class ToggleKey : public VirtualKey {
public:
    ToggleKey() {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
    }
    virtual const char* label(VirtualKeyboard *) const = 0;
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;
    void paintLabel(VirtualKeyboard *keyboard, cairo_t *cr) override;

protected:
    virtual void toggle(VirtualKeyboard *keyboard, InputContext *inputContext) = 0;
    virtual bool isOn(VirtualKeyboard *keyboard) = 0;
};

class ShiftToggleKey : public ToggleKey {
public:
    ShiftToggleKey() {}
    const char* label(VirtualKeyboard *) const override { return u8"\u21E7"; }

protected:
    void toggle(VirtualKeyboard *keyboard, InputContext *inputContext) override;
    bool isOn(VirtualKeyboard *keyboard) override;
};

class LanguageSwitchKey : public VirtualKey {
public:
    LanguageSwitchKey() {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontColor({1.0, 1.0, 1.0});
    }
    const char* label(VirtualKeyboard *keyboard) const override;
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;
};

} // namespace classicui
} // namespace fcitx

#endif // _FCITX_UI_CLASSIC_VIRTUALKEYGENERAL_H_
