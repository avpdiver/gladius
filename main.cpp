#include <vector>
#include "gladius/gladius.h"
#include "gladius/core/json.h"
#include "gladius/core/filesystem/filesystem.h"
#include "gladius/core/filesystem/json_file.h"

struct A {
	float a;
	JSON_FIELDS(
			JSON_FIELD(A, a)
	);
};

struct B {
	float b;
	std::vector<A> aa;
	JSON_FIELDS(
			JSON_FIELD(B, b),
			JSON_FIELD(B, aa)
	);
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
    gladius::core::filesystem::init();
    gladius::core::filesystem::c_json_file* file = reinterpret_cast<gladius::core::filesystem::c_json_file*>(
            gladius::core::filesystem::open("disk:json", "test.json", gladius::core::filesystem::e_file_mode::read));

    B b;
    file->read(b);

	//gladius::start();
	return 0;
}