/*
 * SPDX-FileCopyrightText: 2022-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_UI_CLASSIC_VIRTUALKEYBOARDCHEWING_H_
#define _FCITX_UI_CLASSIC_VIRTUALKEYBOARDCHEWING_H_

#include "virtualkeyboard.h"
#include "virtualkeyboardi18n.h"
#include "virtualkeygeneral.h"

namespace fcitx {
namespace classicui {

enum class ChewingKeyboardMode {
    Text,
    Mark,
};

class ChewingKeyboard : public I18nKeyboard {
public:
    KeyboardType type() const override { return KeyboardType::Chewing; };
    const char *label() const override { return "TW"; }
    void updateKeys() override;
    void switchMode();
    ChewingKeyboardMode mode() const { return mode_; }
    bool isAdditionalMarkOn() const { return isAdditionalMarkOn_; }
    void toggleMark();

private:
    void setTextKeys();
    void setMarkKeys();
    void setAdditionalMarkKeys();
    ChewingKeyboardMode mode_ = ChewingKeyboardMode::Text;
    bool isAdditionalMarkOn_ = false;
};

class ChewingTextKey : public TextKey {
public:
    ChewingTextKey(std::string text, std::string keyName)
        : TextKey(text, "", keyName, "") {}
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;
};

class ChewingNumberKey : public TextKey {
public:
    ChewingNumberKey(std::string keyName, std::string text = "")
        : TextKey(
            text.empty() ? keyName : text,
            "",
            keyName,
            ""
        ),
        inputNumberUsually_(text.empty())
        {}
    const char* label(VirtualKeyboard *keyboard) const override;
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;

private:
    typedef TextKey super;
    //** Numpad-keys inputs the number usually, but `ㄅ`-key inputs `ㄅ` usually. */
    const bool inputNumberUsually_;
};

class ChewingEnterKey : public EnterKey {
public:
    ChewingEnterKey() {
        setFontSize(18);
    }
    const char* label(VirtualKeyboard *keyboard) const override;
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;

private:
    typedef EnterKey super;
};

class ChewingSpaceKey : public KeyByName {
public:
    ChewingSpaceKey() : KeyByName("space") {
        setFontSize(18);
        setFontColor({1.0, 1.0, 1.0});
        setCustomBackgroundColor({0.3, 0.3, 0.3});
    }
    const char* label(VirtualKeyboard *keyboard) const override;
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;
};

class ChewingModeSwitchKey : public SwitchKey {
public:
    ChewingModeSwitchKey() : SwitchKey() {}
    const char* label(VirtualKeyboard *) const override { return "注#"; }

protected:
    int numberOfStates() const override { return 2; }
    const char *stateLabel(int index) const override {
        return index == 0 ? "注" : "#";
    }
    void switchState(VirtualKeyboard *keyboard, InputContext *inputContext) override;
    int currentIndex(VirtualKeyboard *keyboard) override;
};

class ChewingMarkToggleKey : public VirtualKey {
public:
    ChewingMarkToggleKey() {
        setFontSize(18);
        setFontColor({1.0, 1.0, 1.0});
        setCustomBackgroundColor({0.3, 0.3, 0.3});
    }
    const char* label(VirtualKeyboard *keyboard) const override;
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;
};

} // namespace classicui
} // namespace fcitx

#endif // _FCITX_UI_CLASSIC_VIRTUALKEYBOARDCHEWING_H_
