//
// Created by avpdiver on 31.08.16.
//

#include "gladius.h"
#include "core/window/window.h"
#include "core/logger/logger.h"
#include "graphics/render3d/render3d.h"

namespace gladius {

core::c_window g_window;

void shutdown();

bool start() {
	if (!core::logger::init(core::logger::e_log_level::debug)) {
		shutdown();
		return false;
	}

	if (!g_window.create()) {
		shutdown();
		return false;
	}

	if (!graphics::render3d::init(&g_window, false)) {
		shutdown();
		return false;
	}

	while (true) {
		g_window.process_events();
		if (g_window.is_closed()) {
			break;
		}
		if (!graphics::render3d::render()) {
			break;
		}
	}

	shutdown();
	return true;
}

void shutdown() {
	graphics::render3d::shutdown();
	g_window.close();
	core::logger::shutdown();
}

}