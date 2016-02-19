#include <iostream>
#include "core/logging/logging.h"
#include "core/window/window.h"
#include "graphics/renderer3d.h"

using namespace std;

//#undef main

int main (int argc, char *argv[])
{
    gladius::core::c_window window;

    window.create();

    if (!gladius::graphics::renderer3d::init(&window, false))
    {
        window.close();
        return 1;
    }

    const char* error = gladius::core::logging::get_error();

    while (!window.is_closed())
    {
        SDL_PumpEvents();
    }

    gladius::graphics::renderer3d::shutdown();

    return 0;
}