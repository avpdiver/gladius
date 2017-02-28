//
// Created by avpdiver on 31.08.16.
//

#include "gladius.h"

#include "core/filesystem/filesystem.h"
#include "graphics/render3d/renderer3d.h"

namespace gladius {

void c_gladius::shutdown() {
	graphics::render3d::renderer3d.shutdown();
	m_window.close();
	core::filesystem::shutdown();
	core::logger::shutdown();
}

bool c_gladius::start(const s_gladius_desc& desc) {

	if (!core::filesystem::init()) {
		shutdown();
		return false;
	}

	if (desc.logging && !core::logger::init(core::logger::e_log_level::debug)) {
		shutdown();
		return false;
	}

	if (!m_window.create(desc.window_desc)) {
		shutdown();
		return false;
	}

	if (!graphics::render3d::renderer3d.init(&m_window)) {
		shutdown();
		return false;
	}

	while (true) {
		m_window.process_events();
		if (m_window.is_closed()) {
			break;
		}
		if (!graphics::render3d::renderer3d.render()) {
			break;
		}
	}

	shutdown();
	return true;
}


}