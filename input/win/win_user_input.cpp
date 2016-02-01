//
// Created by alexa on 05.12.2015.
//

#include "../../core/platform.h"

#ifdef WINDOWS

#include "../user_input.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace gladius
{
    namespace input
    {
        void process_event(void *data)
        {

        }

        core::window_event_listener_t m_window_event_listener = process_event;
        core::c_window                *m_window               = nullptr;

        IDirectInput8A       *m_direct_input;
        IDirectInputDevice8A *m_keyboard;
        IDirectInputDevice8A *m_mouse;

        bool  m_buttons[2][e_keys::keys_number];
        float m_hold_duration[e_keys::keys_number]            = {0.0f};
        float m_analogs[e_keys::keys_number];
        float m_analogs_time_correct[e_keys::keys_number];

        DIMOUSESTATE2 m_mouse_state;
        unsigned char m_key_buffer[256];
        unsigned char m_keyboard__mapping[e_keys::key_apps + 1]; // map e_keys enum to DX key codes

        void map_keyboard_keys()
        {
            m_keyboard__mapping[e_keys::key_escape]      = 1;
            m_keyboard__mapping[e_keys::key_1]           = 2;
            m_keyboard__mapping[e_keys::key_2]           = 3;
            m_keyboard__mapping[e_keys::key_3]           = 4;
            m_keyboard__mapping[e_keys::key_4]           = 5;
            m_keyboard__mapping[e_keys::key_5]           = 6;
            m_keyboard__mapping[e_keys::key_6]           = 7;
            m_keyboard__mapping[e_keys::key_7]           = 8;
            m_keyboard__mapping[e_keys::key_8]           = 9;
            m_keyboard__mapping[e_keys::key_9]           = 10;
            m_keyboard__mapping[e_keys::key_0]           = 11;
            m_keyboard__mapping[e_keys::key_minus]       = 12;
            m_keyboard__mapping[e_keys::key_equals]      = 13;
            m_keyboard__mapping[e_keys::key_back]        = 14;
            m_keyboard__mapping[e_keys::key_tab]         = 15;
            m_keyboard__mapping[e_keys::key_q]           = 16;
            m_keyboard__mapping[e_keys::key_w]           = 17;
            m_keyboard__mapping[e_keys::key_e]           = 18;
            m_keyboard__mapping[e_keys::key_r]           = 19;
            m_keyboard__mapping[e_keys::key_t]           = 20;
            m_keyboard__mapping[e_keys::key_y]           = 21;
            m_keyboard__mapping[e_keys::key_u]           = 22;
            m_keyboard__mapping[e_keys::key_i]           = 23;
            m_keyboard__mapping[e_keys::key_o]           = 24;
            m_keyboard__mapping[e_keys::key_p]           = 25;
            m_keyboard__mapping[e_keys::key_lbracket]    = 26;
            m_keyboard__mapping[e_keys::key_rbracket]    = 27;
            m_keyboard__mapping[e_keys::key_return]      = 28;
            m_keyboard__mapping[e_keys::key_lcontrol]    = 29;
            m_keyboard__mapping[e_keys::key_a]           = 30;
            m_keyboard__mapping[e_keys::key_s]           = 31;
            m_keyboard__mapping[e_keys::key_d]           = 32;
            m_keyboard__mapping[e_keys::key_f]           = 33;
            m_keyboard__mapping[e_keys::key_g]           = 34;
            m_keyboard__mapping[e_keys::key_h]           = 35;
            m_keyboard__mapping[e_keys::key_j]           = 36;
            m_keyboard__mapping[e_keys::key_k]           = 37;
            m_keyboard__mapping[e_keys::key_l]           = 38;
            m_keyboard__mapping[e_keys::key_semicolon]   = 39;
            m_keyboard__mapping[e_keys::key_apostrophe]  = 40;
            m_keyboard__mapping[e_keys::key_grave]       = 41;
            m_keyboard__mapping[e_keys::key_lshift]      = 42;
            m_keyboard__mapping[e_keys::key_backslash]   = 43;
            m_keyboard__mapping[e_keys::key_z]           = 44;
            m_keyboard__mapping[e_keys::key_x]           = 45;
            m_keyboard__mapping[e_keys::key_c]           = 46;
            m_keyboard__mapping[e_keys::key_v]           = 47;
            m_keyboard__mapping[e_keys::key_b]           = 48;
            m_keyboard__mapping[e_keys::key_n]           = 49;
            m_keyboard__mapping[e_keys::key_m]           = 50;
            m_keyboard__mapping[e_keys::key_comma]       = 51;
            m_keyboard__mapping[e_keys::key_period]      = 52;
            m_keyboard__mapping[e_keys::key_slash]       = 53;
            m_keyboard__mapping[e_keys::key_rshift]      = 54;
            m_keyboard__mapping[e_keys::key_multiply]    = 55;
            m_keyboard__mapping[e_keys::key_lalt]        = 56;
            m_keyboard__mapping[e_keys::key_space]       = 57;
            m_keyboard__mapping[e_keys::key_capital]     = 58;
            m_keyboard__mapping[e_keys::key_f1]          = 59;
            m_keyboard__mapping[e_keys::key_f2]          = 60;
            m_keyboard__mapping[e_keys::key_f3]          = 61;
            m_keyboard__mapping[e_keys::key_f4]          = 62;
            m_keyboard__mapping[e_keys::key_f5]          = 63;
            m_keyboard__mapping[e_keys::key_f6]          = 64;
            m_keyboard__mapping[e_keys::key_f7]          = 65;
            m_keyboard__mapping[e_keys::key_f8]          = 66;
            m_keyboard__mapping[e_keys::key_f9]          = 67;
            m_keyboard__mapping[e_keys::key_f10]         = 68;
            m_keyboard__mapping[e_keys::key_numlock]     = 69;
            m_keyboard__mapping[e_keys::key_scroll]      = 70;
            m_keyboard__mapping[e_keys::key_numpad7]     = 71;
            m_keyboard__mapping[e_keys::key_numpad8]     = 72;
            m_keyboard__mapping[e_keys::key_numpad9]     = 73;
            m_keyboard__mapping[e_keys::key_subtract]    = 74;
            m_keyboard__mapping[e_keys::key_numpad4]     = 75;
            m_keyboard__mapping[e_keys::key_numpad5]     = 76;
            m_keyboard__mapping[e_keys::key_numpad6]     = 77;
            m_keyboard__mapping[e_keys::key_add]         = 78;
            m_keyboard__mapping[e_keys::key_numpad1]     = 79;
            m_keyboard__mapping[e_keys::key_numpad2]     = 80;
            m_keyboard__mapping[e_keys::key_numpad3]     = 81;
            m_keyboard__mapping[e_keys::key_numpad0]     = 82;
            m_keyboard__mapping[e_keys::key_decimal]     = 83;
            m_keyboard__mapping[e_keys::key_f11]         = 87;
            m_keyboard__mapping[e_keys::key_f12]         = 88;
            m_keyboard__mapping[e_keys::key_numpadenter] = 156;
            m_keyboard__mapping[e_keys::key_rcontrol]    = 157;
            m_keyboard__mapping[e_keys::key_divide]      = 181;
            m_keyboard__mapping[e_keys::key_sysrq]       = 183;
            m_keyboard__mapping[e_keys::key_ralt]        = 184;
            m_keyboard__mapping[e_keys::key_pause]       = 197;
            m_keyboard__mapping[e_keys::key_home]        = 199;
            m_keyboard__mapping[e_keys::key_up]          = 200;
            m_keyboard__mapping[e_keys::key_pgup]        = 201;
            m_keyboard__mapping[e_keys::key_left]        = 203;
            m_keyboard__mapping[e_keys::key_right]       = 205;
            m_keyboard__mapping[e_keys::key_end]         = 207;
            m_keyboard__mapping[e_keys::key_down]        = 208;
            m_keyboard__mapping[e_keys::key_pgdn]        = 209;
            m_keyboard__mapping[e_keys::key_insert]      = 210;
            m_keyboard__mapping[e_keys::key_delete]      = 211;
            m_keyboard__mapping[e_keys::key_lwin]        = 219;
            m_keyboard__mapping[e_keys::key_rwin]        = 220;
            m_keyboard__mapping[e_keys::key_apps]        = 221;
        }

        void clear_state()
        {
            memset(&m_mouse_state, 0, sizeof(DIMOUSESTATE2));
            memset(m_key_buffer, 0, sizeof(m_key_buffer));
        }

        bool create_direct_input()
        {
            if (FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void **) &m_direct_input, nullptr)))
            {
                return false;
            }
        }

        bool create_keyboard()
        {
            map_keyboard_keys();

            if (FAILED(m_direct_input->CreateDevice(GUID_SysKeyboard, &m_keyboard, nullptr)))
            {
                return false;
            }

            if (FAILED(m_keyboard->SetDataFormat(&c_dfDIKeyboard)))
            {
                return false;
            }

            if (FAILED(m_keyboard->SetCooperativeLevel(m_window->getHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
            {
                return false;
            }

            DIPROPDWORD dipdw;
            dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
            dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
            dipdw.diph.dwObj        = 0;
            dipdw.diph.dwHow        = DIPH_DEVICE;
            dipdw.dwData            = 10;

            if (FAILED(m_keyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
            {
                return false;
            }
        }

        bool create_mouse()
        {
            if (FAILED(m_direct_input->CreateDevice(GUID_SysMouse, &m_mouse, nullptr)))
            {
                return false;
            }

            if (FAILED(m_mouse->SetDataFormat(&c_dfDIMouse2)))
            {
                return false;
            }

            if (FAILED(m_mouse->SetCooperativeLevel(m_window->getHWND(), DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
            {
                return false;
            }

            clear_state();
        }

        void release_keyboard()
        {
            if (m_keyboard)
            {
                m_keyboard->Unacquire();
                m_keyboard->Release();
                m_keyboard = nullptr;
            }
        }

        void release_mouse()
        {
            if (m_mouse)
            {
                m_mouse->Unacquire();
                m_mouse->Release();
                m_mouse = nullptr;
            }
        }

        void release_direct_input()
        {
            if (m_direct_input)
            {
                m_direct_input->Release();
                m_direct_input = nullptr;
            }
        }

        void update_keyboard()
        {
            m_keyboard->Acquire();
            m_keyboard->GetDeviceState(sizeof(m_key_buffer), m_key_buffer);
        }

        void update_mouse()
        {
            m_mouse->Acquire();
            m_mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &m_mouse_state);
        }

        bool create(core::c_window *window)
        {
            m_window = window;
            create_direct_input();
            create_keyboard();
            create_mouse();
        }

        void release()
        {
            release_keyboard();
            release_mouse();
            release_direct_input();
        }

        void update(float frameDelta)
        {
            memcpy(m_buttons[1], m_buttons[0], sizeof(m_buttons[0]));
            memset(m_buttons[0], 0, sizeof(m_buttons[0]));
            memset(m_analogs, 0, sizeof(m_analogs));

            HWND foreground = GetForegroundWindow();
            bool visible    = IsWindowVisible(foreground) != 0;

            if (foreground != m_window->getHWND() // wouldn't be able to acquire
                || !visible)
            {
                clear_state();
                return;
            }

            update_keyboard();
            update_mouse();

            for (uint32_t i = 0; i <= e_keys::key_apps; ++i)
            {
                m_buttons[0][i] = (m_key_buffer[m_keyboard__mapping[i]] & 0x80) != 0;
            }

            for (uint32_t i = 0; i < 8; ++i)
            {
                if (m_mouse_state.rgbButtons[i] > 0)
                    m_buttons[0][e_keys::mouse0 + i] = true;
            }

            m_analogs[e_analog_input::mouse_x] = (float) m_mouse_state.lX * .0018f;
            m_analogs[e_analog_input::mouse_y] = (float) m_mouse_state.lY * -.0018f;

            if (m_mouse_state.lZ > 0)
            {
                m_analogs[e_analog_input::mouse_scroll] = 1.0f;
            }
            else if (m_mouse_state.lZ < 0)
            {
                m_analogs[e_analog_input::mouse_scroll] = -1.0f;
            }

            // Update time duration for buttons pressed
            for (uint32_t i = 0; i < e_keys::keys_number; ++i)
            {
                if (m_buttons[0][i])
                {
                    if (!m_buttons[1][i])
                        m_hold_duration[i] = 0.0f;
                    else
                        m_hold_duration[i] += frameDelta;
                }
            }

            for (uint32_t i = 0; i < e_analog_input::analog_input_number; ++i)
            {
                m_analogs_time_correct[i] = m_analogs[i] * frameDelta;
            }

        }

        bool is_any_pressed(void)
        {
            return m_buttons[0] != 0;
        }

        bool is_pressed(e_keys di)
        {
            return m_buttons[0][di];
        }

        bool is_first_pressed(e_keys di)
        {
            return m_buttons[0][di] && !m_buttons[1][di];
        }

        bool is_released(e_keys di)
        {
            return !m_buttons[0][di];
        }

        bool is_first_released(e_keys di)
        {
            return !m_buttons[0][di] && m_buttons[1][di];
        }

        float get_duration_pressed(e_keys di)
        {
            return m_hold_duration[di];
        }

        float get_analog_input(e_analog_input ai)
        {
            return m_analogs[ai];
        }

        float get_time_corrected(e_analog_input ai)
        {
            return m_analogs_time_correct[ai];
        }
    }
}

#endif