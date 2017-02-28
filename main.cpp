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
int main (int argc, char *argv[])
#endif
{
    /*gladius::g_gladius_desc.logging = true;
    gladius::g_gladius_desc.name = "gladius";
    gladius::g_gladius_desc.screen.caption = "GLADIUS";
    gladius::g_gladius_desc.screen.fullscreen = false;
    gladius::g_gladius_desc.screen.width = 1024;
    gladius::g_gladius_desc.screen.height = 768;
	gladius::start();*/

    std::cout << "Main thread id: " << std::this_thread::get_id()
              << std::endl;
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 20; ++i)
    {
        auto fut = std::async([]
                              {
                                  std::this_thread::sleep_for(std::chrono::seconds(1));
                                  std::cout << std::this_thread::get_id() << " ";
                              });
        futures.push_back(std::move(fut));
    }
    std::for_each(futures.begin(), futures.end(), [](std::future<void> & fut)
    {
        fut.wait();
    });
    std::cout << std::endl;

    return 0;
}