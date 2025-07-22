#pragma once
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <wiiuse.h>

#include "nunchuk_joystick.h"

class GDWiimote : public godot::RefCounted
{
    GDCLASS(GDWiimote, godot::RefCounted);

private:
    wiimote *wm = nullptr;
    int device_id = -1;

    NunchukJoystick *joystick = nullptr;

    // thresholds
    float orient_threshold = 0.0f;
    int accel_threshold = 0;

    float nunchuk_deadzone = 0.1f;
    float nunchuk_threshold = 0.05f;
    float nunchuk_orient_threshold = 0.1f;
    int nunchuk_accel_threshold = 5;

    bool is_calibrating_nunchuk = false;

protected:
    static void _bind_methods();

public:
    GDWiimote();
    GDWiimote(wiimote *wm_ptr, int dev_id);
    ~GDWiimote();

    void assign_wiimote(wiimote *wm_ptr, int dev_id);

    int get_device_id() const { return device_id; }
    wiimote *get_wiimote_ptr() const { return wm; }

    // Core per-device event processing
    void handle_event();

    // Button relays (Wiimote + Nunchuk)
    void relay_button(wiimote *wm, int wiibtn, godot::JoyButton godot_btn);
    void relay_button(nunchuk_t *nc, int ncbtn, godot::JoyButton godot_btn);

    // Joystick
    void poll_nunchuk_joystick();
    void start_nunchuk_calibration();
    void stop_nunchuk_calibration();

    // LEDs + rumble
    void set_leds(const godot::Array &led_indices);
    bool get_led(int led_index) const;
    void set_rumble(bool enabled);
    void toggle_rumble();
    void pulse_rumble(double duration_msec);

    // Motion + thresholds
    void set_motion_sensing(bool enable);
    void set_motion_plus(bool enable);
    void set_orient_threshold(float threshold);
    void set_accel_threshold(int threshold);

    void set_nunchuk_deadzone(float dz);
    void set_nunchuk_threshold(float dt);
    void set_nunchuk_orient_threshold(float threshold);
    void set_nunchuk_accel_threshold(int threshold);

    bool is_nunchuk_connected() const;
    void initialize_nunchuk();

    // Accelerometer
    godot::Vector3 get_accel() const;
    godot::Vector3 get_gforce() const;
    godot::Vector2 get_raw_tilt() const;
    godot::Vector2 get_smoothed_tilt() const;

    godot::Vector3 get_nunchuk_accel() const;
    godot::Vector3 get_nunchuk_gforce() const;
    godot::Vector2 get_nunchuk_raw_tilt() const;
    godot::Vector2 get_nunchuk_smoothed_tilt() const;

    godot::Vector3 get_gyro() const;

    // Battery
    float get_battery_level() const;
};
