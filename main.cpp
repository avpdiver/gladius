#include <iostream>

#include "gladius/core/logging/logging.h"
#include "gladius/core/window/window.h"
#include "gladius/graphics/render3d/render3d.h"
#include "gladius/core/filesystem/filesystem.h"

using namespace std;
using namespace gladius;

#undef main

#ifdef PLATFORM_WINDOWS
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif

#ifdef PLATFORM_LINUX
int main (int argc, char *argv[])
#endif
{
	core::logging::init(core::logging::e_log_level::debug);
	core::c_window window;

	window.create();

	/*if (!graphics::render3d::init(&window, false)) {
		exit(-1);
	}*/

	while (true) {
		window.process_events();
		if (window.is_closed()) {
			break;
		}
		/*if (!graphics::render3d::render()) {
			break;
		}*/
	}

	core::logging::shutdown();

	return 0;
}