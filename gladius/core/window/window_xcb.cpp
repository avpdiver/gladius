//
// Created by pav on 28.09.15.
//

#ifdef  PLATFORM_LINUX

#include <cstring>
#include "window.h"
#include "../logger/logger.h"

namespace gladius {
namespace core {

bool c_window::create() {
	int screen_index;
	m_window_info.connection = xcb_connect(nullptr, &screen_index);

	if (!m_window_info.connection) {
		return false;
	}

	const xcb_setup_t *setup = xcb_get_setup(m_window_info.connection);
	xcb_screen_iterator_t screen_iterator = xcb_setup_roots_iterator(setup);

	while (screen_index-- > 0) {
		xcb_screen_next(&screen_iterator);
	}

	xcb_screen_t *screen = screen_iterator.data;

	m_window_info.handle = xcb_generate_id(m_window_info.connection);

	uint32_t value_list[] = {
		screen->white_pixel,
		XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_STRUCTURE_NOTIFY
	};

	xcb_create_window(
		m_window_info.connection,
		XCB_COPY_FROM_PARENT,
		m_window_info.handle,
		screen->root,
		20,
		20,
		500,
		500,
		0,
		XCB_WINDOW_CLASS_INPUT_OUTPUT,
		screen->root_visual,
		XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK,
		value_list);

	xcb_change_property(
		m_window_info.connection,
		XCB_PROP_MODE_REPLACE,
		m_window_info.handle,
		XCB_ATOM_WM_NAME,
		XCB_ATOM_STRING,
		8,
		strlen("gladius"),
		"gladius");

	// Prepare notification for window destruction
	xcb_intern_atom_cookie_t protocols_cookie = xcb_intern_atom(m_window_info
																	.connection, 1, 12, "WM_PROTOCOLS");
	xcb_intern_atom_reply_t *protocols_reply = xcb_intern_atom_reply(m_window_info
																		 .connection, protocols_cookie, 0);
	xcb_intern_atom_cookie_t delete_cookie = xcb_intern_atom(m_window_info
																 .connection, 0, 16, "WM_DELETE_WINDOW");
	m_window_info.delete_reply = xcb_intern_atom_reply(m_window_info.connection, delete_cookie, 0);
	xcb_change_property(m_window_info.connection, XCB_PROP_MODE_REPLACE, m_window_info
		.handle, (*protocols_reply).atom, 4, 32, 1, &(*m_window_info.delete_reply).atom);
	free(protocols_reply);

	// Display window
	xcb_map_window(m_window_info.connection, m_window_info.handle);
	xcb_flush(m_window_info.connection);

	return true;
}

void c_window::close() {
	free(m_window_info.delete_reply);
	m_window_info.delete_reply = nullptr;

	if (m_window_info.connection != nullptr) {
		xcb_destroy_window(m_window_info.connection, m_window_info.handle);
		xcb_disconnect(m_window_info.connection);
		m_window_info.connection = nullptr;
	}
}

bool c_window::is_closed() {
	return m_window_info.connection == nullptr;
}

void c_window::process_events() {
	xcb_generic_event_t *event;
	while (true) {
		event = xcb_poll_for_event(m_window_info.connection);
		if (event) {

			switch (event->response_type & 0x7f) {
				// Resize
			case XCB_CONFIGURE_NOTIFY: {
				xcb_configure_notify_event_t *configure_event = (xcb_configure_notify_event_t *) event;
				static uint16_t width = configure_event->width;
				static uint16_t height = configure_event->height;

				if (((configure_event->width > 0) && (width != configure_event->width)) ||
					((configure_event->height > 0) && (height != configure_event->height))) {
					width = configure_event->width;
					height = configure_event->height;
				}
				break;
			}

				// Close
			case XCB_CLIENT_MESSAGE:
				if ((*(xcb_client_message_event_t *) event).data.data32[0] == (*m_window_info.delete_reply).atom) {
					process_event(e_window_event::ON_CLOSE, nullptr);
					close();
					return;
				}
				break;
			case XCB_KEY_PRESS:
				break;
			}

			free(event);
		} else {
			break;
		}
	}
}

}
}

#endif