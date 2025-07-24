#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event_joypad_motion.hpp>

#include "gdwiimote.h"

// Nunchuk status
bool GDWiimote::is_nunchuk_connected() const
{
    return wm && ((wm->exp.type == EXP_NUNCHUK) || (wm->exp.type == EXP_MOTION_PLUS_NUNCHUK));
}

void GDWiimote::initialize_nunchuk()
{
    if (!is_nunchuk_connected())
    {
        godot::UtilityFunctions::print("Nunchuk not connected to Wiimote ", device_id);
        return;
    }
    joystick->initialize_joystick(&wm->exp.nunchuk, nunchuk_deadzone, nunchuk_threshold);
    wiiuse_set_nunchuk_orient_threshold(wm, nunchuk_orient_threshold);
    wiiuse_set_nunchuk_accel_threshold(wm, nunchuk_accel_threshold);
}

void GDWiimote::start_nunchuk_joystick_calibration()
{
    if (!is_calibrating_nunchuk)
    {
        is_calibrating_nunchuk = true;
        joystick->initialize_calibration(wm->exp.nunchuk.js.x, wm->exp.nunchuk.js.y);
    }
    else
    {
        godot::UtilityFunctions::print("Calibration already in progress!");
    }
}

void GDWiimote::stop_nunchuk_joystick_calibration()
{
    if (is_calibrating_nunchuk)
        is_calibrating_nunchuk = false;
    else
        godot::UtilityFunctions::print("Calibration not in progress!");
}

// Nunchuk thresholds
void GDWiimote::set_nunchuk_joystick_deadzone(float dz) { nunchuk_deadzone = dz; }
void GDWiimote::set_nunchuk_joystick_threshold(float dt) { nunchuk_threshold = dt; }

// Poll joystick
void GDWiimote::poll_nunchuk_joystick()
{
    float pos_x = wm->exp.nunchuk.js.x;
    float pos_y = wm->exp.nunchuk.js.y;
    if (joystick->is_motion_detected(pos_x, pos_y))
    {
        pos_x = joystick->normalize_x(pos_x);
        pos_y = joystick->normalize_y(pos_y);

        auto input = godot::Input::get_singleton();

        godot::Ref<godot::InputEventJoypadMotion> ev_x;
        ev_x.instantiate();
        ev_x->set_device(device_id);
        ev_x->set_axis(godot::JoyAxis::JOY_AXIS_LEFT_X);
        ev_x->set_axis_value(pos_x);
        input->parse_input_event(ev_x);

        godot::Ref<godot::InputEventJoypadMotion> ev_y;
        ev_y.instantiate();
        ev_y->set_device(device_id);
        ev_y->set_axis(godot::JoyAxis::JOY_AXIS_LEFT_Y);
        ev_y->set_axis_value(pos_y);
        input->parse_input_event(ev_y);

        joystick->update_prev_pos(pos_x, pos_y);
    }
} //