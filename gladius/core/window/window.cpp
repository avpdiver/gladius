//
// Created by alexa on 026 26 08 2016.
//

#include <algorithm>
#include <cstring>
#include "window.h"

namespace gladius {
namespace core {

size_t g_listener_id = 0;

c_window::c_window() {
}

c_window::~c_window() {
    close();
}

size_t c_window::add_event_listener(e_window_event event, window_event_listener_t listener) {
    m_listeners[event].emplace_back(listener, ++g_listener_id);
    return g_listener_id;
}

void c_window::remove_event_listener(size_t listener) {
    for (auto &p : m_listeners) {
        auto e = std::remove_if(p.second.begin(), p.second.end(),
                                [listener](std::pair<window_event_listener_t, size_t> &l)->bool {
                                    return l.second == listener;
                                });
        p.second.erase(e, p.second.end());
    }
}

void c_window::process_event(e_window_event event_type, void *event) const {
    const auto listeners = m_listeners.find(event_type);
    if (listeners == m_listeners.end()) {
        return;
    }

    for (auto &l : listeners->second) {
        l.first(event);
    }
}

}
}