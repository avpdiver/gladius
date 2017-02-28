#include <iostream>
#include <future>
#include <algorithm>
#include "gladius/gladius.h"
#include "gladius/core/threading/worker_pool.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif

#ifdef PLATFORM_LINUX
#undef main

int main(int argc, char *argv[])
#endif
{
    gladius::s_gladius_desc desc = gladius::s_gladius_desc()
            .set_name("gladius engine")
            .logging_enabled(true)
            .set_window_desc(gladius::core::s_window_desc()
                                     .set_caption("gladius window")
                                     .set_height(1920)
                                     .set_width(1080)
                                     .is_fullscreen(false));

    gladius::c_gladius engine;

    engine.start(desc);

    return 0;
}