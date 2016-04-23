#include <iostream>
#include "gladius/core/logging/logging.h"
#include "gladius/core/memory/alloc_policies/lockfree_pool_policy.h"
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

    window.create ();

    if (!gladius::graphics::render3d::init (&window, false))
    {
        std::cout << gladius::core::logging::get_error () << std::endl;
        exit (-1);
    }

    while (true)
    {
        window.process_events ();
        if (window.is_closed ())
        {
            break;
        }
        if (!gladius::graphics::render3d::render())
        {
            std::cout <<  gladius::core::logging::get_error() << std::endl;
            break;
        }
    }

    return 0;
}