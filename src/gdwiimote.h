#pragma once
#include <wiiuse.h>
#include "gamepadmotion.h"

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "nunchuk_joystick.h"

class GDWiimote : public godot::RefCounted
{
    GDCLASS(GDWiimote, godot::RefCounted);

private:
    wiimote *wm = nullptr;
    int device_id = -1;

    NunchukJoystick *joystick = nullptr;
    GamepadMotion motion_state;

    // thresholds
    float orient_threshold = 0.0f;
    int accel_threshold = 0;

    float nunchuk_deadzone = 0.1f;
    float nunchuk_threshold = 0.05f;
    float nunchuk_orient_threshold = 0.1f;
    int nunchuk_accel_threshold = 5;

    bool is_calibrating_nunchuk = false;
    bool process_motion = false;

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
    void handle_event(double delta);

    // Button relays (Wiimote + Nunchuk)
    void relay_button(wiimote *wm, int wiibtn, godot::JoyButton godot_btn);
    void relay_button(nunchuk_t *nc, int ncbtn, godot::JoyButton godot_btn);

    // Nunchuk
    bool is_nunchuk_connected() const;
    void initialize_nunchuk();

    // Joystick
    void poll_nunchuk_joystick();
    void start_nunchuk_joystick_calibration();
    void stop_nunchuk_joystick_calibration();

    // LEDs and rumble
    void set_leds(const godot::Array &led_indices);
    bool get_led(int led_index) const;
    void set_rumble(bool enabled);
    void toggle_rumble();
    void pulse_rumble(double duration_msec);

    // Battery
    float get_battery_level() const;

    // Motion sensing
    void set_motion_sensing(bool enable);
    void set_motion_plus(bool enable);
    void set_motion_processing(bool enable);

    // Thresholds
    void set_orient_threshold(float threshold);
    void set_accel_threshold(int threshold);

    void set_nunchuk_joystick_deadzone(float dz);
    void set_nunchuk_joystick_threshold(float dt);
    void set_nunchuk_orient_threshold(float threshold);
    void set_nunchuk_accel_threshold(int threshold);

    // Accelerometer
    godot::Vector3 get_raw_accel() const; // wiiuse
    godot::Vector3 get_accel() const;     // wiiuse
    godot::Vector3 get_processed_accel(); // gamepadmotion

    godot::Vector3 get_nunchuk_raw_accel() const; // wiiuse
    godot::Vector3 get_nunchuk_accel() const;     // wiiuse

    // Gyroscope
    void start_gyro_calibration();
    void stop_gyro_calibration();
    void reset_gyro_calibration();
    void set_gyro_calibration_mode(int mode);

    godot::Vector3 get_raw_gyro() const; // wiiuse
    godot::Vector2 get_world_space_gyro(float sideReductionThreshold);
    godot::Vector2 get_player_space_gyro(float yawRelaxFactor);

    // Orientation
    godot::Quaternion get_fusion_orientation();
    godot::Vector3 get_raw_orientation() const;      // wiiuse
    godot::Vector3 get_smoothed_orientation() const; // wiiuse

    godot::Vector3 get_nunchuk_raw_orientation() const;      // wiiuse
    godot::Vector3 get_nunchuk_smoothed_orientation() const; // wiiuse

    // Wii board
    godot::Array get_board_raw_data() const;
    godot::Array get_board_interpolated_weights() const;

    // IR
    bool is_ir_active();
    void set_ir(bool enable);
    float get_ir_cursor_distance();
    godot::Vector2 get_ir_cursor_calculated_position();
    godot::Vector2 get_ir_cursor_absolute_coordinate();
    int get_ir_num_dots();

    godot::Vector2 get_ir_dot_raw_position(int index);
    godot::Vector2 get_ir_dot_interpolated_position(int index);
    float get_ir_dot_size(int index);
    bool is_ir_dot_visible(int index);
};
