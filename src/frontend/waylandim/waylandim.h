/*
 * SPDX-FileCopyrightText: 2016-2016 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_FRONTEND_WAYLANDIM_WAYLANDIM_H_
#define _FCITX_FRONTEND_WAYLANDIM_WAYLANDIM_H_

#include <fcitx/addonfactory.h>
#include <fcitx/addoninstance.h>
#include <fcitx/addonmanager.h>
#include <fcitx/instance.h>
#include "wayland_public.h"
#include "waylandim_public.h"

namespace fcitx {

class WaylandIMModule;
class WaylandIMServer;
#if 0
class WaylandIMServerV2;
#endif

class WaylandIMModule : public AddonInstance {
public:
    WaylandIMModule(Instance *instance);
    ~WaylandIMModule();

    FCITX_ADDON_DEPENDENCY_LOADER(wayland, instance_->addonManager());
    Instance *instance() { return instance_; }

#if 0
    wayland::ZwpInputMethodV2 *getInputMethodV2(InputContext *ic);

    FCITX_ADDON_EXPORT_FUNCTION(WaylandIMModule, getInputMethodV2);
#endif

private:
    Instance *instance_;
    std::unordered_map<std::string, std::unique_ptr<WaylandIMServer>> servers_;
#if 0
    std::unordered_map<std::string, std::unique_ptr<WaylandIMServerV2>>
        serversV2_;
#endif
    std::unique_ptr<HandlerTableEntry<WaylandConnectionCreated>>
        createdCallback_;
    std::unique_ptr<HandlerTableEntry<WaylandConnectionClosed>> closedCallback_;
};
} // namespace fcitx

FCITX_DECLARE_LOG_CATEGORY(waylandim);

#define WAYLANDIM_DEBUG() FCITX_LOGC(::waylandim, Debug)

#endif // _FCITX_FRONTEND_WAYLANDIM_WAYLANDIM_H_
