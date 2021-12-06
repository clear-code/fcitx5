/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_UI_CLASSIC_VIRTUALKEYBOARDRUSSIAN_H_
#define _FCITX_UI_CLASSIC_VIRTUALKEYBOARDRUSSIAN_H_

#include "virtualkeyboard.h"
#include "virtualkeyboardi18n.h"
#include "virtualkeygeneral.h"

namespace fcitx {
namespace classicui {

enum class RussianKeyboardMode {
    Text,
    Mark,
};

class RussianKeyboard : public I18nKeyboard {
public:
    KeyboardType type() const override { return KeyboardType::Russian; };
    const char *label() const override { return "RU"; }
    void updateKeys() override;
    void switchMode();
    RussianKeyboardMode mode() const { return mode_; }
    bool isLatinTextOn() const { return isLatinTextOn_; }
    void toggleText();

private:
    void setCyrillicTextKeys();
    void setLatinTextKeys();
    void setMarkKeys();
    RussianKeyboardMode mode_ = RussianKeyboardMode::Text;
    bool isLatinTextOn_ = false;
};

class RussianDummyKey : public DummyKey {
public:
    RussianDummyKey(double witdh) : DummyKey() {
        width_ = witdh;
    }
};

class RussianShiftToggleKey : public ShiftToggleKey {
public:
    RussianShiftToggleKey() : ShiftToggleKey() {
        width_ = 40;
    }
};

class RussianTextKey : public TextKey {
public:
    RussianTextKey(std::string text, std::string upperText = "", std::string keyName = "",
        std::string upperKeyName = "")
        : TextKey(text, upperText, keyName, upperKeyName) {
        width_ = 50;
    }
};

class RussianModeSwitchKey : public SwitchKey {
public:
    RussianModeSwitchKey() : SwitchKey() {}
    const char* label(VirtualKeyboard *) const override { return "A#"; }

protected:
    const char *stateLabel(int index) const override {
        return index == 0 ? "A" : "#";
    }
    void switchState(VirtualKeyboard *keyboard, InputContext *inputContext) override;
    int currentIndex(VirtualKeyboard *keyboard) override;
};

class RussianTextToggleKey : public ToggleKey {
public:
    RussianTextToggleKey() {
        setFontSize(18);
    }
    const char* label(VirtualKeyboard *) const override { return "Latin"; }

protected:
    void toggle(VirtualKeyboard *keyboard, InputContext *inputContext) override;
    bool isOn(VirtualKeyboard *keyboard) override;
};

} // namespace classicui
} // namespace fcitx

#endif // _FCITX_UI_CLASSIC_VIRTUALKEYBOARDRUSSIAN_H_
