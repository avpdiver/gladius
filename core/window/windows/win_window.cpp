//
// Created by pav on 28.09.15.
//

#include "../../platform.h"

#ifdef WINDOWS

#include "../window.h"

namespace gladius
{
    namespace core
    {
        LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

        c_window::c_window() : m_hwnd(nullptr)
        {

        }

        bool c_window::create()
        {
            HINSTANCE hinst = GetModuleHandle(0);

            // Register class
            WNDCLASSEX wcex;
            wcex.cbSize        = sizeof(WNDCLASSEX);
            wcex.style         = CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc   = WndProc;
            wcex.cbClsExtra    = 0;
            wcex.cbWndExtra    = 0;
            wcex.hInstance     = hinst;
            wcex.hIcon         = LoadIcon(hinst, IDI_APPLICATION);
            wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
            wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
            wcex.lpszMenuName  = nullptr;
            wcex.lpszClassName = "gladius";
            wcex.hIconSm       = LoadIcon(hinst, IDI_APPLICATION);

            if (RegisterClassEx(&wcex) == 0)
            {
                return false;
            }

            RECT rc = {0, 0, 500, 500};
            AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

            m_hwnd = CreateWindow("gladius", "gladius", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hinst, nullptr);
            if (m_hwnd == nullptr)
            {
                return false;
            }

            SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        }

        void c_window::close()
        {
            if (m_hwnd == nullptr)
            {
                return;
            }

            DestroyWindow(m_hwnd);
            m_hwnd = nullptr;
        }

        void c_window::show()
        {
            ShowWindow(m_hwnd, SW_SHOWDEFAULT);
        }

        void c_window::hide()
        {
            ShowWindow(m_hwnd, SW_HIDE);
        }

        void c_window::process_events()
        {
            MSG msg = {};
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        bool c_window::is_closed()
        {
            return m_hwnd == nullptr;
        }

        HWND c_window::getHWND()
        {
            return m_hwnd;
        }

        LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            c_window *window = reinterpret_cast<c_window *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

            switch (message)
            {
                case WM_PAINT:
                {
                    PAINTSTRUCT ps;
                    HDC         hdc = BeginPaint(hWnd, &ps);
                    EndPaint(hWnd, &ps);
                    break;
                }

                case WM_SHOWWINDOW:
                    if (wParam)
                    {
                        window->process_event(e_window_event::ON_SHOW, nullptr);
                    }
                    else
                    {
                        window->process_event(e_window_event::ON_HIDE, nullptr);
                    }
                    break;

                case WM_SIZE:
                    window->process_event(e_window_event::ON_RESIZE, &lParam);
                    //Graphics::Resize((UINT)(UINT64)lParam & 0xFFFF, (UINT)(UINT64)lParam >> 16);
                    break;

                case WM_CLOSE:
                    window->process_event(e_window_event::ON_CLOSE, nullptr);
                    window->close();
                    break;

                case WM_DESTROY:
                    PostQuitMessage(0);
                    break;

                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }

            return 0;
        }
    }
}

#endif
