#include "wiimotemanager.h"
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

#include <thread>
#include <chrono>

#include "debug.h"

using namespace godot;

void notification()
{
}

void WiimoteManager::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_max_wiimotes", "max"), &WiimoteManager::set_max_wiimotes);
    ClassDB::bind_method(D_METHOD("get_max_wiimotes"), &WiimoteManager::get_max_wiimotes);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_wiimotes"), "set_max_wiimotes", "get_max_wiimotes");

    ClassDB::bind_method(D_METHOD("connect_wiimotes"), &WiimoteManager::connect_wiimotes);
    ClassDB::bind_method(D_METHOD("finalize_connection"), &WiimoteManager::finalize_connection);
    ClassDB::bind_method(D_METHOD("disconnect_wiimotes"), &WiimoteManager::disconnect_wiimotes);
    ClassDB::bind_method(D_METHOD("get_connected_wiimotes"), &WiimoteManager::get_connected_wiimotes);

    ClassDB::bind_method(D_METHOD("start_polling"), &WiimoteManager::enable_polling);
    ClassDB::bind_method(D_METHOD("stop_polling"), &WiimoteManager::disable_polling);
    ClassDB::bind_method(D_METHOD("poll"), &WiimoteManager::poll);
}

WiimoteManager *WiimoteManager::singleton = nullptr;

WiimoteManager::WiimoteManager()
{
    ERR_FAIL_COND_MSG(singleton != nullptr, "WiimoteManager singleton already exists!");
    singleton = this;
}

WiimoteManager::~WiimoteManager()
{
    disconnect_wiimotes();
    singleton = nullptr;
}

void WiimoteManager::set_max_wiimotes(int max)
{
    if (num_connected >= 0)
    {
        UtilityFunctions::print("Cannot change max_wiimotes while connected!");
        return;
    }
    max_wiimotes = MAX(1, MIN(max, 16)); // kind of an arbitrary limit
}

int WiimoteManager::get_max_wiimotes() const
{
    return max_wiimotes;
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

    enable_polling();

    return gdwiimotes;
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
    disable_polling();
}

void WiimoteManager::enable_polling()
{
    SceneTree *tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
    if (tree)
    {
        Callable cb = callable_mp(this, &WiimoteManager::poll);

        if (tree->is_connected("process_frame", cb))
        {
            godot::UtilityFunctions::print("Polling is already enabled!");
        }
        else
        {
            tree->connect("process_frame", cb);
        }
    }
}

void WiimoteManager::disable_polling()
{
    SceneTree *tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
    if (tree)
    {
        Callable cb = callable_mp(this, &WiimoteManager::poll);

        if (tree->is_connected("process_frame", cb))
        {
            tree->disconnect("process_frame", cb);
        }
        else
        {
            godot::UtilityFunctions::print("Polling is already disabled!");
        }
    }
}

void WiimoteManager::poll()
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
