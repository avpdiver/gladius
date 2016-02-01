#include <iostream>
#include <thread>
#include "core/window/window.h"
#include "input/user_input.h"

using namespace std;

int main ()
{
    cout << "Hello, World!" << endl;

    gladius::core::c_window window;

    window.create();
    window.show();

    gladius::input::create(&window);

    while (!window.is_closed())
    {
        window.process_events();
        gladius::input::update(1.0);
        if (gladius::input::is_pressed(gladius::input::e_keys::key_escape))
        {
            window.close();
        }
    }

    gladius::input::release();

    return 0;
}