#include "gdwiimoteserver.h"
#include "wiipair.h"
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

#include <thread>
#include <chrono>

#include "debug.h"

void GDWiimoteServer::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("set_max_wiimotes", "max"), &GDWiimoteServer::set_max_wiimotes);
    godot::ClassDB::bind_method(godot::D_METHOD("get_max_wiimotes"), &GDWiimoteServer::get_max_wiimotes);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::INT, "max_wiimotes"), "set_max_wiimotes", "get_max_wiimotes");

    godot::ClassDB::bind_method(godot::D_METHOD("initialize_connection"), &GDWiimoteServer::initialize_connection);
    godot::ClassDB::bind_method(godot::D_METHOD("finalize_connection"), &GDWiimoteServer::finalize_connection);
    godot::ClassDB::bind_method(godot::D_METHOD("disconnect_wiimotes"), &GDWiimoteServer::disconnect_wiimotes);
    godot::ClassDB::bind_method(godot::D_METHOD("get_connected_wiimotes"), &GDWiimoteServer::get_connected_wiimotes);

    godot::ClassDB::bind_method(godot::D_METHOD("start_polling"), &GDWiimoteServer::enable_polling);
    godot::ClassDB::bind_method(godot::D_METHOD("stop_polling"), &GDWiimoteServer::disable_polling);
    godot::ClassDB::bind_method(godot::D_METHOD("poll"), &GDWiimoteServer::poll);
}

GDWiimoteServer *GDWiimoteServer::singleton = nullptr;

GDWiimoteServer::GDWiimoteServer()
{
    ERR_FAIL_COND_MSG(singleton != nullptr, "GDWiimoteServer singleton already exists!");
    singleton = this;
}

GDWiimoteServer::~GDWiimoteServer()
{
    disconnect_wiimotes();
    singleton = nullptr;
}

void GDWiimoteServer::set_max_wiimotes(int max)
{
    if (num_connected >= 0)
    {
        godot::UtilityFunctions::print("Cannot change max_wiimotes while connected!");
        return;
    }
    max_wiimotes = godot::MAX(1, godot::MIN(max, 16)); // kind of an arbitrary limit
}

int GDWiimoteServer::get_max_wiimotes() const
{
    return max_wiimotes;
}

void GDWiimoteServer::initialize_connection(bool pair)
{
    if (num_connected >= 0)
    {
        godot::UtilityFunctions::print("Cannot connect during runtime!");
        return;
    }

    // Allocate wiimote structures
    wiimotes = wiiuse_init(max_wiimotes);
    int found = wiiuse_find(wiimotes, max_wiimotes, 5);

    if (found <= 0)
    {
        // First, pair any unpaired Wiimotes (Windows only)
        godot::UtilityFunctions::print("Press 1+2 to pair Wiimotes.");

        if (pair)
        {
            pair_wiimotes();
            godot::UtilityFunctions::print("Press 1+2 to initialize connection.");
            found = wiiuse_find(wiimotes, max_wiimotes, 5);
        }
    }

    if (found <= 0)
    {
        godot::UtilityFunctions::print("No Wiimotes found.");
        wiiuse_cleanup(wiimotes, max_wiimotes);
        wiimotes = nullptr;
        return;
    }

    num_connected = wiiuse_connect(wiimotes, max_wiimotes);
    if (num_connected <= 0)
    {
        godot::UtilityFunctions::print("Found ", found, " but could not connect to any.");
        wiiuse_cleanup(wiimotes, max_wiimotes);
        wiimotes = nullptr;
        return;
    }

    godot::UtilityFunctions::print("Wiimotes found: ", found, ", connected: ", num_connected);
    return;
}

godot::TypedArray<GDWiimote> GDWiimoteServer::finalize_connection()
{
    // Wrap each connected wiimote
    gdwiimotes.clear();

    for (int i = 0; i < num_connected; i++)
    {
        if (!wiimotes[i] || !WIIMOTE_IS_CONNECTED(wiimotes[i]))
            continue;

        godot::Ref<GDWiimote> gdwm = memnew(GDWiimote(wiimotes[i], i));
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

void GDWiimoteServer::disconnect_wiimotes()
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

void GDWiimoteServer::enable_polling()
{
    godot::SceneTree *tree = Object::cast_to<godot::SceneTree>(godot::Engine::get_singleton()->get_main_loop());
    if (tree)
    {
        godot::Callable cb = callable_mp(this, &GDWiimoteServer::poll);

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

void GDWiimoteServer::disable_polling()
{
    godot::SceneTree *tree = Object::cast_to<godot::SceneTree>(godot::Engine::get_singleton()->get_main_loop());
    if (tree)
    {
        godot::Callable cb = callable_mp(this, &GDWiimoteServer::poll);

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

void GDWiimoteServer::poll()
{
    if ((num_connected < 0) || !wiimotes)
        return;

    // Poll events
    if (wiiuse_poll(wiimotes, max_wiimotes))
    {
        for (int i = 0; i < gdwiimotes.size(); i++)
        {
            godot::Ref<GDWiimote> gdwm = gdwiimotes[i];
            if (!gdwm.is_valid())
                continue;

            wiimote *wm = gdwm->get_wiimote_ptr();
            if (!wm || !WIIMOTE_IS_CONNECTED(wm))
                continue;

            gdwm->handle_event();
        }
    }
}
