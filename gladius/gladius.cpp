//
// Created by avpdiver on 31.08.16.
//

#include "gladius.h"

#include "core/logger/logger.h"
#include "core/filesystem/filesystem.h"
#include "core/filesystem/json_file.h"

#include "graphics/render3d/render3d.h"
#include "graphics/render3d/render3d_globals.h"
#include "graphics/render3d/render3d_pipeline.h"

namespace gladius {

s_gladius_desc g_gladius_desc;
core::c_window g_window;

void shutdown();

bool start(const char *config_filename) {

	if (!core::filesystem::init()) {
		shutdown();
		return false;
	}

	core::filesystem::c_json_file *file = reinterpret_cast<core::filesystem::c_json_file *>(
		core::filesystem::open("disk:json", config_filename, core::filesystem::e_file_mode::read));
	file->read(g_gladius_desc);
	core::filesystem::close(file);

	if (g_gladius_desc.logging && !core::logger::init(core::logger::e_log_level::debug)) {
		shutdown();
		return false;
	}

	if (!g_window.create(g_gladius_desc.screen)) {
		shutdown();
		return false;
	}

	if (!graphics::render3d::init(&g_window, false)) {
		shutdown();
		return false;
	}

	graphics::render3d::resources::load_pipeline(g_gladius_desc.pipeline.c_str());

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
	core::filesystem::shutdown();
	core::logger::shutdown();
}

}