/*
 * SPDX-FileCopyrightText: 2017-2017 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_UI_CLASSIC_INPUTWINDOW_H_
#define _FCITX_UI_CLASSIC_INPUTWINDOW_H_

#include <utility>
#include <cairo/cairo.h>
#include <pango/pango.h>
#include "fcitx/candidatelist.h"
#include "fcitx/inputcontext.h"
#include "common.h"
#include "fcitx-utils/log.h"

namespace fcitx {
namespace classicui {

class ClassicUI;

using PangoAttrListUniquePtr = UniqueCPtr<PangoAttrList, pango_attr_list_unref>;

class MultilineLayout {
public:
    MultilineLayout() = default;
    FCITX_INLINE_DEFINE_DEFAULT_DTOR_AND_MOVE(MultilineLayout);

    void contextChanged() {
        for (const auto &layout : lines_) {
            pango_layout_context_changed(layout.get());
        }
    }
    int characterCount() const {
        int count = 0;
        for (const auto &layout : lines_) {
            count += pango_layout_get_character_count(layout.get());
        }
        return count;
    }

    int width() const;

    int size() { return lines_.size(); }
    void render(cairo_t *cr, int x, int y, int lineHeight, bool highlight);

    std::vector<GObjectUniquePtr<PangoLayout>> lines_;
    std::vector<PangoAttrListUniquePtr> attrLists_;
    std::vector<PangoAttrListUniquePtr> highlightAttrLists_;
};

class Keyboard;
class Key {
public:
    Key(std::string keyName, std::string label, std::string upperKeyName = "",
        std::string upperLabel = "");

    const char* keyName(bool useUpper) const {
        if (!useUpper || upperKeyName_.empty()) {
            return keyName_.c_str();
        }
        return upperKeyName_.c_str();
    }

    const char* label(bool useUpper) const {
        if (!useUpper || upperLabel_.empty()) {
            return label_.c_str();
        }
        return upperLabel_.c_str();
    }

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

    fcitx::Key convert(bool useUpper) const;
    virtual void click(Keyboard *keyboard, InputContext *inputContext) const;

    double width_ = 60;
    double height_ = 50;
    double leftMargin_ = 5;
    double topMargin_ = 5;
    bool newLine_ = false;
    bool visible_ = true;

private:
    /*
     * Be used in converting to Fcitx::Key.
     * Corresponding to keyNameList in keynametable.h.
     */
    const std::string keyName_;

    /*
     * Text for display.
     */
    const std::string label_;

    const std::string upperKeyName_;
    const std::string upperLabel_;

    Rect region_;
};

class DummyKey : public Key {
public:
    DummyKey() : Key("", "") {
        visible_ = false;
    }
};

class UpperToggleKey : public Key {
public:
    UpperToggleKey(std::string labelInLower, std::string labelInUpper)
                   : Key("", labelInLower, "", labelInUpper) {}
    void click(Keyboard *keyboard, InputContext *inputContext) const override;
};

class NormalSwitchKey : public Key {
public:
    NormalSwitchKey(std::string label) : Key("", label) {}
    void click(Keyboard *keyboard, InputContext *inputContext) const override;
};

class NumberSwitchKey : public Key {
public:
    NumberSwitchKey(std::string label) : Key("", label) {}
    void click(Keyboard *keyboard, InputContext *inputContext) const override;
};

class MarkSwitchKey : public Key {
public:
    MarkSwitchKey(std::string label) : Key("", label) {}
    void click(Keyboard *keyboard, InputContext *inputContext) const override;
};

class Keyboard {
public:
    Keyboard();
    void paint(cairo_t *cr, unsigned int offsetX, unsigned int offsetY);
    void click(InputContext *inputContext, int x, int y);
    void setNormalKeys();
    void setNumberKeys();
    void setMarkKeys();
    std::pair<unsigned int, unsigned int> size();
    unsigned int marginX() { return 15; }
    unsigned int marginY() { return 5; }

    std::vector<std::unique_ptr<Key>> keys_;
    bool useUpper_ = false;

private:
    void paintOneKey(cairo_t *cr, Key *key);
};

class InputWindow {
public:
    InputWindow(ClassicUI *parent);
    void update(InputContext *inputContext);
    std::pair<unsigned int, unsigned int> sizeHint(bool withKeyboard = true);
    void paint(cairo_t *cr, unsigned int width, unsigned int height);
    void hide();
    bool visible() const { return visible_; }
    bool hover(int x, int y);
    void click(int x, int y);
    void wheel(bool up);

protected:
    void resizeCandidates(size_t n);
    void appendText(std::string &s, PangoAttrList *attrList,
                    PangoAttrList *highlightAttrList, const Text &text);
    void insertAttr(PangoAttrList *attrList, TextFormatFlags format, int start,
                    int end, bool highlight) const;
    void setTextToLayout(
        InputContext *inputContext, PangoLayout *layout,
        PangoAttrListUniquePtr *attrList,
        PangoAttrListUniquePtr *highlightAttrList,
        std::initializer_list<std::reference_wrapper<const Text>> texts);
    void setTextToMultilineLayout(InputContext *inputContext,
                                  MultilineLayout &layout, const Text &text);
    int highlight() const;

    ClassicUI *parent_;
    GObjectUniquePtr<PangoFontMap> fontMap_;
    double fontMapDefaultDPI_ = 96.0;
    GObjectUniquePtr<PangoContext> context_;
    GObjectUniquePtr<PangoLayout> upperLayout_;
    GObjectUniquePtr<PangoLayout> lowerLayout_;
    std::vector<MultilineLayout> labelLayouts_;
    std::vector<MultilineLayout> candidateLayouts_;
    std::vector<Rect> candidateRegions_;
    Keyboard keyboard_;
    TrackableObjectReference<InputContext> inputContext_;
    bool visible_ = false;
    int cursor_ = 0;
    size_t nCandidates_ = 0;
    bool hasPrev_ = false;
    bool hasNext_ = false;
    Rect prevRegion_;
    Rect nextRegion_;
    bool prevHovered_ = false;
    bool nextHovered_ = false;
    int candidateIndex_ = -1;
    CandidateLayoutHint layoutHint_ = CandidateLayoutHint::NotSet;
    size_t candidatesHeight_ = 0;
    int hoverIndex_ = -1;
    bool hasVirtualKeyboard_ = true;
};
} // namespace classicui
} // namespace fcitx

FCITX_DECLARE_LOG_CATEGORY(keyboard);

#define FCITX_KEYBOARD() FCITX_LOGC(::keyboard, Debug)

#endif // _FCITX_UI_CLASSIC_INPUTWINDOW_H_
