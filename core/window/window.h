//
// Created by pav on 28.09.15.
//

#ifndef DAR_WINDOW_H
#define DAR_WINDOW_H

#include <stddef.h>
#include <unordered_map>
#include <vector>
#include "../platform.h"

#ifdef UNIX
    #include <X11/Xlib.h>
#elif defined(WINDOWS)
    #include <windows.h>
#endif

#include <functional>

namespace gladius
{
    namespace core
    {
        enum e_window_event
        {
            ON_SHOW,
            ON_HIDE,
            ON_CLOSE,
            ON_RESIZE,
            ON_INPUT
        };

        typedef std::function<void (void*)> window_event_listener_t;

        class c_window
        {
        public:
            c_window ();

            ~c_window ();

        public:
            bool create ();

            void close ();

            void show ();

            void hide ();

            void process_events();

            void process_event (e_window_event event_type, void *event) const;

        public:
            bool is_closed();

        public:
            void add_event_listener (e_window_event event, window_event_listener_t* listener);

            void remove_event_listener (window_event_listener_t* listener);

        private:
            std::unordered_map<e_window_event, std::vector<window_event_listener_t*>, std::hash<int>> m_listeners;

#ifdef NIX

        public:
            Display* get_display();
            Window& get_window();

        private:
            Display *m_display;
            Window m_window;

#elif defined(WINDOWS)
        public:
            HWND getHWND();

        private:
            HWND m_hwnd;
#endif

        };
    }
}

#endif //DAR_WINDOW_H
