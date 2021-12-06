/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_UI_CLASSIC_VIRTUALKEYBOARDI18N_H_
#define _FCITX_UI_CLASSIC_VIRTUALKEYBOARDI18N_H_

#include "common.h"
#include "fcitx/inputmethodmanager.h"

namespace fcitx {
namespace classicui {

enum class KeyboardType {
    Unknown,
    Anthy,
    Pinyin,
    Russian,
};

static std::map<KeyboardType, std::string> imeNames = {
    {KeyboardType::Unknown, ""},
    {KeyboardType::Anthy, "anthy"},
    {KeyboardType::Pinyin, "pinyin"},
    {KeyboardType::Russian, "keyboard-ru"},
};

class VirtualKey;
class I18nKeyboard {
public:
    virtual KeyboardType type() const = 0;
    virtual const char *label() const = 0;
    virtual void updateKeys() = 0;
    virtual std::vector<std::string> otherNecessaryImeList() { return {}; }
    virtual void syncState(std::string currentInputMethodName);
    std::vector<std::unique_ptr<VirtualKey>> &keys() { return keys_; }
protected:
    std::vector<std::unique_ptr<VirtualKey>> keys_;
};

class I18nKeyboardSelector {
public:
    I18nKeyboard *selectType(KeyboardType type);
    std::tuple<I18nKeyboard *, bool> select(std::string inputMethodName,
        std::vector<fcitx::InputMethodGroupItem> &inputMethodItems);
private:
    KeyboardType findType(std::string inputMethodName);
    bool containInputMethod(std::vector<fcitx::InputMethodGroupItem> &items,
                            std::string name);
};

class NullI18nKeyboard : public I18nKeyboard {
public:
    KeyboardType type() const override { return KeyboardType::Unknown; }
    const char *label() const override { return "X"; }
    void updateKeys() override {
        // do nothing
    }
};

} // namespace classicui
} // namespace fcitx

#endif // _FCITX_UI_CLASSIC_VIRTUALKEYBOARDI18N_H_
