//
// Created by pav on 28.09.15.
//

#ifndef GLADIUS_WINDOW_H
#define GLADIUS_WINDOW_H

#include <stddef.h>
#include <unordered_map>
#include <vector>
#include <functional>

#include "glm/vec2.hpp"

#ifdef PLATFORM_LINUX
#include <xcb/xcb.h>
#endif

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace gladius {
namespace core {

enum e_window_event {
    ON_SHOW,
    ON_HIDE,
    ON_CLOSE,
    ON_RESIZE
};

struct s_window_info {
    glm::uvec2 extent;

#ifdef PLATFORM_WINDOWS
	HINSTANCE           instance;
	HWND                handle;
	s_window_info() :
		instance(),
		handle() {
	}
#endif

#ifdef PLATFORM_LINUX
    xcb_connection_t *connection;
    xcb_window_t handle;
    xcb_intern_atom_reply_t *delete_reply;
    s_window_info()
        : connection(), handle(), delete_reply() {
    }
#endif
};

struct s_window_desc {
public:
    s_window_desc& set_caption(const char* caption_) {
        caption = caption_;
        return *this;
    }

    s_window_desc& set_width(uint32_t width_) {
        width = width_;
        return *this;
    }

    s_window_desc& set_height(uint32_t height_) {
        height = height_;
        return *this;
    }

    s_window_desc& is_fullscreen(bool fullscreen_) {
        fullscreen = fullscreen_;
        return *this;
    }

public:
    uint32_t width = 640;
    uint32_t height = 480;
    bool fullscreen = false;
	std::string caption = "gladius";
};

class c_window;

typedef std::function<void(void *)> window_event_listener_t;

class c_window {
public:
    c_window();

    ~c_window();

public:
    bool create(const s_window_desc& desc);

    void close();

public:
    const s_window_info *const get_window_info() const {
        return &m_window_info;
    }

    void set_size(int width, int height);

    const glm::uvec2 get_size() const {
        return m_window_info.extent;
    }

public:
    bool is_closed();

public:
    void process_events();
    void process_event(e_window_event event_type, void *event) const;

public:
    size_t add_event_listener(e_window_event event, window_event_listener_t listener);

    void remove_event_listener(size_t listener);

private:
    std::unordered_map<e_window_event, std::vector<std::pair<window_event_listener_t, size_t>>, std::hash<int>>
        m_listeners;

private:
    s_window_info m_window_info;
};

}
}

#endif //GLADIUS_WINDOW_H
