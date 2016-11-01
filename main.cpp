#include "gladius/gladius.h"

#include "gladius/core/threading/worker_pool.h"
#include "gladius/core/meta/type_id.h"
#include "gladius/core/meta/list.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif

#ifdef PLATFORM_LINUX
#undef main
int main (int argc, char *argv[])
#endif
{
	using ltype = typename gladius::core::meta::make_list<uint32_t, uint64_t>::type;


    auto l = gladius::core::meta::type_list_length<ltype>::value;

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
	return 0;
}