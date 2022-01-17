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
 * Base class that provides function to convert to fcitx::key.
 * The name of key is in keynametable.h and is used for taking the corresponding sym.
 */
class KeyByNameAndCode : public VirtualKey {
protected:
    KeyByNameAndCode(
        const std::string &name,
        uint32_t code,
        const std::string &upperName = ""
    ) : name_(name), upperName_(upperName), code_(code) {}

    const char* keyName(bool withShift = false) const {
        if (withShift) {
            const auto baseSymName = upperName_.empty() ? name_ : upperName_;
            return ("SHIFT_" + baseSymName).c_str();
        }
        return name_.c_str();
    };

    fcitx::Key convert(bool withShift = false) const {
        const auto keyFromName = fcitx::Key(keyName(withShift));
        return fcitx::Key(keyFromName.sym(), keyFromName.states(), code_);
    }

    
    /// Be used in converting to Fcitx::Key.
    /// Corresponding to keyNameList in keynametable.h.
    const std::string name_;

    /// Be used in converting to Fcitx::Key with shift on.
    /// Corresponding to keyNameList in keynametable.h.
    /// If this is empty, `name_` with `SHIFT_` prefix is used for the key name,
    /// which is converted to the state of the shift modifier.
    const std::string upperName_;

    const uint32_t code_;
};

/*
 * Normal key, which can act the same way as physical keys.
 */
class NormalKey : public KeyByNameAndCode {
    public:
    NormalKey(
        const std::string &label,
        uint32_t code,
        const std::string &upperLabel = "",
        const std::string &name = "",
        const std::string &upperName = ""
    ) : KeyByNameAndCode(
            name.empty() ? label : name,
            code,
            upperName.empty() ? upperLabel : upperName
        ),
        label_(label),
        upperLabel_(upperLabel) {}
    virtual const char* label(VirtualKeyboard *keyboard) const override;
    virtual void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;

private:
    /// Text for display.
    const std::string label_;
    const std::string upperLabel_;
};

/*
 * Key for inputting marks.
 * Without `name` value, this simply inputs the label texts directly.
 * With `name` value, this sends the event to IME first.
 */
class MarkKey : public VirtualKey {
public:
    MarkKey(
        const std::string &label,
        const std::string &name = ""
    ) : label_(label),
        name_(name) {}
    virtual const char* label(VirtualKeyboard *keyboard) const override;
    virtual void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;

private:
    bool sendKeyEventFirst() const { return !name_.empty(); }

    /// Text for display, and commit-string.
    const std::string label_;

    /// Be used for deciding KeySym in converting to Fcitx::Key.
    /// Corresponding to keyNameList in keynametable.h.
    const std::string name_;
};

/*
 * Key for inputting numbers. This is similar to MarkKey, but this sends the number to IME first.
 * If there are selectable candidates in IME, the number is used for selecting them.
 */
class NumberKey : public VirtualKey {
public:
    NumberKey(const std::string &number) : number_(number) {}
    virtual const char* label(VirtualKeyboard *keyboard) const override;
    virtual void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;

private:
    const std::string number_;
};

class SpaceKey : public NormalKey {
public:
    SpaceKey() : NormalKey("", 65, "", "space") {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontColor({1.0, 1.0, 1.0});
    };
};

class NormalEnterKey : public NormalKey {
public:
    NormalEnterKey() : NormalKey("Enter", 36, "", "Return") {
        setCustomBackgroundColor({0.2, 0.7, 0.6});
        setFontColor({1.0, 1.0, 1.0});
    };
};

class NormalBackSpaceKey : public NormalKey {
public:
    NormalBackSpaceKey() : NormalKey("Back", 22, "", "BackSpace") {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontColor({1.0, 1.0, 1.0});
    };
};

class UpKey : public NormalKey {
public:
    UpKey() : NormalKey(u8"\u2191", 111, "", "Up") {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontColor({1.0, 1.0, 1.0});
    };
};

class LeftKey : public NormalKey {
public:
    LeftKey() : NormalKey(u8"\u2190", 113, "", "Left") {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontColor({1.0, 1.0, 1.0});
    };
};

class DownKey : public NormalKey {
public:
    DownKey() : NormalKey(u8"\u2193", 116, "", "Down") {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontColor({1.0, 1.0, 1.0});
    };
};

class RightKey : public NormalKey {
public:
    RightKey() : NormalKey(u8"\u2192", 114, "", "Right") {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontColor({1.0, 1.0, 1.0});
    };
};

/*
 * TODO Delete this class
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

/// TODO Delete this class
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

/* TODO Delete this class
 * Keys like enter and arrow keys that can not use commit-string and need to forward.
 */
class ForwardKey : public KeyByName {
public:
    ForwardKey(std::string keyName, std::string label, bool tryToSendKeyEventFirst = true)
        : KeyByName(keyName), label_(label), tryToSendKeyEventFirst_(tryToSendKeyEventFirst) {}
    virtual const char* label(VirtualKeyboard *) const override { return label_.c_str(); }
    virtual void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;

private:
    const std::string label_;
    bool tryToSendKeyEventFirst_ = true;

    /*
     * Key release must be forwarded only when key push had been forwarded in advance.
     */
    bool canForwardKeyRelease_ = false;
};

/// TODO Delete this class
class EnterKey : public ForwardKey {
public:
    EnterKey(bool tryToSendKeyEventFirst = true)
        : ForwardKey("Return", "Enter", tryToSendKeyEventFirst) {
        setCustomBackgroundColor({0.2, 0.7, 0.6});
        setFontColor({1.0, 1.0, 1.0});
    };
};

/// TODO Delete this class
class BackSpaceKey : public ForwardKey {
public:
    BackSpaceKey(bool tryToSendKeyEventFirst = true)
        : ForwardKey("BackSpace", "Back", tryToSendKeyEventFirst) {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontColor({1.0, 1.0, 1.0});
    };
};

/// TODO Delete this class
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

class SwitchKey : public VirtualKey {
public:
    SwitchKey() {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
    }
    virtual const char *label(VirtualKeyboard *) const = 0;
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;
    void paintLabel(VirtualKeyboard *keyboard, cairo_t *cr) override;

protected:
    virtual int numberOfStates() const = 0;
    virtual const char *stateLabel(int index) const = 0;
    virtual void switchState(VirtualKeyboard *keyboard, InputContext *inputContext) = 0;
    virtual int currentIndex(VirtualKeyboard *keyboard) = 0;
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
