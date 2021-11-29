/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_UI_CLASSIC_VIRTUALKEYBOARD_H_
#define _FCITX_UI_CLASSIC_VIRTUALKEYBOARD_H_

#include "common.h"
#include <cairo/cairo.h>
#include <pango/pango.h>
#include "fcitx/inputcontext.h"
#include "fcitx-utils/log.h"

namespace fcitx {
namespace classicui {

class Keyboard;
class Key {
public:
    virtual const char* label(Keyboard *keyboard) const = 0;
    virtual void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) const = 0;

    void setRegion(int x, int y) {
        region_
            .setPosition(x, y)
            .setSize(width_, height_);
    }
    bool contains(int x, int y) const { return region_.contains(x, y); }

    void setCustomLayout(double scale, bool newLine = false) {
        newLine_ = newLine;
        width_ *= scale;
    }

    double width_ = 60;
    double height_ = 50;
    bool newLine_ = false;
    bool visible_ = true;

private:
    Rect region_;
};

/*
 * For making empty space in keyboard layout.
 */
class DummyKey : public Key {
public:
    DummyKey() {
        visible_ = false;
    }
    const char* label(Keyboard *) const override { return ""; }
    void click(Keyboard *, InputContext *, bool) const override {}
};

/*
 * Key that is visible, but do not work.
 */
class EmptyKey : public Key {
public:
    const char* label(Keyboard *) const override { return ""; }
    void click(Keyboard *, InputContext *, bool) const override {}
};

/*
 * Base class that provides function to convert to fcitx::key by keyname in keynametable.h.
 * Keyname corresponds to keysym, but not to keycode.
 */
class KeyByName : public Key {
protected:
    KeyByName(std::string keyName) : keyName_(keyName) {}
    const char* keyName() const { return keyName_.c_str(); };
    fcitx::Key convert() const { return fcitx::Key(keyName()); }

    /*
     * Be used in converting to Fcitx::Key.
     * Corresponding to keyNameList in keynametable.h.
     */
    const std::string keyName_;
};

class TextKey : public KeyByName {
public:
    TextKey(std::string keyName, std::string text, std::string upperText = "")
            : KeyByName(keyName), text_(text), upperText_(upperText) {};
    const char* label(Keyboard *keyboard) const override;
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) const override;

private:
    /*
     * Text for display, and commit-string.
     */
    const std::string text_;
    const std::string upperText_;
};

class MarkKey : public KeyByName {
public:
    MarkKey(std::string keyName, std::string hankakuMark, std::string zenkakuMark)
            : KeyByName(keyName), hankakuMark_(hankakuMark), zenkakuMark_(zenkakuMark) {};
    const char* label(Keyboard *keyboard) const override;
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) const override;

private:
    const std::string hankakuMark_;
    const std::string zenkakuMark_;
};

/*
 * Keys like enter and arrow keys that can not use commit-string and need to forward.
 */
class ForwardKey : public KeyByName {
public:
    ForwardKey(std::string keyName, std::string label) : KeyByName(keyName), label_(label) {}
    const char* label(Keyboard *) const override { return label_.c_str(); }
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) const override;

private:
    const std::string label_;
};

class ZenkakuHankakuKey : public Key {
public:
    const char* label(Keyboard *keyboard) const override;
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) const override;
};

class UpperToggleKey : public Key {
public:
    const char* label(Keyboard *keyboard) const override;
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) const override;
};

class ModeSwitchKey : public Key {
public:
    const char* label(Keyboard *keyboard) const override;
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) const override;
};

enum class KeyboardMode {
    ZenkakuText,
    HankakuText,
    Mark,
};

class Keyboard {
public:
    Keyboard();
    void paint(cairo_t *cr, unsigned int offsetX, unsigned int offsetY);
    bool click(InputContext *inputContext, int x, int y, bool isRelease);
    void setTextKeys(bool isZenkakuMode);
    void setMarkKeys();
    std::pair<unsigned int, unsigned int> size();
    unsigned int marginX() { return 15; }
    unsigned int marginY() { return 5; }

    std::vector<std::unique_ptr<Key>> keys_;
    Key *pushingKey_;
    bool isAnyKeyPushing_ = false;

    KeyboardMode mode_ = KeyboardMode::ZenkakuText;
    bool useUpperHankakuText_ = false;
    bool useZenkakuMark_ = false;

private:
    void paintOneKey(cairo_t *cr, Key *key);
    std::tuple<Key *, bool> findClickedKey(int x, int y);
};

} // namespace classicui
} // namespace fcitx

FCITX_DECLARE_LOG_CATEGORY(keyboard);

#define FCITX_KEYBOARD() FCITX_LOGC(::keyboard, Debug)

#endif // _FCITX_UI_CLASSIC_VIRTUALKEYBOARD_H_
