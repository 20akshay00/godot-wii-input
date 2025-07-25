#include "gdwiimote.h"
#include <godot_cpp/classes/input_event_joypad_button.hpp>

#include <thread>
#include <chrono>
#include "debug.h"

static const int LED_MASKS[4] = {WIIMOTE_LED_1, WIIMOTE_LED_2, WIIMOTE_LED_3, WIIMOTE_LED_4};

GDWiimote::GDWiimote() {}

GDWiimote::GDWiimote(wiimote *wm_ptr, int dev_id)
{
    assign_wiimote(wm_ptr, dev_id);
    motion_state.SetCalibrationMode(GamepadMotionHelpers::CalibrationMode::Manual);
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
    // LED control
    godot::ClassDB::bind_method(godot::D_METHOD("get_led", "led_index"), &GDWiimote::get_led);
    godot::ClassDB::bind_method(godot::D_METHOD("set_leds", "led_indices"), &GDWiimote::set_leds);

    // Rumble control
    godot::ClassDB::bind_method(godot::D_METHOD("set_rumble", "enabled"), &GDWiimote::set_rumble);
    godot::ClassDB::bind_method(godot::D_METHOD("toggle_rumble"), &GDWiimote::toggle_rumble);
    godot::ClassDB::bind_method(godot::D_METHOD("pulse_rumble", "duration_msec"), &GDWiimote::pulse_rumble);

    // Wiimote status
    godot::ClassDB::bind_method(godot::D_METHOD("get_battery_level"), &GDWiimote::get_battery_level);

    // Motion controls

    /// Enable/disable motion sensing capabilities
    godot::ClassDB::bind_method(godot::D_METHOD("set_motion_sensing", "enable"), &GDWiimote::set_motion_sensing);
    godot::ClassDB::bind_method(godot::D_METHOD("set_motion_plus", "enable"), &GDWiimote::set_motion_plus);
    godot::ClassDB::bind_method(godot::D_METHOD("set_motion_processing", "enable"), &GDWiimote::set_motion_processing);

    /// Motion thresholds
    godot::ClassDB::bind_method(godot::D_METHOD("set_orient_threshold", "threshold"), &GDWiimote::set_orient_threshold);
    godot::ClassDB::bind_method(godot::D_METHOD("set_accel_threshold", "threshold"), &GDWiimote::set_accel_threshold);

    godot::ClassDB::bind_method(godot::D_METHOD("set_nunchuk_orient_threshold", "threshold"), &GDWiimote::set_nunchuk_orient_threshold);
    godot::ClassDB::bind_method(godot::D_METHOD("set_nunchuk_accel_threshold", "threshold"), &GDWiimote::set_nunchuk_accel_threshold);

    /// Accelerometer
    godot::ClassDB::bind_method(godot::D_METHOD("get_raw_accel"), &GDWiimote::get_raw_accel);
    godot::ClassDB::bind_method(godot::D_METHOD("get_accel"), &GDWiimote::get_accel);
    godot::ClassDB::bind_method(godot::D_METHOD("get_processed_accel"), &GDWiimote::get_processed_accel);

    godot::ClassDB::bind_method(godot::D_METHOD("get_nunchuk_raw_accel"), &GDWiimote::get_nunchuk_raw_accel);
    godot::ClassDB::bind_method(godot::D_METHOD("get_nunchuk_accel"), &GDWiimote::get_nunchuk_accel);

    /// Gyroscope
    godot::ClassDB::bind_method(godot::D_METHOD("start_gyro_calibration"), &GDWiimote::start_gyro_calibration);
    godot::ClassDB::bind_method(godot::D_METHOD("stop_gyro_calibration"), &GDWiimote::stop_gyro_calibration);
    godot::ClassDB::bind_method(godot::D_METHOD("reset_gyro_calibration"), &GDWiimote::reset_gyro_calibration);
    godot::ClassDB::bind_method(godot::D_METHOD("set_gyro_calibration_mode", "mode"), &GDWiimote::set_gyro_calibration_mode);

    godot::ClassDB::bind_method(godot::D_METHOD("get_raw_gyro"), &GDWiimote::get_raw_gyro);
    godot::ClassDB::bind_method(godot::D_METHOD("get_world_space_gyro", "side_reduction_threshold"), &GDWiimote::get_world_space_gyro);
    godot::ClassDB::bind_method(godot::D_METHOD("get_player_space_gyro", "yaw_relax_factor"), &GDWiimote::get_player_space_gyro);

    /// Orientation
    godot::ClassDB::bind_method(godot::D_METHOD("get_fusion_orientation"), &GDWiimote::get_fusion_orientation);
    godot::ClassDB::bind_method(godot::D_METHOD("get_raw_orientation"), &GDWiimote::get_raw_orientation);
    godot::ClassDB::bind_method(godot::D_METHOD("get_smoothed_orientation"), &GDWiimote::get_smoothed_orientation);
    godot::ClassDB::bind_method(godot::D_METHOD("get_nunchuk_raw_orientation"), &GDWiimote::get_nunchuk_raw_orientation);
    godot::ClassDB::bind_method(godot::D_METHOD("get_nunchuk_smoothed_orientation"), &GDWiimote::get_nunchuk_smoothed_orientation);

    // Nunchuk settings

    /// Nunchuk status
    godot::ClassDB::bind_method(godot::D_METHOD("is_nunchuk_connected"), &GDWiimote::is_nunchuk_connected);
    godot::ClassDB::bind_method(godot::D_METHOD("initialize_nunchuk"), &GDWiimote::initialize_nunchuk);

    godot::ClassDB::bind_method(godot::D_METHOD("start_nunchuk_joystick_calibration"), &GDWiimote::start_nunchuk_joystick_calibration);
    godot::ClassDB::bind_method(godot::D_METHOD("stop_nunchuk_joystick_calibration"), &GDWiimote::stop_nunchuk_joystick_calibration);

    /// Nunchuk thresholds
    godot::ClassDB::bind_method(godot::D_METHOD("set_nunchuk_joystick_deadzone", "dz"), &GDWiimote::set_nunchuk_joystick_deadzone);
    godot::ClassDB::bind_method(godot::D_METHOD("set_nunchuk_joystick_threshold", "dt"), &GDWiimote::set_nunchuk_joystick_threshold);

    ADD_SIGNAL(godot::MethodInfo("nunchuk_inserted",
                                 godot::PropertyInfo(godot::Variant::INT, "device_id")));
    ADD_SIGNAL(godot::MethodInfo("nunchuk_removed",
                                 godot::PropertyInfo(godot::Variant::INT, "device_id")));
    ADD_SIGNAL(godot::MethodInfo("wiimote_disconnected",
                                 godot::PropertyInfo(godot::Variant::INT, "device_id")));
}

