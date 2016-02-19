#include <iostream>
#include "core/logging/logging.h"
#include "core/window/window.h"

using namespace std;

//#undef main

int main (int argc, char *argv[])
{
    gladius::core::c_window window;

    window.create();

    const char* error = gladius::core::logging::get_error();

    while (!window.is_closed())
    {
        SDL_PumpEvents();
    }

    return 0;
}