/*
 * SPDX-FileCopyrightText: 2017-2017 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "inputwindow.h"
#include <functional>
#include <initializer_list>
#include <limits>
#include <pango/pangocairo.h>
#include "fcitx-utils/color.h"
#include "fcitx-utils/log.h"
#include "fcitx/inputmethodentry.h"
#include "fcitx/inputpanel.h"
#include "fcitx/instance.h"
#include "fcitx/misc_p.h"
#include "classicui.h"

namespace fcitx::classicui {

auto newPangoLayout(PangoContext *context) {
    GObjectUniquePtr<PangoLayout> ptr(pango_layout_new(context));
    pango_layout_set_single_paragraph_mode(ptr.get(), false);
    return ptr;
}

static void prepareLayout(cairo_t *cr, PangoLayout *layout) {
    const PangoMatrix *matrix;

    matrix = pango_context_get_matrix(pango_layout_get_context(layout));

    if (matrix) {
        cairo_matrix_t cairo_matrix;

        cairo_matrix_init(&cairo_matrix, matrix->xx, matrix->yx, matrix->xy,
                          matrix->yy, matrix->x0, matrix->y0);

        cairo_transform(cr, &cairo_matrix);
    }
}

static void renderLayout(cairo_t *cr, PangoLayout *layout, int x, int y) {
    auto context = pango_layout_get_context(layout);
    auto *metrics = pango_context_get_metrics(
        context, pango_context_get_font_description(context),
        pango_context_get_language(context));
    auto ascent = pango_font_metrics_get_ascent(metrics);
    pango_font_metrics_unref(metrics);
    auto baseline = pango_layout_get_baseline(layout);
    auto yOffset = PANGO_PIXELS(ascent - baseline);
    cairo_save(cr);

    cairo_move_to(cr, x, y + yOffset);
    prepareLayout(cr, layout);
    pango_cairo_show_layout(cr, layout);

    cairo_restore(cr);
}

int MultilineLayout::width() const {
    int width = 0;
    for (const auto &layout : lines_) {
        int w, h;
        pango_layout_get_pixel_size(layout.get(), &w, &h);
        width = std::max(width, w);
    }
    return width;
}

void MultilineLayout::render(cairo_t *cr, int x, int y, int lineHeight,
                             bool highlight) {
    for (size_t i = 0; i < lines_.size(); i++) {
        if (highlight) {
            pango_layout_set_attributes(lines_[i].get(),
                                        highlightAttrLists_[i].get());
        } else {
            pango_layout_set_attributes(lines_[i].get(), attrLists_[i].get());
        }
        renderLayout(cr, lines_[i].get(), x, y);
        y += lineHeight;
    }
}

InputWindow::InputWindow(ClassicUI *parent) : parent_(parent) {
    fontMap_.reset(pango_cairo_font_map_new());
    // Although documentation says it is 96 by default, try not rely on this
    // behavior.
    fontMapDefaultDPI_ = pango_cairo_font_map_get_resolution(
        PANGO_CAIRO_FONT_MAP(fontMap_.get()));
    context_.reset(pango_font_map_create_context(fontMap_.get()));
    upperLayout_ = newPangoLayout(context_.get());
    lowerLayout_ = newPangoLayout(context_.get());

    keyboard_.reset(new VirtualKeyboard(
        parent->instance(),
        context_.get(),
        parent->config().useInputMethodLanguageToDisplayText.value()
    ));
}

void InputWindow::insertAttr(PangoAttrList *attrList, TextFormatFlags format,
                             int start, int end, bool highlight) const {
    if (format & TextFormatFlag::Underline) {
        auto *attr = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
        attr->start_index = start;
        attr->end_index = end;
        pango_attr_list_insert(attrList, attr);
    }
    if (format & TextFormatFlag::Italic) {
        auto *attr = pango_attr_style_new(PANGO_STYLE_ITALIC);
        attr->start_index = start;
        attr->end_index = end;
        pango_attr_list_insert(attrList, attr);
    }
    if (format & TextFormatFlag::Strike) {
        auto *attr = pango_attr_strikethrough_new(true);
        attr->start_index = start;
        attr->end_index = end;
        pango_attr_list_insert(attrList, attr);
    }
    if (format & TextFormatFlag::Bold) {
        auto *attr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
        attr->start_index = start;
        attr->end_index = end;
        pango_attr_list_insert(attrList, attr);
    }
    Color color =
        (format & TextFormatFlag::HighLight)
            ? *parent_->theme().inputPanel->highlightColor
            : (highlight ? *parent_->theme().inputPanel->highlightCandidateColor
                         : *parent_->theme().inputPanel->normalColor);
    const auto scale = std::numeric_limits<uint16_t>::max();
    auto *attr = pango_attr_foreground_new(
        color.redF() * scale, color.greenF() * scale, color.blueF() * scale);
    attr->start_index = start;
    attr->end_index = end;
    pango_attr_list_insert(attrList, attr);

    if (color.alpha() != 255) {
        auto *alphaAttr =
            pango_attr_foreground_alpha_new(color.alphaF() * scale);
        alphaAttr->start_index = start;
        alphaAttr->end_index = end;
        pango_attr_list_insert(attrList, alphaAttr);
    }

    auto background = *parent_->theme().inputPanel->highlightBackgroundColor;
    if (format.test(TextFormatFlag::HighLight) && background.alpha() > 0) {
        attr = pango_attr_background_new(background.redF() * scale,
                                         background.greenF() * scale,
                                         background.blueF() * scale);
        attr->start_index = start;
        attr->end_index = end;
        pango_attr_list_insert(attrList, attr);

        if (background.alpha() != 255) {
            auto *alphaAttr =
                pango_attr_background_alpha_new(background.alphaF() * scale);
            alphaAttr->start_index = start;
            alphaAttr->end_index = end;
            pango_attr_list_insert(attrList, alphaAttr);
        }
    }
}

void InputWindow::appendText(std::string &s, PangoAttrList *attrList,
                             PangoAttrList *highlightAttrList,
                             const Text &text) {
    for (size_t i = 0, e = text.size(); i < e; i++) {
        auto start = s.size();
        s.append(text.stringAt(i));
        auto end = s.size();
        if (start == end) {
            continue;
        }
        const auto format = text.formatAt(i);
        insertAttr(attrList, format, start, end, false);
        if (highlightAttrList) {
            insertAttr(highlightAttrList, format, start, end, true);
        }
    }
}

void InputWindow::resizeCandidates(size_t n) {
    while (labelLayouts_.size() < n) {
        labelLayouts_.emplace_back();
    }
    while (candidateLayouts_.size() < n) {
        candidateLayouts_.emplace_back();
    }

    nCandidates_ = n;
}

void InputWindow::setTextToMultilineLayout(InputContext *inputContext,
                                           MultilineLayout &layout,
                                           const Text &text) {
    auto lines = text.splitByLine();
    layout.lines_.clear();
    layout.attrLists_.clear();
    layout.highlightAttrLists_.clear();

    for (const auto &line : lines) {
        layout.lines_.emplace_back(pango_layout_new(context_.get()));
        layout.attrLists_.emplace_back();
        layout.highlightAttrLists_.emplace_back();
        setTextToLayout(inputContext, layout.lines_.back().get(),
                        &layout.attrLists_.back(),
                        &layout.highlightAttrLists_.back(), {line});
    }
}

void InputWindow::setTextToLayout(
    InputContext *inputContext, PangoLayout *layout,
    PangoAttrListUniquePtr *attrList, PangoAttrListUniquePtr *highlightAttrList,
    std::initializer_list<std::reference_wrapper<const Text>> texts) {
    auto *newAttrList = pango_attr_list_new();
    if (attrList) {
        // PangoAttrList does not have "clear()". So when we set new text,
        // we need to create a new one and get rid of old one.
        // We keep a ref to the attrList.
        attrList->reset(pango_attr_list_ref(newAttrList));
    }
    PangoAttrList *newHighlightAttrList = nullptr;
    if (highlightAttrList) {
        newHighlightAttrList = pango_attr_list_new();
        highlightAttrList->reset(newHighlightAttrList);
    }
    std::string line;
    for (const auto &text : texts) {
        appendText(line, newAttrList, newHighlightAttrList, text);
    }

    auto entry = parent_->instance()->inputMethodEntry(inputContext);
    if (*parent_->config().useInputMethodLanguageToDisplayText && entry &&
        !entry->languageCode().empty()) {
        if (auto language =
                pango_language_from_string(entry->languageCode().c_str())) {
            if (newAttrList) {
                auto attr = pango_attr_language_new(language);
                attr->start_index = 0;
                attr->end_index = line.size();
                pango_attr_list_insert(newAttrList, attr);
            }
            if (newHighlightAttrList) {
                auto attr = pango_attr_language_new(language);
                attr->start_index = 0;
                attr->end_index = line.size();
                pango_attr_list_insert(newHighlightAttrList, attr);
            }
        }
    }

    pango_layout_set_text(layout, line.c_str(), line.size());
    pango_layout_set_attributes(layout, newAttrList);
    pango_attr_list_unref(newAttrList);
}

void InputWindow::update(InputContext *inputContext) {
    if (parent_->suspended()) {
        visible_ = false;
        return;
    }
    // | aux up | preedit
    // | aux down
    // | 1 candidate | 2 ...
    // or
    // | aux up | preedit
    // | aux down
    // | candidate 1
    // | candidate 2
    // | candidate 3
    auto *instance = parent_->instance();
    auto &inputPanel = inputContext->inputPanel();
    inputContext_ = inputContext->watch();

    if (hasVirtualKeyboard()) {
        keyboard_->syncState();
        keyboard_->setInputContext(inputContext);
    }

    cursor_ = -1;
    auto preedit = instance->outputFilter(inputContext, inputPanel.preedit());
    auto auxUp = instance->outputFilter(inputContext, inputPanel.auxUp());
    pango_layout_set_single_paragraph_mode(upperLayout_.get(), true);
    setTextToLayout(inputContext, upperLayout_.get(), nullptr, nullptr,
                    {auxUp, preedit});
    if (preedit.cursor() >= 0 &&
        static_cast<size_t>(preedit.cursor()) <= preedit.textLength()) {
        cursor_ = preedit.cursor() + auxUp.toString().size();
    }

    auto auxDown = instance->outputFilter(inputContext, inputPanel.auxDown());
    setTextToLayout(inputContext, lowerLayout_.get(), nullptr, nullptr,
                    {auxDown});

    if (auto candidateList = inputPanel.candidateList()) {
        // Count non-placeholder candidates.
        int count = 0;

        for (int i = 0, e = candidateList->size(); i < e; i++) {
            const auto &candidate = candidateList->candidate(i);
            if (candidate.isPlaceHolder()) {
                continue;
            }
            count++;
        }
        resizeCandidates(count);

        candidateIndex_ = -1;
        int localIndex = 0;
        for (int i = 0, e = candidateList->size(); i < e; i++) {
            const auto &candidate = candidateList->candidate(i);
            // Skip placeholder.
            if (candidate.isPlaceHolder()) {
                continue;
            }

            if (i == candidateList->cursorIndex()) {
                candidateIndex_ = localIndex;
            }

            Text labelText = candidate.hasCustomLabel()
                                 ? candidate.customLabel()
                                 : candidateList->label(i);

            labelText = instance->outputFilter(inputContext, labelText);
            setTextToMultilineLayout(inputContext, labelLayouts_[localIndex],
                                     labelText);
            auto candidateText =
                instance->outputFilter(inputContext, candidate.text());
            setTextToMultilineLayout(
                inputContext, candidateLayouts_[localIndex], candidateText);
            localIndex++;
        }

        layoutHint_ = candidateList->layoutHint();
        if (auto *pageable = candidateList->toPageable()) {
            hasPrev_ = pageable->hasPrev();
            hasNext_ = pageable->hasNext();
        } else {
            hasPrev_ = false;
            hasNext_ = false;
        }
    } else {
        nCandidates_ = 0;
        candidateIndex_ = -1;
        hasPrev_ = false;
        hasNext_ = false;
    }

    visible_ = hasVirtualKeyboard() ||
               nCandidates_ ||
               pango_layout_get_character_count(upperLayout_.get()) ||
               pango_layout_get_character_count(lowerLayout_.get());
}

std::pair<unsigned int, unsigned int> InputWindow::sizeHint() {
    auto &theme = parent_->theme();
    auto *fontDesc =
        pango_font_description_from_string(parent_->config().font->c_str());
    pango_context_set_font_description(context_.get(), fontDesc);
    pango_font_description_free(fontDesc);
    pango_layout_context_changed(upperLayout_.get());
    pango_layout_context_changed(lowerLayout_.get());
    for (size_t i = 0; i < nCandidates_; i++) {
        labelLayouts_[i].contextChanged();
        candidateLayouts_[i].contextChanged();
    }
    auto *metrics = pango_context_get_metrics(
        context_.get(), pango_context_get_font_description(context_.get()),
        pango_context_get_language(context_.get()));
    auto fontHeight = pango_font_metrics_get_ascent(metrics) +
                      pango_font_metrics_get_descent(metrics);
    pango_font_metrics_unref(metrics);
    fontHeight = PANGO_PIXELS(fontHeight);

    size_t width = 0;
    size_t height = 0;
    auto updateIfLarger = [](size_t &m, size_t n) {
        if (n > m) {
            m = n;
        }
    };
    int w, h;

    const auto &textMargin = *theme.inputPanel->textMargin;
    auto extraW = *textMargin.marginLeft + *textMargin.marginRight;
    auto extraH = *textMargin.marginTop + *textMargin.marginBottom;
    if (pango_layout_get_character_count(upperLayout_.get())) {
        pango_layout_get_pixel_size(upperLayout_.get(), &w, &h);
        height += fontHeight + extraH;
        updateIfLarger(width, w + extraW);
    }
    if (pango_layout_get_character_count(lowerLayout_.get())) {
        pango_layout_get_pixel_size(lowerLayout_.get(), &w, &h);
        height += fontHeight + extraH;
        updateIfLarger(width, w + extraW);
    }

    bool vertical = parent_->config().verticalCandidateList.value();
    if (layoutHint_ == CandidateLayoutHint::Vertical) {
        vertical = true;
    } else if (layoutHint_ == CandidateLayoutHint::Horizontal) {
        vertical = false;
    }
    if (hasVirtualKeyboard())
        vertical = false;

    size_t wholeH = 0, wholeW = 0;
    for (size_t i = 0; i < nCandidates_; i++) {
        size_t candidateW = 0, candidateH = 0;
        if (labelLayouts_[i].characterCount()) {
            candidateW += labelLayouts_[i].width();
            updateIfLarger(candidateH,
                           std::max(1, labelLayouts_[i].size()) * fontHeight +
                               extraH);
        }
        if (candidateLayouts_[i].characterCount()) {
            candidateW += candidateLayouts_[i].width();
            updateIfLarger(
                candidateH,
                std::max(1, candidateLayouts_[i].size()) * fontHeight + extraH);
        }
        candidateW += extraW;

        if (vertical) {
            wholeH += candidateH;
            updateIfLarger(wholeW, candidateW);
        } else {
            wholeW += candidateW;
            updateIfLarger(wholeH, candidateH);
        }
    }
    updateIfLarger(width, wholeW);
    candidatesHeight_ = wholeH;
    height += wholeH;
    const auto &margin = *theme.inputPanel->contentMargin;
    width += *margin.marginLeft + *margin.marginRight;
    height += *margin.marginTop + *margin.marginBottom;

    if (nCandidates_ && (hasPrev_ || hasNext_)) {
        const auto &prev = theme.loadAction(*theme.inputPanel->prev);
        const auto &next = theme.loadAction(*theme.inputPanel->next);
        if (prev.valid() && next.valid()) {
            width += prev.width() + next.width();
        }
    }

    if (hasVirtualKeyboard()) {
        // TODO: Probably it's not correct yet
        auto pair = keyboard_->size();
        size_t borderWidth = 2;
        size_t keyboardWidth = pair.first + keyboard_->marginX() * 2;
        size_t keyboardHeight = pair.second + keyboard_->marginY() * 2;
        width = std::max(width, keyboardWidth + borderWidth * 2);
        height += keyboardHeight;
    }

    return {width, height};
}

void InputWindow::paint(cairo_t *cr, unsigned int width, unsigned int height) {
    auto &theme = parent_->theme();
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    theme.paint(cr, *theme.inputPanel->background, width, height);
    const auto &margin = *theme.inputPanel->contentMargin;
    const auto &textMargin = *theme.inputPanel->textMargin;
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_save(cr);

    prevRegion_ = Rect();
    nextRegion_ = Rect();
    if (nCandidates_ && (hasPrev_ || hasNext_)) {
        const auto &prev = theme.loadAction(*theme.inputPanel->prev);
        const auto &next = theme.loadAction(*theme.inputPanel->next);
        if (prev.valid() && next.valid()) {
            cairo_save(cr);
            nextRegion_.setPosition(width - *margin.marginRight - next.width(),
                                    height - *margin.marginBottom -
                                        next.height());
            nextRegion_.setSize(next.width(), next.height());
            cairo_translate(cr, nextRegion_.left(), nextRegion_.top());
            shrink(nextRegion_, *theme.inputPanel->next->clickMargin);
            double alpha = 1.0;
            if (!hasNext_) {
                alpha = 0.3;
            } else if (nextHovered_) {
                alpha = 0.7;
            }
            theme.paint(cr, *theme.inputPanel->next, alpha);
            cairo_restore(cr);
            cairo_save(cr);
            prevRegion_.setPosition(
                width - *margin.marginRight - next.width() - prev.width(),
                height - *margin.marginBottom - prev.height());
            prevRegion_.setSize(prev.width(), prev.height());
            cairo_translate(cr, prevRegion_.left(), prevRegion_.top());
            shrink(prevRegion_, *theme.inputPanel->prev->clickMargin);
            alpha = 1.0;
            if (!hasPrev_) {
                alpha = 0.3;
            } else if (prevHovered_) {
                alpha = 0.7;
            }
            theme.paint(cr, *theme.inputPanel->prev, alpha);
            cairo_restore(cr);
        }
    }

    // Move position to the right place.
    cairo_translate(cr, *margin.marginLeft, *margin.marginTop);

    cairo_save(cr);
    cairoSetSourceColor(cr, *theme.inputPanel->normalColor);
    // CLASSICUI_DEBUG() << theme.inputPanel->normalColor->toString();
    auto *metrics = pango_context_get_metrics(
        context_.get(), pango_context_get_font_description(context_.get()),
        pango_context_get_language(context_.get()));
    auto fontHeight = pango_font_metrics_get_ascent(metrics) +
                      pango_font_metrics_get_descent(metrics);
    pango_font_metrics_unref(metrics);
    fontHeight = PANGO_PIXELS(fontHeight);

    size_t currentHeight = 0;
    int w, h;
    auto extraW = *textMargin.marginLeft + *textMargin.marginRight;
    auto extraH = *textMargin.marginTop + *textMargin.marginBottom;
    if (pango_layout_get_character_count(upperLayout_.get())) {
        renderLayout(cr, upperLayout_.get(), *textMargin.marginLeft,
                     *textMargin.marginTop);
        pango_layout_get_pixel_size(upperLayout_.get(), &w, &h);
        PangoRectangle pos;
        if (cursor_ >= 0) {
            pango_layout_get_cursor_pos(upperLayout_.get(), cursor_, &pos,
                                        nullptr);

            cairo_save(cr);
            cairo_set_line_width(cr, 2);
            auto offsetX = pango_units_to_double(pos.x);
            cairo_move_to(cr, *textMargin.marginLeft + offsetX + 1,
                          *textMargin.marginTop);
            cairo_line_to(cr, *textMargin.marginLeft + offsetX + 1,
                          *textMargin.marginTop + fontHeight);
            cairo_stroke(cr);
            cairo_restore(cr);
        }
        currentHeight += fontHeight + extraH;
    }
    if (pango_layout_get_character_count(lowerLayout_.get())) {
        renderLayout(cr, lowerLayout_.get(), *textMargin.marginLeft,
                     *textMargin.marginTop + currentHeight);
        pango_layout_get_pixel_size(lowerLayout_.get(), &w, nullptr);
        currentHeight += fontHeight + extraH;
    }

    bool vertical = parent_->config().verticalCandidateList.value();
    if (layoutHint_ == CandidateLayoutHint::Vertical) {
        vertical = true;
    } else if (layoutHint_ == CandidateLayoutHint::Horizontal) {
        vertical = false;
    }
    if (hasVirtualKeyboard())
        vertical = false;

    candidateRegions_.clear();
    candidateRegions_.reserve(nCandidates_);
    size_t wholeW = 0, wholeH = 0;

    // size of text = textMargin + actual text size.
    // HighLight = HighLight margin + TEXT.
    // Click region = HighLight - click

    for (size_t i = 0; i < nCandidates_; i++) {
        int x, y;
        if (vertical) {
            x = 0;
            y = currentHeight + wholeH;
        } else {
            x = wholeW;
            y = currentHeight;
        }
        x += *textMargin.marginLeft;
        y += *textMargin.marginTop;
        int labelW = 0, labelH = 0, candidateW = 0, candidateH = 0;
        if (labelLayouts_[i].characterCount()) {
            labelW = labelLayouts_[i].width();
            labelH = fontHeight * labelLayouts_[i].size();
        }
        if (candidateLayouts_[i].characterCount()) {
            candidateW = candidateLayouts_[i].width();
            candidateH = fontHeight * candidateLayouts_[i].size();
        }
        int vheight = std::max({fontHeight, labelH, candidateH});
        if (vertical) {
            wholeH += vheight + extraH;
        } else {
            wholeW += candidateW + labelW + extraW;
            wholeH = std::max(wholeH, static_cast<size_t>(vheight + extraH));
        }
        const auto &highlightMargin = *theme.inputPanel->highlight->margin;
        const auto &clickMargin = *theme.inputPanel->highlight->clickMargin;
        auto highlightWidth = labelW + candidateW;
        if (*theme.inputPanel->fullWidthHighlight && vertical) {
            // Last candidate, fill.
            highlightWidth = width - *margin.marginLeft - *margin.marginRight -
                             *textMargin.marginRight - *textMargin.marginLeft;
        }
        const int highlightIndex = highlight();
        bool highlight = false;
        if (highlightIndex >= 0 && i == static_cast<size_t>(highlightIndex)) {
            cairo_save(cr);
            cairo_translate(cr, x - *highlightMargin.marginLeft,
                            y - *highlightMargin.marginTop);
            theme.paint(cr, *theme.inputPanel->highlight,
                        highlightWidth + *highlightMargin.marginLeft +
                            *highlightMargin.marginRight,
                        vheight + *highlightMargin.marginTop +
                            *highlightMargin.marginBottom);
            cairo_restore(cr);
            highlight = true;
        }
        Rect candidateRegion;
        candidateRegion
            .setPosition(*margin.marginLeft + x - *highlightMargin.marginLeft +
                             *clickMargin.marginLeft,
                         *margin.marginTop + y - *highlightMargin.marginTop +
                             *clickMargin.marginTop)
            .setSize(highlightWidth + *highlightMargin.marginLeft +
                         *highlightMargin.marginRight -
                         *clickMargin.marginLeft - *clickMargin.marginRight,
                     vheight + *highlightMargin.marginTop +
                         *highlightMargin.marginBottom -
                         *clickMargin.marginTop - *clickMargin.marginBottom);
        candidateRegions_.push_back(candidateRegion);
        if (labelLayouts_[i].characterCount()) {
            labelLayouts_[i].render(cr, x, y, fontHeight, highlight);
        }
        if (candidateLayouts_[i].characterCount()) {
            candidateLayouts_[i].render(cr, x + labelW, y, fontHeight,
                                        highlight);
        }
    }

    if (hasVirtualKeyboard()) {
        wholeH += currentHeight + keyboard_->marginY();
        keyboard_->paint(cr, keyboard_->marginX(), wholeH);
    }

    cairo_restore(cr);
}

void InputWindow::click(int x, int y, bool isRelease) {
    auto *inputContext = inputContext_.get();
    if (!inputContext) {
        return;
    }

    if (hasVirtualKeyboard())
        keyboard_->click(inputContext, x, y, isRelease);

    const auto candidateList = inputContext->inputPanel().candidateList();
    if (!candidateList) {
        return;
    }
    for (size_t idx = 0, e = candidateRegions_.size(); idx < e; idx++) {
        if (candidateRegions_[idx].contains(x, y)) {
            const auto *candidate =
                nthCandidateIgnorePlaceholder(*candidateList, idx);
            if (candidate) {
                candidate->select(inputContext);
            }
            break;
        }
    }
    if (auto *pageable = candidateList->toPageable()) {
        if (pageable->hasPrev() && prevRegion_.contains(x, y)) {
            pageable->prev();
            inputContext->updateUserInterface(
                UserInterfaceComponent::InputPanel);
            return;
        }
        if (pageable->hasNext() && nextRegion_.contains(x, y)) {
            pageable->next();
            inputContext->updateUserInterface(
                UserInterfaceComponent::InputPanel);
        }
    }
}

void InputWindow::wheel(bool up) {
    if (!*parent_->config().useWheelForPaging) {
        return;
    }
    auto *inputContext = inputContext_.get();
    if (!inputContext) {
        return;
    }
    const auto candidateList = inputContext->inputPanel().candidateList();
    if (!candidateList) {
        return;
    }
    if (auto *pageable = candidateList->toPageable()) {
        if (up) {
            if (pageable->hasPrev()) {
                pageable->prev();
                inputContext->updateUserInterface(
                    UserInterfaceComponent::InputPanel);
            }
        } else {
            if (pageable->hasNext()) {
                pageable->next();
                inputContext->updateUserInterface(
                    UserInterfaceComponent::InputPanel);
            }
        }
    }
}

int InputWindow::highlight() const {
    int highlightIndex = (hoverIndex_ >= 0) ? hoverIndex_ : candidateIndex_;
    return highlightIndex;
}

bool InputWindow::hover(int x, int y) {
    bool needRepaint = false;
    auto oldHighlight = highlight();
    hoverIndex_ = -1;
    for (int idx = 0, e = candidateRegions_.size(); idx < e; idx++) {
        if (candidateRegions_[idx].contains(x, y)) {
            hoverIndex_ = idx;
            break;
        }
    }

    needRepaint = needRepaint || oldHighlight != highlight();

    auto prevHovered = prevRegion_.contains(x, y);
    auto nextHovered = nextRegion_.contains(x, y);
    needRepaint = needRepaint || prevHovered_ != prevHovered;
    needRepaint = needRepaint || nextHovered_ != nextHovered;
    prevHovered_ = prevHovered;
    nextHovered_ = nextHovered;
    return needRepaint;
}

} // namespace fcitx::classicui
