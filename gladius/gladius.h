//
// Created by avpdiver on 31.08.16.
//

#ifndef GLADIUS_GLADIUS_H
#define GLADIUS_GLADIUS_H

#include <string>

#include "core/window/window.h"

namespace gladius {

struct s_gladius_desc {
	std::string name ="gladius";
	bool logging = true;
	core::s_window_desc screen;
};

extern s_gladius_desc g_gladius_desc;

bool start();
}

#endif //GLADIUS_GLADIUS_H
