#include "gdwiimote.h"
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event_joypad_button.hpp>
#include <godot_cpp/classes/input_event_joypad_motion.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/variant/callable.hpp>

#include <thread>
#include <chrono>

static const int LED_MASKS[4] = {WIIMOTE_LED_1, WIIMOTE_LED_2, WIIMOTE_LED_3, WIIMOTE_LED_4};

GDWiimote::GDWiimote() {}

GDWiimote::GDWiimote(wiimote *wm_ptr, int dev_id)
{
    assign_wiimote(wm_ptr, dev_id);
}

GDWiimote::~GDWiimote()
{
    if (joystick)
    {
        delete joystick;
        joystick = nullptr;
    }
}

void GDWiimote::assign_wiimote(wiimote *wm_ptr, int dev_id)
{
    wm = wm_ptr;
    device_id = dev_id;
    joystick = new NunchukJoystick();
}

void GDWiimote::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("set_leds", "led_indices"), &GDWiimote::set_leds);
    godot::ClassDB::bind_method(godot::D_METHOD("get_led", "led_index"), &GDWiimote::get_led);

    godot::ClassDB::bind_method(godot::D_METHOD("set_rumble", "enabled"), &GDWiimote::set_rumble);
    godot::ClassDB::bind_method(godot::D_METHOD("toggle_rumble"), &GDWiimote::toggle_rumble);
    godot::ClassDB::bind_method(godot::D_METHOD("pulse_rumble", "duration_msec"), &GDWiimote::pulse_rumble);

    godot::ClassDB::bind_method(godot::D_METHOD("set_motion_sensing", "enable"), &GDWiimote::set_motion_sensing);
    godot::ClassDB::bind_method(godot::D_METHOD("set_motion_plus", "enable"), &GDWiimote::set_motion_plus);

    godot::ClassDB::bind_method(godot::D_METHOD("set_orient_threshold", "threshold"), &GDWiimote::set_orient_threshold);
    godot::ClassDB::bind_method(godot::D_METHOD("set_accel_threshold", "threshold"), &GDWiimote::set_accel_threshold);

    godot::ClassDB::bind_method(godot::D_METHOD("get_raw_accel"), &GDWiimote::get_raw_accel);
    godot::ClassDB::bind_method(godot::D_METHOD("get_accel"), &GDWiimote::get_accel);
    godot::ClassDB::bind_method(godot::D_METHOD("get_raw_tilt"), &GDWiimote::get_raw_tilt);
    godot::ClassDB::bind_method(godot::D_METHOD("get_smoothed_tilt"), &GDWiimote::get_smoothed_tilt);
    godot::ClassDB::bind_method(godot::D_METHOD("get_gyro"), &GDWiimote::get_gyro);

    godot::ClassDB::bind_method(godot::D_METHOD("set_nunchuk_deadzone", "dz"), &GDWiimote::set_nunchuk_deadzone);
    godot::ClassDB::bind_method(godot::D_METHOD("set_nunchuk_threshold", "dt"), &GDWiimote::set_nunchuk_threshold);

    godot::ClassDB::bind_method(godot::D_METHOD("set_nunchuk_orient_threshold", "threshold"), &GDWiimote::set_nunchuk_orient_threshold);
    godot::ClassDB::bind_method(godot::D_METHOD("set_nunchuk_accel_threshold", "threshold"), &GDWiimote::set_nunchuk_accel_threshold);

    godot::ClassDB::bind_method(godot::D_METHOD("get_nunchuk_raw_accel"), &GDWiimote::get_nunchuk_raw_accel);
    godot::ClassDB::bind_method(godot::D_METHOD("get_nunchuk_accel"), &GDWiimote::get_nunchuk_accel);
    godot::ClassDB::bind_method(godot::D_METHOD("get_nunchuk_raw_tilt"), &GDWiimote::get_nunchuk_raw_tilt);
    godot::ClassDB::bind_method(godot::D_METHOD("get_nunchuk_smoothed_tilt"), &GDWiimote::get_nunchuk_smoothed_tilt);

    godot::ClassDB::bind_method(godot::D_METHOD("is_nunchuk_connected"), &GDWiimote::is_nunchuk_connected);
    godot::ClassDB::bind_method(godot::D_METHOD("initialize_nunchuk"), &GDWiimote::initialize_nunchuk);

    godot::ClassDB::bind_method(godot::D_METHOD("start_nunchuk_calibration"), &GDWiimote::start_nunchuk_calibration);
    godot::ClassDB::bind_method(godot::D_METHOD("stop_nunchuk_calibration"), &GDWiimote::stop_nunchuk_calibration);

    godot::ClassDB::bind_method(godot::D_METHOD("get_battery_level"), &GDWiimote::get_battery_level);

    ADD_SIGNAL(godot::MethodInfo("nunchuk_inserted",
                                 godot::PropertyInfo(godot::Variant::INT, "device_id")));
    ADD_SIGNAL(godot::MethodInfo("nunchuk_removed",
                                 godot::PropertyInfo(godot::Variant::INT, "device_id")));
    ADD_SIGNAL(godot::MethodInfo("wiimote_disconnected",
                                 godot::PropertyInfo(godot::Variant::INT, "device_id")));
}

