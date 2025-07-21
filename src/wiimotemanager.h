#ifndef WIIMOTE_MANAGER_H
#define WIIMOTE_MANAGER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <wiiuse.h>

#include "gdwiimote.h"

class WiimoteManager : public godot::Node
{
    GDCLASS(WiimoteManager, godot::Node);

private:
    int max_wiimotes = 4;
    int num_connected = -1; // -1 means not connected

    wiimote **wiimotes = nullptr;            // raw wiimote pointers
    godot::TypedArray<GDWiimote> gdwiimotes; // wrapped GDWiimotes

protected:
    static void _bind_methods();

public:
    WiimoteManager();
    ~WiimoteManager();

    void set_max_wiimotes(int max);
    int get_max_wiimotes() const;

    // Explicitly connect to Wiimotes, returns an Array of GDWiimote instances.
    godot::TypedArray<GDWiimote> finalize_connection();
    void connect_wiimotes();

    // Disconnect and cleanup
    void disconnect_wiimotes();

    // Poll for events
    void _process(double delta) override;
    void start_polling();
    void stop_polling();

    // Access connected GDWiimotes
    godot::TypedArray<GDWiimote> get_connected_wiimotes() const { return gdwiimotes; }
};

#endif
