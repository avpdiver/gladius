//
// Created by pav on 28.09.15.
//

#ifdef  PLATFORM_WINDOWS

#include "window.h"
#include "../logging/logging.h"

namespace gladius
{
    namespace core
    {
        std::unordered_map<HWND, c_window*> g_windows;

        LRESULT CALLBACK wnd_proc(HWND,UINT,WPARAM,LPARAM);

        bool c_window::create ()
        {
            TCHAR app_name[] = TEXT ("gladius");
            WNDCLASS wndclass;

            m_system_info.instance = GetModuleHandle(nullptr);

            wndclass.style = CS_HREDRAW | CS_VREDRAW;
            wndclass.lpfnWndProc = wnd_proc;
            wndclass.cbClsExtra = 0;
            wndclass.cbWndExtra = 0;
            wndclass.hInstance = m_system_info.instance;
            wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
            wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
            wndclass.lpszMenuName = nullptr;
            wndclass.lpszClassName = app_name;

            if (!RegisterClass(&wndclass)) {
                return false;
            }

            m_system_info.handle = CreateWindow (app_name, TEXT("Environment List Box"),
                                 WS_OVERLAPPEDWINDOW,
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 nullptr, nullptr, m_system_info.instance, nullptr);

            ShowWindow(m_system_info.handle, SW_SHOW);

            g_windows[m_system_info.handle] = this;

            return true;
        }

        void c_window::close ()
        {
            CloseWindow(m_system_info.handle);
            m_system_info.handle = nullptr;
        }

        bool c_window::is_closed ()
        {
            return (m_system_info.handle == nullptr);
        }

        void c_window::process_events ()
        {
            MSG msg;
            while (PeekMessage(&msg, m_system_info.handle, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        LRESULT CALLBACK wnd_proc (HWND handle, UINT msg, WPARAM wparam, LPARAM lparam)
        {
            auto window = g_windows.find(handle);
            if (window != g_windows.end())
            {
                switch (msg)
                {
                    case WM_CLOSE:
                        (window->second)->process_event(e_window_event::ON_CLOSE, nullptr);
                        (window->second)->close();
                        return 0;
                }
            }
            return DefWindowProc(handle, msg, wparam, lparam);
        }
    }
}

#endif