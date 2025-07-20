#ifndef WIIMOTE_MANAGER_H
#define WIIMOTE_MANAGER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <joystick.h>

extern "C"
{
#include <wiiuse.h>
}

using namespace godot;

class WiimoteManager : public Node
{
    GDCLASS(WiimoteManager, Node);

private:
    static constexpr int MAX_WIIMOTES = 4;
    wiimote **wiimotes = nullptr;
    bool connected = false;

    GDJoystick **joysticks = nullptr;
    bool is_calibrating_nunchuk = false;

protected:
    static void _bind_methods();

public:
    WiimoteManager();
    ~WiimoteManager();

    void connect_wiimotes();
    void _process(double delta) override;

private:
    void relay_button(wiimote *wm, int wiibtn, int device_id, JoyButton godot_btn);
    void relay_button(nunchuk_t *nc, int ncbtn, int device_id, JoyButton godot_btn);
    void emit_joypad_button(int device_id, JoyButton godot_btn, bool pressed);
    void poll_nunchuk_joystick(nunchuk_t *nc, int wiimote_index);
    void handle_event(wiimote *wm, int wiimote_index);

    void start_nunchuk_calibration();
    void stop_nunchuk_calibration();
};

#endif
