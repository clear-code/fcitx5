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
#include "fcitx/instance.h"
#include "fcitx/inputcontext.h"
#include <fcitx-utils/event.h>
#include "fcitx-utils/log.h"

namespace fcitx {
namespace classicui {

enum KeyLabelAlignVertical {
    Center,
    Bottom,
};

class Keyboard;
class Key {
public:
    virtual const char* label(Keyboard *keyboard) const = 0;
    virtual void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) const = 0;
    virtual void paintLabel(Keyboard *keyboard, cairo_t *cr);
    void paintBackground(cairo_t *cr, bool highlight);

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

    void setFontColor(std::tuple<double, double, double> colorRgb) {
        fontColorRgb_ = colorRgb;
    }

    void setFontSize(double size) {
        fontSize_ = size;
    }

    void setCustomBackgroundColor(std::tuple<double, double, double> colorRgb) {
        useCustomBackgroundColor_ = true;
        customBackgroundColorRgb_ = colorRgb;
    }

    double labelOffsetX(cairo_text_extents_t extents) const {
        return (width_ - extents.width) / 2 - extents.x_bearing;
    }

    double labelOffsetY(cairo_text_extents_t extents) const {
        if (labelAlignVertical_ == KeyLabelAlignVertical::Bottom) {
            const auto bottomMargin = 10;
            return height_ - extents.height - extents.y_bearing - bottomMargin;
        }
        return (height_ - extents.height) / 2 - extents.y_bearing;
    }

    void setLabelAlign(KeyLabelAlignVertical vertical) {
        labelAlignVertical_ = vertical;
    }

    double width_ = 60;
    double height_ = 50;
    bool newLine_ = false;
    bool visible_ = true;

protected:
    Rect region_;
    double fontSize_ = 22;
    std::tuple<double, double, double> fontColorRgb_ = {0.3, 0.35, 0.4};
    KeyLabelAlignVertical labelAlignVertical_ = KeyLabelAlignVertical::Center;
    bool useCustomBackgroundColor_ = false;
    std::tuple<double, double, double> customBackgroundColorRgb_ = {0, 0, 0};
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

    const char* keyName(bool withShift = false) const {
        if (withShift) {
            return ("SHIFT_" + keyName_).c_str();
        }
        return keyName_.c_str();
    };

    fcitx::Key convert(bool withShift = false) const {
        return fcitx::Key(keyName(withShift));
    }

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

class EnterKey : public ForwardKey {
public:
    EnterKey() : ForwardKey("Return", "Enter") {
        setCustomBackgroundColor({0.2, 0.7, 0.6});
        setFontColor({1.0, 1.0, 1.0});
    };
};

class BackSpaceKey : public ForwardKey {
public:
    BackSpaceKey() : ForwardKey("BackSpace", "Back") {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontColor({1.0, 1.0, 1.0});
    };
};

class ArrowKey : public ForwardKey {
public:
    ArrowKey(std::string keyName, std::string label) : ForwardKey(keyName, label) {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontColor({1.0, 1.0, 1.0});
    };
};

class ZenkakuHankakuKey : public Key {
public:
    ZenkakuHankakuKey() {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
        setFontSize(18);
    }
    const char* label(Keyboard *) const override { return "全角"; }
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) const override;
    void paintLabel(Keyboard *keyboard, cairo_t *cr) override;
};

class ShiftToggleKey : public Key {
public:
    ShiftToggleKey() {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
    }
    const char* label(Keyboard *) const override { return u8"\u21E7"; }
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) const override;
    void paintLabel(Keyboard *keyboard, cairo_t *cr) override;
};

class ModeSwitchKey : public Key {
public:
    ModeSwitchKey() {
        setCustomBackgroundColor({0.3, 0.3, 0.3});
    }
    const char* label(Keyboard *) const override { return "A#"; }
    void click(Keyboard *keyboard, InputContext *inputContext, bool isRelease) const override;
    void paintLabel(Keyboard *keyboard, cairo_t *cr) override;
};

enum class KeyboardMode {
    Text,
    Mark,
};

class Keyboard {
public:
    Keyboard(Instance *instance);
    void paint(cairo_t *cr, unsigned int offsetX, unsigned int offsetY);
    bool click(InputContext *inputContext, int x, int y, bool isRelease);
    void setTextKeys();
    void setMarkKeys();
    std::pair<unsigned int, unsigned int> size();
    unsigned int marginX() { return 15; }
    unsigned int marginY() { return 6; }

protected:
    void onKeyRepeat();

    Instance *instance_;
    std::vector<std::unique_ptr<Key>> keys_;
    Key *pushingKey_ = nullptr;
    TrackableObjectReference<InputContext> lastInputContext_;
    std::unique_ptr<EventSourceTime> repeatKeyTimer_;
    int32_t repeatRate_ = 40, repeatDelay_ = 400;

public: // TODO: Should be moved to protected
    KeyboardMode mode_ = KeyboardMode::Text;
    bool isShiftOn_ = false;
    bool isZenkakuOn_ = true;

private:
    std::tuple<Key *, bool> findClickedKey(int x, int y);
    void paintBackground(cairo_t *cr);
};

} // namespace classicui
} // namespace fcitx

FCITX_DECLARE_LOG_CATEGORY(keyboard);

#define FCITX_KEYBOARD() FCITX_LOGC(::keyboard, Debug)

#endif // _FCITX_UI_CLASSIC_VIRTUALKEYBOARD_H_
