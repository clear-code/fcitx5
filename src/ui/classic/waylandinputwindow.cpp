/*
 * SPDX-FileCopyrightText: 2017-2017 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "waylandinputwindow.h"
#include "waylandim_public.h"
#include "waylandui.h"
#include "waylandwindow.h"
#include "zwp_input_panel_v1.h"
#include "zwp_input_popup_surface_v2.h"

#ifdef __linux__
#include <linux/input-event-codes.h>
#elif __FreeBSD__
#include <dev/evdev/input-event-codes.h>
#else
#define BTN_LEFT 0x110
#endif

namespace fcitx::classicui {

WaylandInputWindow::WaylandInputWindow(WaylandUI *ui)
    : InputWindow(ui->parent()), ui_(ui), window_(ui->newWindow()) {
    window_->createWindow();
    window_->repaint().connect([this]() {
        if (auto *ic = repaintIC_.get()) {
            if (ic->hasFocus()) {
                update(ic);
            }
        }
    });
    window_->click().connect([this](int x, int y, uint32_t button,
                                    uint32_t state) {
        if (button == BTN_LEFT) {
            click(x, y, state == WL_POINTER_BUTTON_STATE_RELEASED);
            repaint();
        }
    });
    window_->hover().connect([this](int x, int y) {
        if (hover(x, y)) {
            repaint();
        }
    });
    window_->leave().connect([this]() {
        if (hover(-1, -1)) {
            repaint();
        }
    });
    window_->axis().connect([this](int, int, uint32_t axis, wl_fixed_t value) {
        if (axis != WL_POINTER_AXIS_VERTICAL_SCROLL) {
            return;
        }
        scroll_ += value;
        bool triggered = false;
        while (scroll_ >= 2560) {
            scroll_ -= 2560;
            wheel(/*up=*/false);
            triggered = true;
        }
        while (scroll_ <= -2560) {
            scroll_ += 2560;
            wheel(/*up=*/true);
            triggered = true;
        }
        if (triggered) {
            repaint();
        }
    });
    initPanel();
}

void WaylandInputWindow::initPanel() {
    if (panelSurface_) {
        return;
    }
    if (!window_->surface()) {
        window_->createWindow();
        return;
    }
    auto panel = ui_->display()->getGlobal<wayland::ZwpInputPanelV1>();
    if (!panel) {
        return;
    }
    panelSurface_.reset(panel->getInputPanelSurface(window_->surface()));
    if (hasVirtualKeyboard())
        panelSurface_->setToplevel(ui_->display()->output(), 0);
    else
        panelSurface_->setOverlayPanel();
}

void WaylandInputWindow::resetPanel() { panelSurface_.reset(); }

void WaylandInputWindow::update(fcitx::InputContext *ic) {
    InputWindow::update(ic);
    if (ic->frontend() == std::string_view("wayland_v2")) {
        if (ic != v2IC_.get()) {
            v2IC_ = ic->watch();
            auto *im = ui_->parent()
                           ->waylandim()
                           ->call<IWaylandIMModule::getInputMethodV2>(ic);
            panelSurfaceV2_.reset(im->getInputPopupSurface(window_->surface()));
        }
    }
    if (!panelSurface_ && !panelSurfaceV2_) {
        return;
    }

    if (!visible()) {
        window_->hide();
        return;
    }
    auto pair = sizeHint();
    int width = pair.first, height = pair.second;

    if (width != window_->width() || height != window_->height()) {
        window_->resize(width, height);
    }

    if (auto *surface = window_->prerender()) {
        cairo_t *c = cairo_create(surface);
        paint(c, width, height);
        cairo_destroy(c);
        window_->render();
    } else {
        repaintIC_ = ic->watch();
    }
}

void WaylandInputWindow::repaint() {

    if (auto *surface = window_->prerender()) {
        cairo_t *c = cairo_create(surface);
        paint(c, window_->width(), window_->height());
        cairo_destroy(c);
        window_->render();
    }
}

} // namespace fcitx::classicui
