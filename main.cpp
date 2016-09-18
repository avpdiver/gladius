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
	//gladius::core::threading::c_worker_pool pool;

	gladius::start("gladius.json");
	return 0;
}