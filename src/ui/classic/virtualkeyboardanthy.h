/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_UI_CLASSIC_VIRTUALKEYBOARDANTHY_H_
#define _FCITX_UI_CLASSIC_VIRTUALKEYBOARDANTHY_H_

#include "virtualkeyboard.h"
#include "virtualkeyboardi18n.h"
#include "virtualkeygeneral.h"

namespace fcitx {
namespace classicui {

enum class AnthyKeyboardMode {
    Text,
    Mark,
};

static const std::string hankakuImeName = "keyboard-us";

class AnthyKeyboard : public I18nKeyboard {
public:
    KeyboardType type() const override { return KeyboardType::Anthy; };
    const char *label() const override { return "JP"; }
    void updateKeys() override;
    std::vector<std::string> otherNecessaryImeList() override { return { hankakuImeName }; }
    void syncState(std::string currentInputMethodName) override;

    void switchMode();
    AnthyKeyboardMode mode() const { return mode_; }
    bool isZenkakuOn() const { return isZenkakuOn_; }
    void toggleZenkakuHankaku(VirtualKeyboard *keyboard);

private:
    void setTextKeys();
    void setMarkKeys();
    AnthyKeyboardMode mode_ = AnthyKeyboardMode::Text;
    bool isZenkakuOn_ = true;
};

class AnthyTextKey : public TextKey {
public:
    AnthyTextKey(std::string text, std::string upperText = "", std::string keyName = "",
        std::string upperKeyName = "")
        : TextKey(text, upperText, keyName, upperKeyName) {}
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;
};

class AnthyMarkKey : public KeyByName {
public:
    AnthyMarkKey(std::string keyName, std::string hankakuMark, std::string zenkakuMark)
        : KeyByName(keyName), hankakuMark_(hankakuMark), zenkakuMark_(zenkakuMark) {}
    const char* label(VirtualKeyboard *keyboard) const override;
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;

private:
    const std::string hankakuMark_;
    const std::string zenkakuMark_;
};

class ZenkakuHankakuKey : public ToggleKey {
public:
    ZenkakuHankakuKey() : ToggleKey(toggle, isOn) {
        setFontSize(18);
    }
    const char* label(VirtualKeyboard *) const override { return "全角"; }

private:
    static void toggle(VirtualKeyboard *keyboard, InputContext *inputContext);
    static bool isOn(VirtualKeyboard *keyboard);
};

class AnthyModeSwitchKey : public VirtualKey {
public:
    AnthyModeSwitchKey() {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
    }
    const char* label(VirtualKeyboard *) const override { return "A#"; }
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;
    void paintLabel(VirtualKeyboard *keyboard, cairo_t *cr) override;
};

} // namespace classicui
} // namespace fcitx

#endif // _FCITX_UI_CLASSIC_VIRTUALKEYBOARDANTHY_H_
