/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_UI_CLASSIC_VIRTUALKEYBOARDHANGUL_H_
#define _FCITX_UI_CLASSIC_VIRTUALKEYBOARDHANGUL_H_

#include "virtualkeyboard.h"
#include "virtualkeyboardi18n.h"
#include "virtualkeygeneral.h"

namespace fcitx {
namespace classicui {

enum class HangulKeyboardMode {
    Text,
    Mark,
};

class HangulKeyboard : public I18nKeyboard {
public:
    KeyboardType type() const override { return KeyboardType::Hangul; };
    const char *label() const override { return "KR"; }
    void updateKeys() override;
    void switchMode();
    HangulKeyboardMode mode() const { return mode_; }

private:
    void setTextKeys();
    void setMarkKeys();
    HangulKeyboardMode mode_ = HangulKeyboardMode::Text;
};

class HangulModeSwitchKey : public SwitchKey {
public:
    HangulModeSwitchKey() : SwitchKey() {}
    const char* label(VirtualKeyboard *) const override { return "A#"; }

protected:
    const char *stateLabel(int index) const override {
        return index == 0 ? "A" : "#";
    }
    void switchState(VirtualKeyboard *keyboard, InputContext *inputContext) override;
    int currentIndex(VirtualKeyboard *keyboard) override;
};

class HangulTextKey : public TextKey {
public:
    HangulTextKey(std::string text, std::string keyName, std::string upperText = "",
        std::string upperKeyName = "")
        : TextKey(text, upperText, keyName, upperKeyName) {
        setFontSize(32);
    }
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;
};

class HangulSpaceKey : public KeyByName {
public:
    HangulSpaceKey() : KeyByName("space") {
        setFontSize(18);
        setFontColor({1.0, 1.0, 1.0});
        setCustomBackgroundColor({0.3, 0.3, 0.3});
    }
    const char* label(VirtualKeyboard *) const override { return ""; }
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;
};

} // namespace classicui
} // namespace fcitx

#endif // _FCITX_UI_CLASSIC_VIRTUALKEYBOARDHANGUL_H_
