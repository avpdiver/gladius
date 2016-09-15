//
// Created by pav on 28.09.15.
//

#ifdef  PLATFORM_WINDOWS

#include "window.h"

namespace gladius
{
    namespace core
    {
        std::unordered_map<HWND, c_window*> g_windows;

        LRESULT CALLBACK wnd_proc(HWND,UINT,WPARAM,LPARAM);

        bool c_window::create (const s_window_desc& desc)
        {
            TCHAR app_name[] = TEXT ("gladius");
            WNDCLASS wndclass;

            m_window_info.instance = GetModuleHandle(nullptr);

            wndclass.style = CS_HREDRAW | CS_VREDRAW;
            wndclass.lpfnWndProc = wnd_proc;
            wndclass.cbClsExtra = 0;
            wndclass.cbWndExtra = 0;
            wndclass.hInstance = m_window_info.instance;
            wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
            wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
            wndclass.lpszMenuName = nullptr;
            wndclass.lpszClassName = app_name;

            if (!RegisterClass(&wndclass)) {
                return false;
            }

            m_window_info.handle = CreateWindow (app_name, TEXT(desc.caption.c_str()),
                                 WS_OVERLAPPEDWINDOW,
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 desc.width, desc.height,
                                 nullptr, nullptr, m_window_info.instance, nullptr);

            ShowWindow(m_window_info.handle, SW_SHOW);

            g_windows[m_window_info.handle] = this;

            return true;
        }

        void c_window::set_size (int width, int height)
        {
            RECT rect;
            if (width != -1)
            {
                GetWindowRect(m_window_info.handle, &rect);
                SetWindowPos(m_window_info.handle, HWND_TOP, rect.left, rect.top, width, height, SWP_NOCOPYBITS);
            }

            GetClientRect(m_window_info.handle, &rect);
            m_window_info.extent.x = rect.right - rect.left;
            m_window_info.extent.y = rect.bottom - rect.top;
            process_event(e_window_event::ON_RESIZE, &(m_window_info.extent));
        }

        void c_window::close ()
        {
            process_event(e_window_event::ON_CLOSE, nullptr);
            CloseWindow(m_window_info.handle);
            m_window_info.handle = nullptr;
        }

        bool c_window::is_closed ()
        {
            return (m_window_info.handle == nullptr);
        }

        void c_window::process_events ()
        {
            MSG msg;
            while (PeekMessage(&msg, m_window_info.handle, 0, 0, PM_REMOVE))
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
                        (window->second)->close();
                        return 0;
                    case WM_SIZE:
                        if (wparam != SIZE_MAXIMIZED)
                            break;
                    case WM_EXITSIZEMOVE:
                        (window->second)->set_size(-1, -1);
                        return 0;
                }
            }
            return DefWindowProc(handle, msg, wparam, lparam);
        }
    }
}

#endif