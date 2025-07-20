#include "wiimotemanager.h"
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event_joypad_button.hpp>
#include <godot_cpp/classes/input_event_joypad_motion.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/callable.hpp>

const int WiimoteManager::led_masks[4] = {WIIMOTE_LED_1, WIIMOTE_LED_2, WIIMOTE_LED_3, WIIMOTE_LED_4};

WiimoteManager::WiimoteManager()
{
    if (!Engine::get_singleton()->is_editor_hint() && wiimotes == nullptr)
    {
        wiimotes = wiiuse_init(MAX_WIIMOTES);
        connect_wiimotes();
        set_process(true);
        joysticks = new GDJoystick *[MAX_WIIMOTES];
        for (int i = 0; i < MAX_WIIMOTES; i++)
        {
            joysticks[i] = new GDJoystick();
        }
    }
    else
    {
        wiimotes = nullptr;
        joysticks = nullptr;
    }
}

WiimoteManager::~WiimoteManager()
{
    if (wiimotes)
    {
        for (int i = 0; i < MAX_WIIMOTES; i++)
        {
            wiiuse_set_leds(wiimotes[i], WIIMOTE_LED_2 | WIIMOTE_LED_4);
            wiiuse_rumble(wiimotes[i], 0);
        }

        wiiuse_cleanup(wiimotes, MAX_WIIMOTES);
        wiimotes = nullptr;
    }

    if (joysticks)
    {
        for (int i = 0; i < MAX_WIIMOTES; i++)
        {
            delete joysticks[i];
        }
        delete[] joysticks;
        joysticks = nullptr;
    }
}

void WiimoteManager::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("connect_wiimotes"), &WiimoteManager::connect_wiimotes);

    ClassDB::bind_method(D_METHOD("set_orient_threshold", "wiimote_index", "threshold"), &WiimoteManager::set_orient_threshold);
    ClassDB::bind_method(D_METHOD("set_accel_threshold", "wiimote_index", "threshold"), &WiimoteManager::set_accel_threshold);

    ClassDB::bind_method(D_METHOD("start_nunchuk_calibration"), &WiimoteManager::start_nunchuk_calibration);
    ClassDB::bind_method(D_METHOD("stop_nunchuk_calibration"), &WiimoteManager::stop_nunchuk_calibration);

    ClassDB::bind_method(D_METHOD("set_leds", "wiimote_index", "led_indices"), &WiimoteManager::set_leds);
    ClassDB::bind_method(D_METHOD("get_led", "wiimote_index", "led"), &WiimoteManager::get_led);

    ClassDB::bind_method(D_METHOD("set_rumble", "wiimote_index", "enable"), &WiimoteManager::set_rumble);
    ClassDB::bind_method(D_METHOD("pulse_rumble", "wiimote_index", "duration"), &WiimoteManager::pulse_rumble);
    ClassDB::bind_method(D_METHOD("toggle_rumble", "wiimote_index"), &WiimoteManager::toggle_rumble);

    ClassDB::bind_method(D_METHOD("initialize_nunchuk", "wiimote_index"), &WiimoteManager::initialize_nunchuk);
    ClassDB::bind_method(D_METHOD("set_nunchuk_deadzone", "dz"), &WiimoteManager::set_nunchuk_deadzone);
    ClassDB::bind_method(D_METHOD("set_nunchuk_threshold", "dt"), &WiimoteManager::set_nunchuk_threshold);
    ClassDB::bind_method(D_METHOD("set_nunchuk_orient_threshold", "wiimote_index", "threshold"), &WiimoteManager::set_nunchuk_orient_threshold);
    ClassDB::bind_method(D_METHOD("set_nunchuk_accel_threshold", "wiimote_index", "threshold"), &WiimoteManager::set_nunchuk_accel_threshold);
    ClassDB::bind_method(D_METHOD("nunchuk_connected", "wiimote_index"), &WiimoteManager::nunchuk_connected);

    ClassDB::bind_method(D_METHOD("set_motion_sensing", "wiimote_index", "enable"), &WiimoteManager::set_motion_sensing);
    ClassDB::bind_method(D_METHOD("get_battery_level", "wiimote_index"), &WiimoteManager::get_battery_level);

    ADD_SIGNAL(godot::MethodInfo("nunchuk_inserted",
                                 godot::PropertyInfo(godot::Variant::INT, "device_id")));
    ADD_SIGNAL(godot::MethodInfo("nunchuk_removed",
                                 godot::PropertyInfo(godot::Variant::INT, "device_id")));
    ADD_SIGNAL(godot::MethodInfo("wiimote_disconnected",
                                 godot::PropertyInfo(godot::Variant::INT, "device_id")));
}

