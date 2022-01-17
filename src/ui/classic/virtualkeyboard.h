/*
 * SPDX-FileCopyrightText: 2021-2021 daipom <fukuda@clear-code.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_UI_CLASSIC_VIRTUALKEYBOARD_H_
#define _FCITX_UI_CLASSIC_VIRTUALKEYBOARD_H_

#include "common.h"
#include "virtualkeyboardi18n.h"
#include <cairo/cairo.h>
#include <pango/pango.h>
#include "fcitx/instance.h"
#include "fcitx/inputcontext.h"
#include "fcitx/inputmethodmanager.h"
#include "fcitx/inputpanel.h"
#include <fcitx-utils/event.h>
#include "fcitx-utils/log.h"

namespace fcitx {
namespace classicui {

enum KeyLabelAlignVertical {
    Top,
    Center,
    Bottom,
};

class VirtualKeyboard;
class VirtualKey {
public:
    virtual const char* label(VirtualKeyboard *keyboard) const = 0;
    virtual void click(VirtualKeyboard *keyboard, InputContext *inputContext, bool isRelease) = 0;
    virtual void paintLabel(VirtualKeyboard *keyboard, cairo_t *cr);
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
        if (labelAlignVertical_ == KeyLabelAlignVertical::Top) {
            const auto topMargin = 10;
            return - extents.y_bearing + topMargin;
        }
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

class VirtualKeyboard {
public:
    VirtualKeyboard(Instance *instance);
    void paint(cairo_t *cr, unsigned int offsetX, unsigned int offsetY);
    bool click(InputContext *inputContext, int x, int y, bool isRelease);
    bool syncState();
    void switchLanguage();
    void setCurrentInputMethod(std::string name);
    void enumerateGroup();

    /// Some IMs needs to handle the single Shift key event,
    /// not only the states of the main key.
    /// Ex. In IMs using customXkbState such as keyboard-ru, some states of modifiers are not
    /// propagated.
    /// So it is not enough to process states of the main key in `WaylandIMInputContextV1`
    /// in these IMs.
    /// (See event implementation of
    /// `EventWatcherPhase::ReservedFirst` of `EventType::InputContextKeyEvent` in `Instance`.)
    void sendShiftModifierToIM(InputContext *inputContext, bool isRelease);

    bool isPreediting();
    bool isSeletingCandidates();
    void updateInputPanel();

    std::vector<std::unique_ptr<VirtualKey>> &keys() { return i18nKeyboard_->keys(); }
    I18nKeyboard *i18nKeyboard() { return i18nKeyboard_.get(); }
    template<class T>
    T *i18nKeyboard() {
        static_assert(std::is_base_of<I18nKeyboard, T>::value,
            "type parameter of this function must derive from I18nKeyboard");
        return static_cast<T *>(i18nKeyboard_.get());
    }

    std::pair<unsigned int, unsigned int> size();
    unsigned int marginX() { return 15; }
    unsigned int marginY() { return 6; }
    VirtualKey *pushingKey() { return pushingKey_; }

protected:
    Instance *instance_;
    VirtualKey *pushingKey_ = nullptr;
    TrackableObjectReference<InputContext> lastInputContext_;

public: // TODO: Should be moved to protected
    bool isShiftOn_ = false;

private:
    std::tuple<VirtualKey *, bool> findClickedKey(int x, int y);
    void paintBackground(cairo_t *cr);
    void setI18nKeyboard(I18nKeyboard *i18nKeyboard);

    std::unique_ptr<I18nKeyboard> i18nKeyboard_;
    I18nKeyboardSelector i18nKeyboardSelector_;
};

} // namespace classicui
} // namespace fcitx

FCITX_DECLARE_LOG_CATEGORY(keyboard);

#define FCITX_KEYBOARD() FCITX_LOGC(::keyboard, Debug)

#endif // _FCITX_UI_CLASSIC_VIRTUALKEYBOARD_H_
