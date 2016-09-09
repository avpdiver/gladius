//
// Created by avpdiver on 31.08.16.
//

#ifndef GLADIUS_GLADIUS_H
#define GLADIUS_GLADIUS_H

#include <string>

#include "core/json.h"
#include "core/window/window.h"

namespace gladius {

struct s_gladius_desc {
	std::string name ="gladius";
	bool logging = true;
	core::s_window_desc screen;
	std::string pipeline;

	JSON_FIELDS(
		JSON_FIELD(s_gladius_desc, name),
		JSON_FIELD(s_gladius_desc, logging),
		JSON_FIELD(s_gladius_desc, screen),
		JSON_FIELD(s_gladius_desc, pipeline)
	);
};

extern s_gladius_desc g_gladius_desc;

bool start(const char*);
}

#endif //GLADIUS_GLADIUS_H
