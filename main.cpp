#include <iostream>

#include "gladius/core/logging/logging.h"
#include "gladius/core/window/window.h"
#include "gladius/graphics/render3d/render3d.h"

#include "gladius/core/filesystem/filesystem.h"

using namespace std;

#undef main

#ifdef PLATFORM_WINDOWS
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif

#ifdef PLATFORM_LINUX
int main (int argc, char *argv[])
#endif
{
    gladius::core::filesystem::init();

    gladius::core::filesystem::open("disk:disk",
                                    "D:/dev/gamedev/gladius/CMakeLists.txt",
                                    gladius::core::filesystem::e_file_mode::read);
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
        if (!gladius::graphics::render3d::render())
        {
            break;
        }
    }

    gladius::core::filesystem::shutdown();

    return 0;
}