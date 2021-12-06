/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_UI_CLASSIC_VIRTUALKEYBOARDPINYIN_H_
#define _FCITX_UI_CLASSIC_VIRTUALKEYBOARDPINYIN_H_

#include "virtualkeyboard.h"
#include "virtualkeyboardi18n.h"
#include "virtualkeygeneral.h"

namespace fcitx {
namespace classicui {

enum class PinyinKeyboardMode {
    Text,
    Mark,
};

class PinyinKeyboard : public I18nKeyboard {
public:
    KeyboardType type() const override { return KeyboardType::Pinyin; };
    const char *label() const override { return "CH"; }
    void updateKeys() override;
    void switchMode();
    PinyinKeyboardMode mode() const { return mode_; }
    bool isAdditionalMarkOn() const { return isAdditionalMarkOn_; }
    void toggleMark();

private:
    void setTextKeys();
    void setMarkKeys();
    void setAdditionalMarkKeys();
    PinyinKeyboardMode mode_ = PinyinKeyboardMode::Text;
    bool isAdditionalMarkOn_ = false;
};

class PinyinTextKey : public TextKey {
public:
    PinyinTextKey(std::string text, std::string upperText = "", std::string keyName = "",
        std::string upperKeyName = "")
        : TextKey(text, upperText, keyName, upperKeyName) {}
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;
};

class PinyinMarkKey : public VirtualKey {
public:
    PinyinMarkKey(std::string mark) : mark_(mark) {}
    const char* label(VirtualKeyboard *keyboard) const override;
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;

private:
    std::string mark_;
};

class PinyinSpaceKey : public KeyByName {
public:
    PinyinSpaceKey() : KeyByName("space") {
        setFontSize(18);
        setFontColor({1.0, 1.0, 1.0});
        setCustomBackgroundColor({0.3, 0.3, 0.3});
    }
    const char* label(VirtualKeyboard *keyboard) const override;
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;
};

class PinyinEnterKey : public EnterKey {
public:
    PinyinEnterKey() {
        setFontSize(18);
    }
    const char* label(VirtualKeyboard *keyboard) const override;
};

class PinyinModeSwitchKey : public VirtualKey {
public:
    PinyinModeSwitchKey() {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
    }
    const char* label(VirtualKeyboard *) const override { return "A#"; }
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;
    void paintLabel(VirtualKeyboard *keyboard, cairo_t *cr) override;
};

class PinyinMarkToggleKey : public VirtualKey {
public:
    PinyinMarkToggleKey() {
        setFontSize(18);
        setFontColor({1.0, 1.0, 1.0});
        setCustomBackgroundColor({0.3, 0.3, 0.3});
    }
    const char* label(VirtualKeyboard *keyboard) const override;
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;
};

} // namespace classicui
} // namespace fcitx

#endif // _FCITX_UI_CLASSIC_VIRTUALKEYBOARDPINYIN_H_