// LED control

bool GDWiimote::get_led(int led_index) const
{
    if (!wm)
        return false;
    if (led_index < 1 || led_index > 4)
        return false;
    return (wm->leds & LED_MASKS[led_index - 1]) != 0;
}

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

// Rumble control
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

// Battery
float GDWiimote::get_battery_level() const
{
    if (!wm)
        return -1;
    return wm->battery_level;
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

// Handle wm->event
void GDWiimote::handle_event(double delta)
{
    if (!wm)
        return;

    // update GamePadMotion state if motion plus is enabled
    if (process_motion)
    {
        // uses a Y-up, right-handed coordinate system
        motion_state.ProcessMotion(
            wm->exp.mp.angle_rate_gyro.roll,
            wm->exp.mp.angle_rate_gyro.yaw,
            wm->exp.mp.angle_rate_gyro.pitch,
            wm->gforce.y,
            wm->gforce.z,
            wm->gforce.x,
            delta);
    }

    switch (wm->event)
    {
    case WIIUSE_NUNCHUK_INSERTED:
        emit_signal("nunchuk_inserted", device_id);
        DEBUG_PRINT("Nunchuk inserted on Wiimote ", device_id);
        initialize_nunchuk();
        break;

    case WIIUSE_NUNCHUK_REMOVED:
        emit_signal("nunchuk_removed", device_id);
        DEBUG_PRINT("Nunchuk removed on Wiimote ", device_id);
        break;

    case WIIUSE_UNEXPECTED_DISCONNECT:
        emit_signal("wiimote_disconnected", device_id);
        DEBUG_PRINT("Wiimote ", device_id, " disconnected");
        break;

    case WIIUSE_DISCONNECT:
        emit_signal("wiimote_disconnected", device_id);
        DEBUG_PRINT("Wiimote ", device_id, " disconnected");
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