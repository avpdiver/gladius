//
// Created by pav on 28.09.15.
//

#include "../../platform.h"

#ifdef UNIX

#include "../window.h"

#include <X11/extensions/XInput2.h>

namespace gladius
{
    namespace core
    {
        const int g_event_mask = ExposureMask |
                                 ButtonPressMask |
                                 ButtonReleaseMask |
                                 EnterWindowMask |
                                 LeaveWindowMask |
                                 PointerMotionMask |
                                 FocusChangeMask |
                                 KeyPressMask |
                                 KeyReleaseMask |
                                 SubstructureNotifyMask |
                                 StructureNotifyMask |
                                 SubstructureRedirectMask;

        c_window::c_window () : m_display (nullptr), m_window (0)
        {

        }

        Display *c_window::get_display ()
        {
            return m_display;
        }

        Window &c_window::get_window ()
        {
            return m_window;
        }

        bool c_window::is_closed ()
        {
            return m_display == nullptr || m_window == 0;
        }

        bool c_window::create ()
        {
            m_display = XOpenDisplay (0);
            if (m_display == nullptr)
            {
                return false;
            }

            m_window = XCreateSimpleWindow (m_display, DefaultRootWindow(m_display), 0, 0, 500, 500, 0, 0, 0);

            Atom WM_DELETE_WINDOW = XInternAtom (m_display, "WM_DELETE_WINDOW", False);
            XSetWMProtocols (m_display, m_window, &WM_DELETE_WINDOW, 1);

            return true;
        }

        void c_window::close ()
        {
            if (m_display != nullptr)
            {
                if (m_window != 0)
                {
                    XDestroyWindow (m_display, m_window);
                    m_window = 0;
                }
                XCloseDisplay (m_display);
                m_display = nullptr;
            }
        }

        void c_window::show ()
        {
            if (is_closed ())
            {
                return;
            }
            XSelectInput (m_display, m_window, g_event_mask);
            XMapWindow (m_display, m_window);
            XFlush (m_display);
        }

        void c_window::hide ()
        {
            if (is_closed ())
            {
                return;
            }
            XUnmapWindow (m_display, m_window);
            XFlush (m_display);
        }

        void c_window::process_events ()
        {
            if (is_closed ())
            {
                return;
            }

            XEvent event;
            XGenericEventCookie *cookie = &event.xcookie;

            while (XPending (m_display) > 0)
            {
                XNextEvent (m_display, &event);

                if (XGetEventData (m_display, cookie) && cookie->type == GenericEvent)
                {
                    process_event (e_window_event::ON_INPUT, cookie);
                    XFreeEventData (m_display, cookie);
                }

                switch (event.type)
                {
                    case DestroyNotify:
                        process_event (e_window_event::ON_CLOSE, nullptr);
                        break;
                    case MapNotify:
                        process_event (e_window_event::ON_SHOW, nullptr);
                        break;
                    case UnmapNotify:
                        process_event (e_window_event::ON_HIDE, nullptr);
                        break;
                    case ConfigureNotify:
                        process_event (e_window_event::ON_RESIZE, nullptr);
                        break;
                    case ClientMessage:
                        process_event (e_window_event::ON_CLOSE, nullptr);
                        close ();
                        return;
                }
            }
        }
    }
}

#endif
