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
    TextRomaji,
    TextJisKana,
    Mark,
};

static const std::string hankakuImeName = "keyboard-us";
static const std::string actionNameOfAnthyRomaji = "anthy-typing-method-romaji";
static const std::string actionNameOfAnthyKana = "anthy-typing-method-kana";
static const std::string actionNameOfAnthyTypingMethod = "anthy-typing-method";

class AnthyKeyboard : public I18nKeyboard {
public:
    KeyboardType type() const override { return KeyboardType::Anthy; };
    const char *label() const override { return "JP"; }
    void updateKeys() override;
    void syncState(
        VirtualKeyboard *keyboard,
        const std::string &currentInputMethodName
    ) override;

    void switchMode(VirtualKeyboard *keyboard);
    AnthyKeyboardMode mode() const { return mode_; }
    bool isZenkakuOn() const { return isZenkakuOn_; }
    void toggleZenkakuHankaku(VirtualKeyboard *keyboard);

protected:
    std::vector<std::string> otherNecessaryImeList() override { return { hankakuImeName }; }

private:
    void setTextRomajiKeys();
    void setMarkKeys();
    void setTextJisKanaKeys();
    AnthyKeyboardMode mode_ = AnthyKeyboardMode::TextRomaji;
    bool isZenkakuOn_ = true;
};

class AnthyMarkKey : public MarkKey {
public:
    AnthyMarkKey(
        const std::string &name,
        const std::string &hankakuMark,
        const std::string &zenkakuMark
    ) : MarkKey("", name),
        hankakuMark_(hankakuMark),
        zenkakuMark_(zenkakuMark) {}
    const char* label(VirtualKeyboard *keyboard) const override;

private:
    const std::string hankakuMark_;
    const std::string zenkakuMark_;
};

class AnthyKanaKey : public NormalKey {
public:
    AnthyKanaKey(
        const std::string &label,
        uint32_t code,
        const std::string &upperLabel,
        const std::string &name,
        const std::string &upperName,
        bool isNumberKey = false
    ) : NormalKey(
            label,
            code,
            upperLabel,
            name,
            upperName
        ),
        isNumberKey_(isNumberKey) {
        setCustomLayout(0.8);
    }

    const char *label(VirtualKeyboard *keyboard) const override;

private:
    typedef NormalKey super;
    bool isNumberKey_;
};

class AnthyKanaNumPadKey : public NumberKey {
public:
    AnthyKanaNumPadKey(const std::string &number) : NumberKey(number) {}
    void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) override;
};

class ZenkakuHankakuKey : public ToggleKey {
public:
    ZenkakuHankakuKey() {
        setFontSize(18);
    }
    const char* label(VirtualKeyboard *) const override { return "全角"; }

protected:
    void toggle(VirtualKeyboard *keyboard, InputContext *inputContext) override;
    bool isOn(VirtualKeyboard *keyboard) override;
};

class AnthyModeSwitchKey : public SwitchKey {
public:
    AnthyModeSwitchKey() : SwitchKey() {}
    const char* label(VirtualKeyboard *) const override { return "Aか#"; }

protected:
    int numberOfStates() const override { return 3; }
    const char *stateLabel(int index) const override {
        switch (index) {
        case 0:
            return "A";
        case 1:
            return "か";
        case 2:
            return "#";
        default:
            break;
        }
        return "";
    }
    void switchState(VirtualKeyboard *keyboard, InputContext *inputContext) override;
    int currentIndex(VirtualKeyboard *keyboard) override;
};

} // namespace classicui
} // namespace fcitx

#endif // _FCITX_UI_CLASSIC_VIRTUALKEYBOARDANTHY_H_
