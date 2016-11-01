#include "gladius/gladius.h"

#include "gladius/ecs/system.h"


struct system1 {

};

struct system2 {

};

struct system3 {

};

struct system4 {

};

#ifdef PLATFORM_WINDOWS
#include <windows.h>
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif

#ifdef PLATFORM_LINUX
#undef main
int main (int argc, char *argv[])
#endif
{
	gladius::ecs::ecs_system<system1> s1;
    gladius::ecs::ecs_system<system2, system1> s2;
    gladius::ecs::ecs_system<system3, system2, system1> s3;
    gladius::ecs::ecs_system<system4, system3, system2, system1> s4;

    auto d1 = s1.dependency_level;
    auto d2 = s2.dependency_level;
    auto d3 = s3.dependency_level;
    auto d4 = s4.dependency_level;

    return 0;

	/*
	gladius::core::threading::c_worker_pool pool;
	for (int i = 0; i < 50; i++) {
		pool.push([=] {
			std::cout << std::this_thread::get_id() << "\t" << i << "\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
		});
	}

	std::this_thread::sleep_for(std::chrono::seconds(30));
	*/
	//gladius::start("gladius.json");
}