// LED & rumble
void GDWiimote::set_leds(const godot::Array &led_indices)
{
    if (!wm)
        return;
    int led_mask = 0;
    for (int i = 0; i < led_indices.size(); i++)
    {
        int led_num = (int)led_indices[i];
        if (led_num >= 1 && led_num <= 4)
            led_mask |= LED_MASKS[led_num - 1];
    }
    wiiuse_set_leds(wm, led_mask);
}

bool GDWiimote::get_led(int led_index) const
{
    if (!wm)
        return false;
    if (led_index < 1 || led_index > 4)
        return false;
    return (wm->leds & LED_MASKS[led_index - 1]) != 0;
}

void GDWiimote::set_rumble(bool enabled)
{
    if (!wm)
        return;
    wiiuse_rumble(wm, enabled ? 1 : 0);
}

void GDWiimote::toggle_rumble()
{
    if (!wm)
        return;
    wiiuse_toggle_rumble(wm);
}

void GDWiimote::pulse_rumble(double duration_msec)
{
    if (!wm)
        return;

    wiiuse_rumble(wm, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(duration_msec)));
    wiiuse_rumble(wm, 0);
}

// Motion thresholds
void GDWiimote::set_motion_sensing(bool enable)
{
    if (wm)
        wiiuse_motion_sensing(wm, enable ? 1 : 0);
}

void GDWiimote::set_motion_plus(bool enable)
{
    if (wm)
    {
        if (enable)
        {
            if ((wm->exp.type == EXP_NUNCHUK) || (wm->exp.type == EXP_MOTION_PLUS_NUNCHUK))
            {
                wiiuse_set_motion_plus(wm, 2); // nunchuck pass-through
            }
            else
            {
                wiiuse_set_motion_plus(wm, 1); // standalone
            }
        }
        else
        {
            wiiuse_set_motion_plus(wm, 0); // disable
        }
    }
}

void GDWiimote::set_orient_threshold(float threshold)
{
    orient_threshold = threshold;
    if (wm)
        wiiuse_set_orient_threshold(wm, threshold);
}

void GDWiimote::set_accel_threshold(int threshold)
{
    accel_threshold = threshold;
    if (wm)
        wiiuse_set_accel_threshold(wm, threshold);
}

// Nunchuk thresholds
void GDWiimote::set_nunchuk_deadzone(float dz) { nunchuk_deadzone = dz; }
void GDWiimote::set_nunchuk_threshold(float dt) { nunchuk_threshold = dt; }
void GDWiimote::set_nunchuk_orient_threshold(float th) { nunchuk_orient_threshold = th; }
void GDWiimote::set_nunchuk_accel_threshold(int th) { nunchuk_accel_threshold = th; }

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

void GDWiimote::start_nunchuk_calibration()
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

void GDWiimote::stop_nunchuk_calibration()
{
    if (is_calibrating_nunchuk)
        is_calibrating_nunchuk = false;
    else
        godot::UtilityFunctions::print("Calibration not in progress!");
}

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
}

// wiimote button relay
void GDWiimote::relay_button(wiimote *wm, int wiibtn, godot::JoyButton godot_btn)
{
    bool pressed = IS_PRESSED(wm, wiibtn);
    godot::Ref<godot::InputEventJoypadButton> ev;
    ev.instantiate();
    ev->set_device(device_id);
    ev->set_button_index(godot_btn);
    ev->set_pressed(pressed);
    godot::Input::get_singleton()->parse_input_event(ev);
}

// nunchuk button relay
void GDWiimote::relay_button(nunchuk_t *nc, int ncbtn, godot::JoyButton godot_btn)
{
    bool pressed = IS_PRESSED(nc, ncbtn);
    godot::Ref<godot::InputEventJoypadButton> ev;
    ev.instantiate();
    ev->set_device(device_id);
    ev->set_button_index(godot_btn);
    ev->set_pressed(pressed);
    godot::Input::get_singleton()->parse_input_event(ev);
}

// Battery
float GDWiimote::get_battery_level() const
{
    if (!wm)
        return -1;
    return wm->battery_level;
}