void WiimoteManager::connect_wiimotes()
{
    int found = wiiuse_find(wiimotes, MAX_WIIMOTES, 5);
    if (found > 0)
    {
        int connected_count = wiiuse_connect(wiimotes, MAX_WIIMOTES);
        UtilityFunctions::print("Wiimotes found: ", found, ", connected: ", connected_count);
        connected = (connected_count > 0);

        wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1);
        wiiuse_set_leds(wiimotes[1], WIIMOTE_LED_2);
        wiiuse_set_leds(wiimotes[2], WIIMOTE_LED_3);
        wiiuse_set_leds(wiimotes[3], WIIMOTE_LED_4);

        // Initialize rumble timers for each Wiimote
        for (int i = 0; i < MAX_WIIMOTES; i++)
        {
            rumble_timers[i] = memnew(godot::Timer);
            rumble_timers[i]->set_one_shot(true);
            rumble_timers[i]->set_autostart(false);
            add_child(rumble_timers[i]);
            rumble_timers[i]->connect("timeout", callable_mp(this, &WiimoteManager::set_rumble).bind(i, false));
        }
    }
    else
    {
        UtilityFunctions::print("No Wiimotes found during connect attempt");
        connected = false;
    }
}

void WiimoteManager::_process(double delta)
{
    if (!wiimotes)
        return;

    // Poll all wiimotes
    if (wiiuse_poll(wiimotes, MAX_WIIMOTES))
    {
        for (int wiimote_index = 0; wiimote_index < MAX_WIIMOTES; wiimote_index++)
        {
            wiimote *wm = wiimotes[wiimote_index];

            if (!wm || !WIIMOTE_IS_CONNECTED(wm))
                continue;

            wiiuse_set_flags(wm, WIIUSE_CONTINUOUS, 1);
            wiiuse_status(wm); // force refresh expansion state

            switch (wm->event)
            {
            case WIIUSE_NUNCHUK_INSERTED:
                // does not seem to fire reliably!
                UtilityFunctions::print("Nunchuk inserted on Wiimote ", wiimote_index);
                emit_signal("nunchuk_inserted", wiimote_index);
                initialize_nunchuk(wiimote_index);
                break;

            case WIIUSE_NUNCHUK_REMOVED:
                UtilityFunctions::print("Nunchuk removed on Wiimote ", wiimote_index);
                emit_signal("nunchuk_removed", wiimote_index);
                break;

            case WIIUSE_UNEXPECTED_DISCONNECT:
                UtilityFunctions::print("Wiimote ", wiimote_index, " disconnected unexpectedly");
                emit_signal("wiimote_disconnected", wiimote_index);
                break;

            case WIIUSE_DISCONNECT:
                UtilityFunctions::print("Wiimote ", wiimote_index, " disconnected");
                emit_signal("wiimote_disconnected", wiimote_index);
                break;

            case WIIUSE_EVENT:
                // Handle button events
                handle_event(wiimote_index);
                break;

            default:
                // Handle other events if necessary
                break;
            }
        }
    }
}

void WiimoteManager::relay_button(wiimote *wm, int wiibtn, int device_id, godot::JoyButton godot_btn)
{
    bool pressed = IS_PRESSED(wm, wiibtn);
    emit_joypad_button(device_id, godot_btn, pressed);
}

void WiimoteManager::relay_button(nunchuk_t *nc, int ncbtn, int device_id, godot::JoyButton godot_btn)
{
    bool pressed = IS_PRESSED(nc, ncbtn);
    emit_joypad_button(device_id, godot_btn, pressed);
}

void WiimoteManager::emit_joypad_button(int device_id, godot::JoyButton godot_btn, bool pressed)
{
    Ref<InputEventJoypadButton> ev;
    ev.instantiate();
    ev->set_device(device_id);
    ev->set_button_index(godot_btn);
    ev->set_pressed(pressed);
    Input::get_singleton()->parse_input_event(ev);
}

// Poll and emit nunchuk joystick axes
void WiimoteManager::poll_nunchuk_joystick(nunchuk_t *nc, int wiimote_index)
{
    float pos_x = nc->js.x;
    float pos_y = nc->js.y;
    GDJoystick *joystick = joysticks[wiimote_index];

    if (joystick->is_motion_detected(pos_x, pos_y))
    {
        // Normalize and emit X axis motion
        pos_x = joystick->normalize_x(pos_x);
        Ref<InputEventJoypadMotion> ev_x;
        ev_x.instantiate();
        ev_x->set_device(wiimote_index);
        ev_x->set_axis(godot::JoyAxis::JOY_AXIS_LEFT_X);
        ev_x->set_axis_value(pos_x);
        godot::Input::get_singleton()->parse_input_event(ev_x);

        // Normalize and emit Y axis motion
        pos_y = joystick->normalize_y(pos_y);
        Ref<InputEventJoypadMotion> ev_y;
        ev_y.instantiate();
        ev_y->set_device(wiimote_index);
        ev_y->set_axis(godot::JoyAxis::JOY_AXIS_LEFT_Y);
        ev_y->set_axis_value(pos_y);
        godot::Input::get_singleton()->parse_input_event(ev_y);

        // Update joystick position only if significant motion is detected
        joystick->update_prev_pos(pos_x, pos_y);
    }
}

