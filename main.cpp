#include <iostream>
#include "gladius/core/logging/logging.h"
#include "gladius/core/window/window.h"
#include "gladius/graphics/render3d/render3d.h"

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

    if (!gladius::graphics::render3d::init(&window, false))
    {
        const char* err = gladius::core::logging::get_error();
        exit(-1);
    }

    while (!window.is_closed())
    {
        SDL_PumpEvents();
        if (!gladius::graphics::render3d::render())
        {
            const char* err = gladius::core::logging::get_error();
            break;
        }
    }

    gladius::graphics::render3d::shutdown();

    SDL_Quit();

    return 0;
}