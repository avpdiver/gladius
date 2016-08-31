#include <iostream>

#include "gladius/gladius.h"

#undef main

#ifdef PLATFORM_WINDOWS
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif

#ifdef PLATFORM_LINUX
int main (int argc, char *argv[])
#endif
{
	gladius::start();
	return 0;
}