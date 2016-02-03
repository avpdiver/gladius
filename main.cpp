#include <iostream>
#include "core/window/window.h"

using namespace std;

int main ()
{
    gladius::core::c_window window;

    window.create();

    while (!window.is_closed())
    {
        SDL_PumpEvents();
    }

    return 0;
}