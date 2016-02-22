//
// Created by pav on 28.09.15.
//

#ifndef GLADIUS_WINDOW_H
#define GLADIUS_WINDOW_H

#include <stddef.h>
#include <unordered_map>
#include <vector>
#include <functional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include "glm/vec2.hpp"

namespace gladius
{
    namespace core
    {
        enum e_window_event
        {
            ON_SHOW,
            ON_HIDE,
            ON_CLOSE,
            ON_RESIZE
        };

        typedef std::function<void(void*)> window_event_listener_t;

        class c_window
        {
        public:
            c_window();

            ~c_window();

        public:
            bool create();

            void close();

        public:
            const SDL_SysWMinfo* const get_system_info() const
            {
                return &m_sys_wm_info;
            }

            const glm::ivec2 get_size() const
            {
                glm::ivec2 size;
                SDL_GetWindowSize(m_window, &size.x, &size.y);
                return size;
            }

        public:
            void process_event(e_window_event event_type, void* event) const;

        public:
            bool is_closed();

        public:
            void add_event_listener(e_window_event event, window_event_listener_t* listener);

            void remove_event_listener(window_event_listener_t* listener);

        private:
            std::unordered_map<e_window_event, std::vector<window_event_listener_t*>, std::hash<int>> m_listeners;

        private:
            SDL_Window* m_window;
            SDL_SysWMinfo m_sys_wm_info;
        };
    }
}

#endif //GLADIUS_WINDOW_H
