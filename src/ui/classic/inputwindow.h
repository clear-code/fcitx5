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
    virtual const char* label(Keyboard *keyboard) const = 0;
    virtual void click(Keyboard *keyboard, InputContext *inputContext) const = 0;

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
    void click(Keyboard *, InputContext *) const override {}
};

/*
 * Key that is visible, but do not work.
 */
class EmptyKey : public Key {
public:
    const char* label(Keyboard *) const override { return ""; }
    void click(Keyboard *, InputContext *) const override {}
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
    void click(Keyboard *keyboard, InputContext *inputContext) const override;

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
    void click(Keyboard *keyboard, InputContext *inputContext) const override;

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
    void click(Keyboard *keyboard, InputContext *inputContext) const override;

private:
    const std::string label_;
};

class ZenkakuHankakuKey : public Key {
public:
    const char* label(Keyboard *keyboard) const override;
    void click(Keyboard *keyboard, InputContext *inputContext) const override;
};

class UpperToggleKey : public Key {
public:
    const char* label(Keyboard *keyboard) const override;
    void click(Keyboard *keyboard, InputContext *inputContext) const override;
};

class ModeSwitchKey : public Key {
public:
    const char* label(Keyboard *keyboard) const override;
    void click(Keyboard *keyboard, InputContext *inputContext) const override;
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
    void click(InputContext *inputContext, int x, int y);
    void setTextKeys(bool isZenkakuMode);
    void setMarkKeys();
    std::pair<unsigned int, unsigned int> size();
    unsigned int marginX() { return 15; }
    unsigned int marginY() { return 5; }

    std::vector<std::unique_ptr<Key>> keys_;
    KeyboardMode mode_ = KeyboardMode::ZenkakuText;
    bool useUpperHankakuText_ = false;
    bool useZenkakuMark_ = false;

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