bool WiimoteManager::nunchuk_connected(int wiimote_index) const
{
    if (wiimotes && wiimote_index >= 0 && wiimote_index < MAX_WIIMOTES)
    {
        return (wiimotes[wiimote_index]->exp.type == EXP_NUNCHUK);
    }
    return false;
}

void WiimoteManager::handle_event(int wiimote_index)
{
    wiimote *wm = wiimotes[wiimote_index];

    // --- Relay main Wiimote buttons ---
    relay_button(wm, WIIMOTE_BUTTON_A, wiimote_index, godot::JoyButton::JOY_BUTTON_A);
    relay_button(wm, WIIMOTE_BUTTON_B, wiimote_index, godot::JoyButton::JOY_BUTTON_B);
    relay_button(wm, WIIMOTE_BUTTON_PLUS, wiimote_index, godot::JoyButton::JOY_BUTTON_START);
    relay_button(wm, WIIMOTE_BUTTON_MINUS, wiimote_index, godot::JoyButton::JOY_BUTTON_BACK);
    relay_button(wm, WIIMOTE_BUTTON_HOME, wiimote_index, godot::JoyButton::JOY_BUTTON_GUIDE);

    // --- Relay D-Pad as native joystick D-Pad buttons ---
    relay_button(wm, WIIMOTE_BUTTON_UP, wiimote_index, godot::JoyButton::JOY_BUTTON_DPAD_UP);
    relay_button(wm, WIIMOTE_BUTTON_DOWN, wiimote_index, godot::JoyButton::JOY_BUTTON_DPAD_DOWN);
    relay_button(wm, WIIMOTE_BUTTON_LEFT, wiimote_index, godot::JoyButton::JOY_BUTTON_DPAD_LEFT);
    relay_button(wm, WIIMOTE_BUTTON_RIGHT, wiimote_index, godot::JoyButton::JOY_BUTTON_DPAD_RIGHT);

    if (wm->exp.type == EXP_NUNCHUK)
    {
        if (!is_calibrating_nunchuk)
        {
            poll_nunchuk_joystick(&wm->exp.nunchuk, wiimote_index);
            // UtilityFunctions::print(joysticks[wiimote_index]->normalize_x(wm->exp.nunchuk.js.x), " ", joysticks[wiimote_index]->normalize_y(wm->exp.nunchuk.js.y), " ", joysticks[wiimote_index]->min_x, " ", joysticks[wiimote_index]->min_y,
            //                         " ", joysticks[wiimote_index]->max_x, " ", joysticks[wiimote_index]->max_y);
        }
        else
        {
            float raw_x = wm->exp.nunchuk.js.x;
            float raw_y = wm->exp.nunchuk.js.y;
            joysticks[wiimote_index]->calibrate(raw_x, raw_y);

            // UtilityFunctions::print("Nunchuk joystick calibration (raw):", joysticks[wiimote_index]->center_x, " ", joysticks[wiimote_index]->center_y,
            //                         " Min:", joysticks[wiimote_index]->min_x, " ", joysticks[wiimote_index]->min_y,
            //                         " Max:", joysticks[wiimote_index]->max_x, " ", joysticks[wiimote_index]->max_y);
        }

        relay_button(&wm->exp.nunchuk, NUNCHUK_BUTTON_C, wiimote_index, godot::JoyButton::JOY_BUTTON_LEFT_SHOULDER);
        relay_button(&wm->exp.nunchuk, NUNCHUK_BUTTON_Z, wiimote_index, godot::JoyButton::JOY_BUTTON_RIGHT_SHOULDER);
    }
}

void WiimoteManager::start_nunchuk_calibration()
{
    if (!is_calibrating_nunchuk)
    {
        is_calibrating_nunchuk = true;
        for (int wiimote_index = 0; wiimote_index < MAX_WIIMOTES; wiimote_index++)
        {
            joysticks[wiimote_index]->initialize_calibration(wiimotes[wiimote_index]->exp.nunchuk.js.x, wiimotes[wiimote_index]->exp.nunchuk.js.y);
        }
    }
    else
    {
        UtilityFunctions::print("Nunchuk calibration already in progress!");
    }
}

