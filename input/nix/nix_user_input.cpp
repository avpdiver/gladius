//
// Created by pav on 28.09.15.
//

#include "../../core/platform.h"

#ifdef UNIX

#include "../user_input.h"

#include <iostream>
#include <X11/extensions/XInput2.h>

namespace gladius
{
    namespace input
    {
        void process_event (void *data);

        core::window_event_listener_t m_window_event_listener = process_event;
        core::c_window *m_window = nullptr;

        bool create (core::c_window *window)
        {
            m_window = window;

            int event, error;
            if (!XQueryExtension (m_window->get_display (), "XInputExtension", &m_opcode, &event, &error))
            {
                std::cout << "X Input extension not available." << std::endl;
                return false;
            }

            int major = 2, minor = 3;
            if (XIQueryVersion (m_window->get_display (), &major, &minor) == BadRequest)
            {
                std::cout << "XI2 not available. Server supports " << major << "." << minor << std::endl;
                return false;
            }

            XIEventMask event_mask;
            unsigned char mask[2] = { 0, 0 };

            event_mask.deviceid = XIAllMasterDevices;
            event_mask.mask_len = sizeof(mask);
            event_mask.mask = mask;

            XISetMask(event_mask.mask, XI_Enter);
            XISetMask(event_mask.mask, XI_Leave);
            XISetMask(event_mask.mask, XI_Motion);
            XISetMask(event_mask.mask, XI_ButtonPress);
            XISetMask(event_mask.mask, XI_ButtonRelease);
            XISetMask(event_mask.mask, XI_KeyPress);
            XISetMask(event_mask.mask, XI_KeyRelease);

            if (XISelectEvents (m_window->get_display (), m_window->get_window (), &event_mask, 1) != Success)
            {
                return false;
            }

            m_window->add_event_listener (core::e_window_event::ON_INPUT, &m_window_event_listener);
        }

        void release ()
        {
            m_window->remove_event_listener (&m_window_event_listener);
        }

        void process_event (void *data)
        {
            XGenericEventCookie *cookie = static_cast<XGenericEventCookie *>(data);
            if (cookie->extension != m_opcode)
            {
                return;
            }

            XIDeviceEvent *event = static_cast<XIDeviceEvent *>(cookie->data);

            switch (event->evtype)
            {
                case XI_Motion:
                    printf("motion\n");
                    //mouse_motion(static_cast<XIRawEvent*>(cookie->data));
                    break;
                case XI_FocusIn:
                case XI_Enter:
                    printf("enter\n");
                    //XIEnterEvent *ee = static_cast<XIEnterEvent *>(event);
                    break;
                case XI_FocusOut:
                case XI_Leave:
                    printf("leave\n");
                    //XILeaveEvent *le = static_cast<XILeaveEvent *>(event);
                    break;
                case XI_ButtonPress:
                case XI_ButtonRelease:
                    printf("buttons\n");
                    break;
                case XI_KeyPress:
                case XI_KeyRelease:
                    printf("keys\n");
                    break;
            }
        }
    }
}

#endif