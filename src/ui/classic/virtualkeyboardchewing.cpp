/*
 * SPDX-FileCopyrightText: 2022-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "virtualkeyboardchewing.h"

namespace fcitx::classicui {

void ChewingKeyboard::updateKeys() {
    if (mode_ == ChewingKeyboardMode::Text) {
        setTextKeys();
        return;
    }
    if (isAdditionalMarkOn()) {
        setAdditionalMarkKeys();
    } else {
        setMarkKeys();
    }
}

void ChewingKeyboard::switchMode() {
    if (mode_ == ChewingKeyboardMode::Text) {
        mode_ = ChewingKeyboardMode::Mark;
    } else {
        mode_ = ChewingKeyboardMode::Text;
    }

    updateKeys();
}

void ChewingKeyboard::toggleMark() {
    isAdditionalMarkOn_ = !isAdditionalMarkOn_;
    updateKeys();
}

void ChewingTextKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "ChewingTextKey pushed: " << label(keyboard);

    auto keyEvent = fcitx::KeyEvent(inputContext, convert(), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;
}

const char* ChewingNumberKey::label(VirtualKeyboard *keyboard) const {
    // `ㄅ` is NumberKey of `1`, and `1` is used for selecting a candidate while selecting candidates.
    // So need to change the label while selecting candidates.
    if (keyboard->isSeletingCandidates())
    {
        return keyName_.c_str();
    }

    return super::label(keyboard);
}

void ChewingNumberKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "ChewingNumberKey pushed";

    // In order to numpad-keys can select candidates too.
    if (keyboard->isSeletingCandidates() || !inputNumberUsually_) {
        auto keyEvent = fcitx::KeyEvent(inputContext, convert(), isRelease);
        auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
        FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;
        return;
    }

    super::click(keyboard, inputContext, isRelease);
}

const char* ChewingEnterKey::label(VirtualKeyboard *keyboard) const {
    return keyboard->isPreediting() ? "確認" : "換行";
}

void ChewingEnterKey::click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) {
    // In fcitx5-chewing, EnterKey does nothing while selecting candidates.
    // This is wierd because EnterKey displays `確認`.
    // So send `Up` key in order to cancel selecting.
    if (keyboard->isSeletingCandidates())
    {
        auto keyEvent = fcitx::KeyEvent(inputContext, fcitx::Key("Up"), isRelease);
        inputContext->keyEvent(keyEvent);
        return;
    }

    super::click(keyboard, inputContext, isRelease);
}

const char* ChewingSpaceKey::label(VirtualKeyboard *keyboard) const {
    return keyboard->isPreediting() ? "一聲/變換" : "空格";
}

void ChewingSpaceKey::click(VirtualKeyboard *, InputContext *inputContext, bool isRelease) {
    FCITX_KEYBOARD() << "ChewingSpaceKey pushed";

    if (isRelease) {
        return;
    }

    auto keyEvent = fcitx::KeyEvent(inputContext, convert(), isRelease);
    auto hasProcessedInIME = inputContext->keyEvent(keyEvent);
    FCITX_KEYBOARD() << "key event result: " << hasProcessedInIME;

    if(hasProcessedInIME) {
        return;
    }

    inputContext->commitString(" ");
}

void ChewingModeSwitchKey::switchState(VirtualKeyboard *keyboard, InputContext *) {
    keyboard->i18nKeyboard<ChewingKeyboard>()->switchMode();
    // Because this switching changes the size of the keyboard.
    keyboard->updateInputPanel();
}

int ChewingModeSwitchKey::currentIndex(VirtualKeyboard *keyboard) {
    if (keyboard->i18nKeyboard<ChewingKeyboard>()->mode() == ChewingKeyboardMode::Text) {
        return 0;
    }
    return 1;
}

const char* ChewingMarkToggleKey::label(VirtualKeyboard *keyboard) const {
    if (keyboard->i18nKeyboard<ChewingKeyboard>()->isAdditionalMarkOn()) {
        return "返回";
    }
    return "更多";
}

void ChewingMarkToggleKey::click(VirtualKeyboard *keyboard, InputContext *, bool isRelease) {
    FCITX_KEYBOARD() << "ChewingMarkToggleKey pushed: " << label(keyboard);
    if (isRelease) {
        return;
    }
    keyboard->i18nKeyboard<ChewingKeyboard>()->toggleMark();
}

void ChewingKeyboard::setTextKeys() {
    keys_.clear();
    keys_.emplace_back(new ChewingNumberKey("1", "ㄅ"));
    keys_.emplace_back(new ChewingNumberKey("2", "ㄉ"));
    // want to set Top-align to these labels: ˇ, `, ´, ˙,
    // but it is difficult because the number muse be Center-align...
    keys_.emplace_back(new ChewingNumberKey("3", "ˇ"));
    keys_.emplace_back(new ChewingNumberKey("4", "`"));
    keys_.emplace_back(new ChewingNumberKey("5", "ㄓ"));
    keys_.emplace_back(new ChewingNumberKey("6", "´"));
    keys_.emplace_back(new ChewingNumberKey("7", "˙"));
    keys_.emplace_back(new ChewingNumberKey("8", "ㄚ"));
    keys_.emplace_back(new ChewingNumberKey("9", "ㄞ"));
    keys_.emplace_back(new ChewingNumberKey("0", "ㄢ"));
    keys_.emplace_back(new ChewingTextKey("ㄦ", "-")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ChewingTextKey("ㄆ", "q"));
    keys_.emplace_back(new ChewingTextKey("ㄊ", "w"));
    keys_.emplace_back(new ChewingTextKey("ㄍ", "e"));
    keys_.emplace_back(new ChewingTextKey("ㄐ", "r"));
    keys_.emplace_back(new ChewingTextKey("ㄔ", "t"));
    keys_.emplace_back(new ChewingTextKey("ㄗ", "y"));
    keys_.emplace_back(new ChewingTextKey("ㄧ", "u"));
    keys_.emplace_back(new ChewingTextKey("ㄛ", "i"));
    keys_.emplace_back(new ChewingTextKey("ㄟ", "o"));
    keys_.emplace_back(new ChewingTextKey("ㄣ", "p"));
    keys_.emplace_back(new BackSpaceKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new ChewingNumberKey("7"));
    keys_.emplace_back(new ChewingNumberKey("8"));
    keys_.emplace_back(new ChewingNumberKey("9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ChewingTextKey("ㄇ", "a"));
    keys_.emplace_back(new ChewingTextKey("ㄋ", "s"));
    keys_.emplace_back(new ChewingTextKey("ㄎ", "d"));
    keys_.emplace_back(new ChewingTextKey("ㄑ", "f"));
    keys_.emplace_back(new ChewingTextKey("ㄕ", "g"));
    keys_.emplace_back(new ChewingTextKey("ㄘ", "h"));
    keys_.emplace_back(new ChewingTextKey("ㄨ", "j"));
    keys_.emplace_back(new ChewingTextKey("ㄜ", "k"));
    keys_.emplace_back(new ChewingTextKey("ㄠ", "l"));
    keys_.emplace_back(new ChewingTextKey("ㄤ", ";"));
    keys_.emplace_back(new ChewingEnterKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new ChewingNumberKey("4"));
    keys_.emplace_back(new ChewingNumberKey("5"));
    keys_.emplace_back(new ChewingNumberKey("6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ChewingTextKey("ㄈ", "z"));
    keys_.emplace_back(new ChewingTextKey("ㄌ", "x"));
    keys_.emplace_back(new ChewingTextKey("ㄏ", "c"));
    keys_.emplace_back(new ChewingTextKey("ㄒ", "v"));
    keys_.emplace_back(new ChewingTextKey("ㄖ", "b"));
    keys_.emplace_back(new ChewingTextKey("ㄙ", "n"));
    keys_.emplace_back(new ChewingTextKey("ㄩ", "m"));
    keys_.emplace_back(new ChewingTextKey("ㄝ", ","));
    keys_.emplace_back(new ChewingTextKey("ㄡ", "."));
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191"));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new ChewingNumberKey("1"));
    keys_.emplace_back(new ChewingNumberKey("2"));
    keys_.emplace_back(new ChewingNumberKey("3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ChewingModeSwitchKey());
    keys_.emplace_back(new TextKey("，")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new TextKey("。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new ChewingSpaceKey()); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new TextKey("！"));
    keys_.emplace_back(new TextKey("？"));
    keys_.emplace_back(new ChewingTextKey("ㄥ", "/"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190"));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193"));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new ChewingNumberKey("0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new TextKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

void ChewingKeyboard::setMarkKeys() {
    keys_.clear();
    keys_.emplace_back(new TextKey("“")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new TextKey("”")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new TextKey("‘")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new TextKey("’")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new TextKey("（"));
    keys_.emplace_back(new TextKey("）"));
    keys_.emplace_back(new TextKey("「"));
    keys_.emplace_back(new TextKey("」"));
    keys_.emplace_back(new TextKey("『"));
    keys_.emplace_back(new TextKey("』"));
    keys_.emplace_back(new BackSpaceKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("7"));
    keys_.emplace_back(new TextKey("8"));
    keys_.emplace_back(new TextKey("9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("《"));
    keys_.emplace_back(new TextKey("》"));
    keys_.emplace_back(new TextKey("〈"));
    keys_.emplace_back(new TextKey("〉"));
    keys_.emplace_back(new TextKey("："));
    keys_.emplace_back(new TextKey("；"));
    keys_.emplace_back(new TextKey("—"));
    keys_.emplace_back(new TextKey("…")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new TextKey("@"));
    keys_.emplace_back(new ChewingEnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("4"));
    keys_.emplace_back(new TextKey("5"));
    keys_.emplace_back(new TextKey("6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ChewingMarkToggleKey());
    keys_.emplace_back(new TextKey("·"));
    keys_.emplace_back(new TextKey("～"));
    keys_.emplace_back(new TextKey("￥"));
    keys_.emplace_back(new TextKey("$"));
    keys_.emplace_back(new TextKey("&"));
    keys_.emplace_back(new TextKey("|"));
    keys_.emplace_back(new TextKey("_")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new TextKey("、")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191"));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("1"));
    keys_.emplace_back(new TextKey("2"));
    keys_.emplace_back(new TextKey("3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ChewingModeSwitchKey());
    keys_.emplace_back(new TextKey("，")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new TextKey("。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new ChewingSpaceKey()); keys_.back()->setCustomLayout(3.0);
    keys_.emplace_back(new TextKey("！"));
    keys_.emplace_back(new TextKey("？"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190"));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193"));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new TextKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

void ChewingKeyboard::setAdditionalMarkKeys() {
    keys_.clear();
    keys_.emplace_back(new TextKey("["));
    keys_.emplace_back(new TextKey("]"));
    keys_.emplace_back(new TextKey("{"));
    keys_.emplace_back(new TextKey("}"));
    keys_.emplace_back(new TextKey("#"));
    keys_.emplace_back(new TextKey("%"));
    keys_.emplace_back(new TextKey("^"));
    keys_.emplace_back(new TextKey("*"));
    keys_.emplace_back(new TextKey("+"));
    keys_.emplace_back(new TextKey("="));
    keys_.emplace_back(new BackSpaceKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("7"));
    keys_.emplace_back(new TextKey("8"));
    keys_.emplace_back(new TextKey("9")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("/"));
    keys_.emplace_back(new TextKey("\\"));
    keys_.emplace_back(new TextKey(":"));
    keys_.emplace_back(new TextKey(";"));
    keys_.emplace_back(new TextKey("("));
    keys_.emplace_back(new TextKey(")"));
    keys_.emplace_back(new TextKey("-"));
    keys_.emplace_back(new TextKey("~"));
    keys_.emplace_back(new TextKey("@"));
    keys_.emplace_back(new ChewingEnterKey()); keys_.back()->setCustomLayout(1.5);
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("4"));
    keys_.emplace_back(new TextKey("5"));
    keys_.emplace_back(new TextKey("6")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ChewingMarkToggleKey());
    keys_.emplace_back(new TextKey("\"")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new TextKey("\'")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Top);
    keys_.emplace_back(new TextKey(u8"\u00A5"));
    keys_.emplace_back(new TextKey("!"));
    keys_.emplace_back(new TextKey("?"));
    keys_.emplace_back(new TextKey("<"));
    keys_.emplace_back(new TextKey(">"));
    keys_.emplace_back(new TextKey("、")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new ArrowKey("Up", u8"\u2191"));
    keys_.emplace_back(new LanguageSwitchKey());
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("1"));
    keys_.emplace_back(new TextKey("2"));
    keys_.emplace_back(new TextKey("3")); keys_.back()->setCustomLayout(1.0, true);

    keys_.emplace_back(new ChewingModeSwitchKey());
    keys_.emplace_back(new TextKey("，")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new TextKey("。")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
    keys_.emplace_back(new ChewingSpaceKey()); keys_.back()->setCustomLayout(3.0);
    keys_.emplace_back(new TextKey("！"));
    keys_.emplace_back(new TextKey("？"));
    keys_.emplace_back(new ArrowKey("Left", u8"\u2190"));
    keys_.emplace_back(new ArrowKey("Down", u8"\u2193"));
    keys_.emplace_back(new ArrowKey("Right", u8"\u2192"));
    keys_.emplace_back(new DummyKey()); keys_.back()->setCustomLayout(0.5);
    keys_.emplace_back(new TextKey("0")); keys_.back()->setCustomLayout(2.0);
    keys_.emplace_back(new TextKey(".")); keys_.back()->setLabelAlign(KeyLabelAlignVertical::Bottom);
}

}