// Handle wm->event
void GDWiimote::handle_event()
{
    if (!wm)
        return;

    switch (wm->event)
    {
    case WIIUSE_NUNCHUK_INSERTED:
        emit_signal("nunchuk_inserted", device_id);
        godot::UtilityFunctions::print("Nunchuk inserted on Wiimote ", device_id);
        initialize_nunchuk();
        break;

    case WIIUSE_NUNCHUK_REMOVED:
        emit_signal("nunchuk_removed", device_id);
        godot::UtilityFunctions::print("Nunchuk removed on Wiimote ", device_id);
        break;

    case WIIUSE_UNEXPECTED_DISCONNECT:
        emit_signal("wiimote_disconnected", device_id);
        godot::UtilityFunctions::print("Wiimote ", device_id, " disconnected");
        break;

    case WIIUSE_DISCONNECT:
        emit_signal("wiimote_disconnected", device_id);
        godot::UtilityFunctions::print("Wiimote ", device_id, " disconnected");
        break;

    case WIIUSE_EVENT:
        // Relay main Wiimote buttons
        relay_button(wm, WIIMOTE_BUTTON_A, godot::JoyButton::JOY_BUTTON_A);
        relay_button(wm, WIIMOTE_BUTTON_B, godot::JoyButton::JOY_BUTTON_B);
        relay_button(wm, WIIMOTE_BUTTON_PLUS, godot::JoyButton::JOY_BUTTON_START);
        relay_button(wm, WIIMOTE_BUTTON_MINUS, godot::JoyButton::JOY_BUTTON_BACK);
        relay_button(wm, WIIMOTE_BUTTON_HOME, godot::JoyButton::JOY_BUTTON_GUIDE);

        relay_button(wm, WIIMOTE_BUTTON_UP, godot::JoyButton::JOY_BUTTON_DPAD_UP);
        relay_button(wm, WIIMOTE_BUTTON_DOWN, godot::JoyButton::JOY_BUTTON_DPAD_DOWN);
        relay_button(wm, WIIMOTE_BUTTON_LEFT, godot::JoyButton::JOY_BUTTON_DPAD_LEFT);
        relay_button(wm, WIIMOTE_BUTTON_RIGHT, godot::JoyButton::JOY_BUTTON_DPAD_RIGHT);

        if (is_nunchuk_connected())
        {
            if (!is_calibrating_nunchuk)
            {
                poll_nunchuk_joystick();
            }
            else
            {
                float raw_x = wm->exp.nunchuk.js.x;
                float raw_y = wm->exp.nunchuk.js.y;
                joystick->calibrate(raw_x, raw_y);
            }

            relay_button(&wm->exp.nunchuk, NUNCHUK_BUTTON_C, godot::JoyButton::JOY_BUTTON_LEFT_SHOULDER);
            relay_button(&wm->exp.nunchuk, NUNCHUK_BUTTON_Z, godot::JoyButton::JOY_BUTTON_RIGHT_SHOULDER);
        }
        break;

    default:
        break;
    }
}

godot::Vector3 GDWiimote::get_raw_accel() const
{
    if (!wm)
        return godot::Vector3();

    return godot::Vector3(wm->accel.x, wm->accel.y, wm->accel.z);
}

godot::Vector3 GDWiimote::get_accel() const
{
    if (!wm)
        return godot::Vector3();

    return godot::Vector3(wm->gforce.x, wm->gforce.y, wm->gforce.z);
}

godot::Vector3 GDWiimote::get_raw_tilt() const
{
    if (!wm)
        return godot::Vector3();

    return godot::Vector3(wm->orient.yaw, wm->orient.pitch, wm->orient.roll);
}

godot::Vector3 GDWiimote::get_smoothed_tilt() const
{
    if (!wm)
        return godot::Vector3();

    return godot::Vector3(0.f, wm->orient.a_pitch, wm->orient.a_roll);
}

godot::Vector3 GDWiimote::get_nunchuk_raw_accel() const
{
    if (!is_nunchuk_connected())
        return godot::Vector3();

    return godot::Vector3(wm->exp.nunchuk.accel.x, wm->exp.nunchuk.accel.y, wm->exp.nunchuk.accel.z);
}

godot::Vector3 GDWiimote::get_nunchuk_accel() const
{
    if (!wm)
        return godot::Vector3();

    return godot::Vector3(wm->exp.nunchuk.gforce.x, wm->exp.nunchuk.gforce.y, wm->exp.nunchuk.gforce.z);
}

godot::Vector3 GDWiimote::get_nunchuk_raw_tilt() const
{
    if (!is_nunchuk_connected())
        return godot::Vector3();

    return godot::Vector3(wm->exp.nunchuk.orient.yaw, wm->exp.nunchuk.orient.pitch, wm->exp.nunchuk.orient.roll);
}

godot::Vector3 GDWiimote::get_nunchuk_smoothed_tilt() const
{
    if (!is_nunchuk_connected())
        return godot::Vector3();

    return godot::Vector3(0.f, wm->exp.nunchuk.orient.a_pitch, wm->exp.nunchuk.orient.a_roll);
}

godot::Vector3 GDWiimote::get_gyro() const
{
    if (!wm || !(wm->exp.type == EXP_MOTION_PLUS || wm->exp.type == EXP_MOTION_PLUS_NUNCHUK))
        return godot::Vector3();

    return godot::Vector3(wm->exp.mp.angle_rate_gyro.yaw,
                          wm->exp.mp.angle_rate_gyro.pitch,
                          wm->exp.mp.angle_rate_gyro.roll);
}