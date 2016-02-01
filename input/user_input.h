//
// Created by pav on 28.09.15.
//

#ifndef DAR_C_USER_INPUT_H
#define DAR_C_USER_INPUT_H

#include "../core/window/window.h"

namespace gladius
{
    namespace input
    {
        enum e_keys : uint32_t
        {
            key_escape = 0,
            key_1,
            key_2,
            key_3,
            key_4,
            key_5,
            key_6,
            key_7,
            key_8,
            key_9,
            key_0,
            key_minus,
            key_equals,
            key_back,
            key_tab,
            key_q,
            key_w,
            key_e,
            key_r,
            key_t,
            key_y,
            key_u,
            key_i,
            key_o,
            key_p,
            key_lbracket,
            key_rbracket,
            key_return,
            key_lcontrol,
            key_a,
            key_s,
            key_d,
            key_f,
            key_g,
            key_h,
            key_j,
            key_k,
            key_l,
            key_semicolon,
            key_apostrophe,
            key_grave,
            key_lshift,
            key_backslash,
            key_z,
            key_x,
            key_c,
            key_v,
            key_b,
            key_n,
            key_m,
            key_comma,
            key_period,
            key_slash,
            key_rshift,
            key_multiply,
            key_lalt,
            key_space,
            key_capital,
            key_f1,
            key_f2,
            key_f3,
            key_f4,
            key_f5,
            key_f6,
            key_f7,
            key_f8,
            key_f9,
            key_f10,
            key_numlock,
            key_scroll,
            key_numpad7,
            key_numpad8,
            key_numpad9,
            key_subtract,
            key_numpad4,
            key_numpad5,
            key_numpad6,
            key_add,
            key_numpad1,
            key_numpad2,
            key_numpad3,
            key_numpad0,
            key_decimal,
            key_f11,
            key_f12,
            key_numpadenter,
            key_rcontrol,
            key_divide,
            key_sysrq,
            key_ralt,
            key_pause,
            key_home,
            key_up,
            key_pgup,
            key_left,
            key_right,
            key_end,
            key_down,
            key_pgdn,
            key_insert,
            key_delete,
            key_lwin,
            key_rwin,
            key_apps,

            mouse0,
            mouse1,
            mouse2,
            mouse3,
            mouse4,
            mouse5,
            mouse6,
            mouse7,

            keys_number
        };

        enum e_analog_input
        {
            mouse_x = 0, mouse_y, mouse_scroll, analog_input_number
        };

        bool create(core::c_window *window);

        void release();

        void update(float dt);

        bool is_any_pressed();

        bool is_pressed(e_keys di);

        bool is_first_pressed(e_keys di);

        bool is_released(e_keys di);

        bool is_first_released(e_keys di);

        float get_duration_pressed(e_keys di);

        float get_analog_input(e_analog_input ai);

        float get_time_corrected(e_analog_input ai);
    }
}


#endif //DAR_C_USER_INPUT_H
