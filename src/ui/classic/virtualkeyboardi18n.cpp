/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "virtualkeyboardi18n.h"
#include "virtualkeyboard.h"
#include "virtualkeyboardanthy.h"
#include "virtualkeyboardpinyin.h"
#include "virtualkeyboardrussian.h"

namespace fcitx::classicui {

void I18nKeyboard::syncState(std::string) {}

I18nKeyboard *I18nKeyboardSelector::selectType(KeyboardType type) {
    // Add case here when adding new keyboard type.
    switch (type) {
    case KeyboardType::Anthy:
        return new AnthyKeyboard();
    case KeyboardType::Pinyin:
        return new PinyinKeyboard();
    case KeyboardType::Russian:
        return new RussianKeyboard();
    default:
        break;
    }
    return nullptr;
}

I18nKeyboard *I18nKeyboardSelector::select(std::string currentInputMethodName,
                                           std::vector<fcitx::InputMethodGroupItem> &inputMethodItems) {
    auto foundType = findType(currentInputMethodName);
    if (foundType == KeyboardType::Unknown) {
        return nullptr;
    }

    auto i18nKeyboard = selectType(foundType);

    for (const auto &anotherIme : i18nKeyboard->otherNecessaryImeList())
    {
        if (!containInputMethod(inputMethodItems, anotherIme)) {
            return nullptr;
        }
    }

    return i18nKeyboard;
}

KeyboardType I18nKeyboardSelector::findType(std::string inputMethodName) {
    for (const auto &[type, name] : imeNames)
    {
        if (inputMethodName == name) {
            return type;
        }
    }
    return KeyboardType::Unknown;
}

bool I18nKeyboardSelector::containInputMethod(std::vector<fcitx::InputMethodGroupItem> &items,
                                              std::string name) {
    auto iter = std::find_if(items.begin(), items.end(),
        [&name](const InputMethodGroupItem &item) {
            return item.name() == name;
        });
    return iter != items.end();
}

}
