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
    gladius::g_gladius_desc.logging = true;
    gladius::g_gladius_desc.name = "gladius";
    gladius::g_gladius_desc.screen.caption = "GLADIUS";
    gladius::g_gladius_desc.screen.fullscreen = false;
    gladius::g_gladius_desc.screen.width = 1024;
    gladius::g_gladius_desc.screen.height = 768;

	gladius::start();
    return 0;
}