void WiimoteManager::stop_nunchuk_calibration()
{
    if (is_calibrating_nunchuk)
    {
        is_calibrating_nunchuk = false;
    }
    else
    {
        UtilityFunctions::print("Nunchuk calibration is not in progress!");
    }
}

void WiimoteManager::set_nunchuk_deadzone(float dz)
{
    nunchuk_deadzone = dz;
}

void WiimoteManager::set_nunchuk_threshold(float dt)
{
    nunchuk_threshold = dt;
}

void WiimoteManager::set_leds(int wiimote_index, const godot::Array &led_indices)
{
    if (wiimote_index_valid(wiimote_index))
        return;

    int led_mask = 0;
    for (int i = 0; i < led_indices.size(); i++)
    {
        int led_num = (int)led_indices[i];
        switch (led_num)
        {
        case 1:
            led_mask |= WIIMOTE_LED_1;
            break;
        case 2:
            led_mask |= WIIMOTE_LED_2;
            break;
        case 3:
            led_mask |= WIIMOTE_LED_3;
            break;
        case 4:
            led_mask |= WIIMOTE_LED_4;
            break;
        }
    }

    wiiuse_set_leds(wiimotes[wiimote_index], led_mask);
}

// set rumble state for a specific Wiimote
void WiimoteManager::set_rumble(int wiimote_index, bool enabled)
{
    if (wiimote_index_valid(wiimote_index))
        return;

    wiiuse_rumble(wiimotes[wiimote_index], enabled ? 1 : 0);
}

// Pulse rumble using preallocated timer
void WiimoteManager::pulse_rumble(int wiimote_index, double duration_sec)
{
    if (wiimote_index_valid(wiimote_index))
        return;

    wiiuse_rumble(wiimotes[wiimote_index], 1);
    rumble_timers[wiimote_index]->start(duration_sec);
    return;
}

void WiimoteManager::toggle_rumble(int wiimote_index)
{
    wiiuse_toggle_rumble(wiimotes[wiimote_index]);
}

void WiimoteManager::set_motion_sensing(int wiimote_index, bool enable)
{
    if (wiimote_index_valid(wiimote_index))
        return;

    wiiuse_motion_sensing(wiimotes[wiimote_index], enable ? 1 : 0);
}

bool WiimoteManager::get_led(int wiimote_index, int led) const
{
    if (wiimote_index_valid(wiimote_index))
        return false;

    if (led < 1 || led > 4)
    {
        UtilityFunctions::print("Invalid LED index: ", led);
        return false;
    }

    int led_mask = led_masks[led - 1];
    return (wiimotes[wiimote_index]->leds & led_mask) == led_mask;
}

float WiimoteManager::get_battery_level(int wiimote_index) const
{
    if (wiimote_index_valid(wiimote_index))
        return -1;

    return wiimotes[wiimote_index]->battery_level;
}

void WiimoteManager::set_orient_threshold(int wiimote_index, float threshold)
{
    if (wiimote_index_valid(wiimote_index))
        return;

    wiiuse_set_orient_threshold(wiimotes[wiimote_index], threshold);
}

void WiimoteManager::set_accel_threshold(int wiimote_index, int threshold)
{
    if (wiimote_index_valid(wiimote_index))
        return;

    wiiuse_set_accel_threshold(wiimotes[wiimote_index], threshold);
}

void WiimoteManager::set_nunchuk_orient_threshold(int wiimote_index, float threshold)
{
    if (wiimote_index_valid(wiimote_index))
        return;

    nunchuk_orient_threshold = threshold;
}

void WiimoteManager::set_nunchuk_accel_threshold(int wiimote_index, int threshold)
{
    if (wiimote_index_valid(wiimote_index))
        return;
    nunchuk_accel_threshold = threshold;
}

bool WiimoteManager::wiimote_index_valid(int wiimote_index) const
{
    if (wiimotes == nullptr || wiimote_index < 0 || wiimote_index >= MAX_WIIMOTES)
    {
        UtilityFunctions::print("Invalid Wiimote index: ", wiimote_index);
        return false;
    }
    return true;
}

void WiimoteManager::initialize_nunchuk(int wiimote_index)
{
    wiimote *wm = wiimotes[wiimote_index];

    if (wm->exp.type != EXP_NUNCHUK)
    {
        UtilityFunctions::print("Nunchuk not connected to Wiimote ", wiimote_index);
        return;
    }

    joysticks[wiimote_index]->initialize_joystick(&wm->exp.nunchuk, nunchuk_deadzone, nunchuk_threshold);
    // Set the nunchuk orientation threshold
    wiiuse_set_nunchuk_orient_threshold(wm, nunchuk_orient_threshold);
    // Set the nunchuk acceleration threshold
    wiiuse_set_nunchuk_accel_threshold(wm, nunchuk_accel_threshold);
}