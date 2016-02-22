#include <iostream>
#include "core/logging/logging.h"
#include "core/window/window.h"
#include "graphics/renderer3d.h"

using namespace std;

#undef main

#ifdef PLATFORM_WINDOWS
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif

#ifdef PLATFORM_LINUX
int main (int argc, char *argv[])
#endif
{
    gladius::core::c_window window;

    window.create();

    if (!gladius::graphics::renderer3d::init(&window, false))
    {
        const char* err = gladius::core::logging::get_error();
        exit(-1);
    }

    while (!window.is_closed())
    {
        SDL_PumpEvents();
    }

    gladius::graphics::renderer3d::shutdown();

    return 0;
}