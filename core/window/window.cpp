//
// Created by pav on 28.09.15.
//


#include <algorithm>
#include "window.h"

namespace gladius
{
    namespace core
    {

        c_window::~c_window ()
        {
            close ();
        }

        void c_window::add_event_listener(e_window_event event, window_event_listener_t* listener)
        {
            m_listeners[event].push_back(listener);
        }

        void c_window::remove_event_listener(window_event_listener_t* listener)
        {
            for (auto& p : m_listeners) {
                auto e = std::remove_if(p.second.begin(), p.second.end(), [listener](window_event_listener_t* l) -> bool { return l == listener; });
                p.second.erase(e, p.second.end());
            }
        }

        void c_window::process_event(e_window_event event_type, void* event) const
        {
            const auto listeners = m_listeners.find(event_type);
            if (listeners == m_listeners.end())
            {
                return;
            }

            for (auto l : listeners->second)
            {
                (*l)(event);
            }
        }
    }
}
