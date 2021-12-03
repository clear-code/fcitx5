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
    void updateKeys() override;
    std::vector<std::string> otherNecessaryImeList() override { return { hankakuImeName }; }
    void syncState(std::string currentInputMethodName) override;

    void switchMode();
    AnthyKeyboardMode mode() const { return mode_; }
    bool isZenkakuOn() const { return isZenkakuOn_; }
    void toggleZenkakuHankaku(Keyboard *keyboard);

private:
    void setTextKeys();
    void setMarkKeys();
    AnthyKeyboardMode mode_ = AnthyKeyboardMode::Text;
    bool isZenkakuOn_ = true;
};

class AnthyTextKey : public TextKey {
public:
    AnthyTextKey(std::string keyName, std::string text, std::string upperText = "")
        : TextKey(keyName, text, upperText) {};
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) override;

private:
    /*
     * Text for display, and commit-string.
     */
    const std::string text_;
    const std::string upperText_;
};

class AnthyMarkKey : public KeyByName {
public:
    AnthyMarkKey(std::string keyName, std::string hankakuMark, std::string zenkakuMark)
        : KeyByName(keyName), hankakuMark_(hankakuMark), zenkakuMark_(zenkakuMark) {};
    const char* label(Keyboard *keyboard) const override;
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) override;

private:
    const std::string hankakuMark_;
    const std::string zenkakuMark_;
};

class ZenkakuHankakuKey : public Key {
public:
    ZenkakuHankakuKey() {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontSize(18);
    }
    const char* label(Keyboard *) const override { return "全角"; }
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) override;
    void paintLabel(Keyboard *keyboard, cairo_t *cr) override;
};

class AnthyModeSwitchKey : public Key {
public:
    AnthyModeSwitchKey() {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
    }
    const char* label(Keyboard *) const override { return "A#"; }
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) override;
    void paintLabel(Keyboard *keyboard, cairo_t *cr) override;
};

} // namespace classicui
} // namespace fcitx

#endif // _FCITX_UI_CLASSIC_VIRTUALKEYBOARDANTHY_H_
