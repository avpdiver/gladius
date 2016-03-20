//
// Created by pav on 28.09.15.
//


#include <algorithm>
#include "window.h"
#include "SDL2/SDL_syswm.h"
#include "../logging/logging.h"

namespace gladius
{
    namespace core
    {
        std::unordered_map<size_t, c_window*> g_windows;

        int watch_events(void *userdata, SDL_Event* event)
        {
            if (event->type == SDL_WINDOWEVENT)
            {
                c_window *window = g_windows[event->window.windowID];
                if (window == nullptr)
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "Window %d is unknown", event->window.windowID);
                    return 1;
                }

                switch (event->window.event)
                {
                    case SDL_WINDOWEVENT_SHOWN:
                        SDL_Log("Window %d shown", event->window.windowID);
                        window->process_event(e_window_event::ON_SHOW, nullptr);
                        break;

                    case SDL_WINDOWEVENT_HIDDEN:
                        SDL_Log("Window %d hidden", event->window.windowID);
                        window->process_event(e_window_event::ON_HIDE, nullptr);
                        break;

                    case SDL_WINDOWEVENT_EXPOSED:
                        SDL_Log("Window %d exposed", event->window.windowID);
                        break;

                    case SDL_WINDOWEVENT_MOVED:
                        SDL_Log("Window %d moved to %d,%d", event->window.windowID, event->window.data1,
                                event->window.data2);
                        break;
                    case SDL_WINDOWEVENT_RESIZED:
                        SDL_Log("Window %d resized to %dx%d", event->window.windowID, event->window.data1,
                                event->window.data2);
                        window->process_event(e_window_event::ON_RESIZE, &event->window);
                        break;

                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        SDL_Log("Window %d size changed to %dx%d", event->window.windowID, event->window.data1,
                                event->window.data2);
                        window->process_event(e_window_event::ON_RESIZE, &event->window);
                        break;

                    case SDL_WINDOWEVENT_MINIMIZED:
                        SDL_Log("Window %d minimized", event->window.windowID);
                        break;

                    case SDL_WINDOWEVENT_MAXIMIZED:
                        SDL_Log("Window %d maximized", event->window.windowID);
                        break;

                    case SDL_WINDOWEVENT_RESTORED:
                        SDL_Log("Window %d restored", event->window.windowID);
                        break;

                    case SDL_WINDOWEVENT_ENTER:
                        SDL_Log("Mouse entered window %d", event->window.windowID);
                        break;

                    case SDL_WINDOWEVENT_LEAVE:
                        SDL_Log("Mouse left window %d", event->window.windowID);
                        break;

                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        SDL_Log("Window %d gained keyboard focus", event->window.windowID);
                        break;

                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        SDL_Log("Window %d lost keyboard focus", event->window.windowID);
                        break;

                    case SDL_WINDOWEVENT_CLOSE:
                        SDL_Log("Window %d closed", event->window.windowID);
                        window->process_event(e_window_event::ON_CLOSE, nullptr);
                        window->close();
                        break;

                    default:
                        SDL_Log("Window %d got unknown event %d", event->window.windowID, event->window.event);
                        break;
                }

                return 1;
            }

            return 0;
        }

        c_window::c_window()
                :m_window(nullptr)
        {
        }

        c_window::~c_window()
        {
            close();
        }

        bool c_window::create()
        {
            if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
            {
                if (SDL_Init(SDL_INIT_VIDEO) != 0)
                {
                    return false;
                }

                SDL_AddEventWatch(watch_events, nullptr);
            }

            m_window = SDL_CreateWindow("GLadius", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
                    SDL_WINDOW_SHOWN);
            if (m_window == nullptr)
            {
                return false;
            }

            SDL_VERSION(&m_sys_wm_info.version);
            SDL_bool res = SDL_GetWindowWMInfo(m_window, &m_sys_wm_info);
            if (res == SDL_FALSE)
            {
                SET_ERROR("Get window info: %s", SDL_GetError());
                close();
                return false;
            }

            g_windows[SDL_GetWindowID(m_window)] = this;

            return true;
        }

        void c_window::close()
        {
            if (m_window != nullptr)
            {
                SDL_DestroyWindow(m_window);
                g_windows.erase(SDL_GetWindowID(m_window));
                m_window = nullptr;
                //SDL_DelEventWatch(watch_events, nullptr);
            }
        }

        bool c_window::is_closed()
        {
            return m_window == nullptr;
        }

        void c_window::add_event_listener(e_window_event event, window_event_listener_t* listener)
        {
            m_listeners[event].push_back(listener);
        }

        void c_window::remove_event_listener(window_event_listener_t* listener)
        {
            for (auto& p : m_listeners)
            {
                auto e = std::remove_if(p.second.begin(), p.second.end(),
                        [listener](window_event_listener_t* l) -> bool { return l == listener; });
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
