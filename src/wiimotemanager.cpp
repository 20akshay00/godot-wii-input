#include "wiimotemanager.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>

#include <thread>
#include <chrono>

using namespace godot;

void WiimoteManager::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_max_wiimotes", "max"), &WiimoteManager::set_max_wiimotes);
    ClassDB::bind_method(D_METHOD("get_max_wiimotes"), &WiimoteManager::get_max_wiimotes);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_wiimotes"), "set_max_wiimotes", "get_max_wiimotes");

    ClassDB::bind_method(D_METHOD("connect_wiimotes"), &WiimoteManager::connect_wiimotes);
    ClassDB::bind_method(D_METHOD("finalize_connection"), &WiimoteManager::finalize_connection);
    ClassDB::bind_method(D_METHOD("disconnect_wiimotes"), &WiimoteManager::disconnect_wiimotes);
    ClassDB::bind_method(D_METHOD("get_connected_wiimotes"), &WiimoteManager::get_connected_wiimotes);

    ClassDB::bind_method(D_METHOD("start_polling"), &WiimoteManager::start_polling);
    ClassDB::bind_method(D_METHOD("stop_polling"), &WiimoteManager::stop_polling);
}

WiimoteManager::WiimoteManager()
{
    // Do not auto-connect in editor
    if (Engine::get_singleton()->is_editor_hint())
    {
        set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
    }
    else
    {
        set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
    }
}

WiimoteManager::~WiimoteManager()
{
    disconnect_wiimotes();
}

void WiimoteManager::set_max_wiimotes(int max)
{
    if (num_connected >= 0)
    {
        UtilityFunctions::print("Cannot change max_wiimotes while connected!");
        return;
    }
    max_wiimotes = MAX(1, MIN(max, 16)); // kind of arbitrary limit
}

int WiimoteManager::get_max_wiimotes() const
{
    return max_wiimotes;
}

TypedArray<GDWiimote> WiimoteManager::finalize_connection()
{
    // Wrap each connected wiimote
    gdwiimotes.clear();

    for (int i = 0; i < num_connected; i++)
    {
        if (!wiimotes[i] || !WIIMOTE_IS_CONNECTED(wiimotes[i]))
            continue;

        Ref<GDWiimote> gdwm = memnew(GDWiimote(wiimotes[i], i));
        gdwiimotes.push_back(gdwm);

        // doesn't make sense to use gdwiimote.set_leds here since it requires a GDArray
        wiiuse_set_leds(wiimotes[i], (i == 0) ? WIIMOTE_LED_1 : (i == 1) ? WIIMOTE_LED_2
                                                            : (i == 2)   ? WIIMOTE_LED_3
                                                                         : WIIMOTE_LED_4);
        // briefly rumble to indicate connection
        wiiuse_rumble(wiimotes[i], 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        wiiuse_rumble(wiimotes[i], 0);
    }

    set_process_mode(Node::ProcessMode::PROCESS_MODE_INHERIT); // start polling
    return gdwiimotes;
}

void WiimoteManager::connect_wiimotes()
{
    if (num_connected >= 0)
    {
        UtilityFunctions::print("Cannot connect during runtime!");
        return;
    }

    // Allocate wiimote structures
    wiimotes = wiiuse_init(max_wiimotes);

    int found = wiiuse_find(wiimotes, max_wiimotes, 5);
    if (found <= 0)
    {
        UtilityFunctions::print("No Wiimotes found.");
        wiiuse_cleanup(wiimotes, max_wiimotes);
        wiimotes = nullptr;
        return;
    }

    num_connected = wiiuse_connect(wiimotes, max_wiimotes);
    if (num_connected <= 0)
    {
        UtilityFunctions::print("Found ", found, " but could not connect to any.");
        wiiuse_cleanup(wiimotes, max_wiimotes);
        wiimotes = nullptr;
        return;
    }

    UtilityFunctions::print("Wiimotes found: ", found, ", connected: ", num_connected);

    return;
}

void WiimoteManager::disconnect_wiimotes()
{
    if (num_connected < 0)
        return;

    // Turn off LEDs and rumble before disconnect
    for (int i = 0; i < max_wiimotes; i++)
    {
        if (wiimotes && wiimotes[i])
        {
            wiiuse_set_leds(wiimotes[i], WIIMOTE_LED_1 | WIIMOTE_LED_3);
            wiiuse_rumble(wiimotes[i], 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            wiiuse_rumble(wiimotes[i], 0);
        }
    }

    // wiiuse cleanup to free wiimote structures
    if (wiimotes)
    {
        wiiuse_cleanup(wiimotes, max_wiimotes);
        wiimotes = nullptr;
    }

    gdwiimotes.clear();
    num_connected = -1; // reset connection state
    set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
}

void WiimoteManager::start_polling()
{
    set_process_mode(Node::ProcessMode::PROCESS_MODE_INHERIT);
}

void WiimoteManager::stop_polling()
{
    set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
}

void WiimoteManager::_process(double delta)
{
    if ((num_connected < 0) || !wiimotes)
        return;

    // Poll events
    if (wiiuse_poll(wiimotes, max_wiimotes))
    {
        for (int i = 0; i < gdwiimotes.size(); i++)
        {
            Ref<GDWiimote> gdwm = gdwiimotes[i];
            if (!gdwm.is_valid())
                continue;

            wiimote *wm = gdwm->get_wiimote_ptr();
            if (!wm || !WIIMOTE_IS_CONNECTED(wm))
                continue;

            gdwm->handle_event();
        }
    }
}
