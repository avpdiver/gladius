#include "gladius/gladius.h"

#include "gladius/core/threading/worker_pool.h"
#include "gladius/core/meta/type_id.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif

#ifdef PLATFORM_LINUX
#undef main
int main (int argc, char *argv[])
#endif
{

    size_t id1 = gladius::core::meta::type_id<uint64_t>();
    size_t id2 = gladius::core::meta::type_id<uint32_t>();

	gladius::core::threading::c_worker_pool pool;
	for (int i = 0; i < 50; i++) {
		pool.push([=] {
			std::cout << std::this_thread::get_id() << "\t" << i << "\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
		});
	}

	std::this_thread::sleep_for(std::chrono::seconds(30));

	//gladius::start("gladius.json");
	return 0;
}