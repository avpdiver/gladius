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
	/*gladius::core::collections::c_concurrent_queue<std::function<void(void)>, 8> queue;

	uint64_t v = 123;
    std::function<void(void)> func;

	queue.push([=] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << std::this_thread::get_id() << "\t" << v << "\n\n\n";
    });

    queue.try_pop(func);
    func();*/

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