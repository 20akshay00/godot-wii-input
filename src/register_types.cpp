#include "register_types.h"

#include "wiimotemanager.h"

#include <godot_cpp/classes/engine.hpp>
#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

static WiimoteManager *wiimote_manager_singleton = nullptr;

void initialize_example_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }

    GDREGISTER_RUNTIME_CLASS(GDWiimote);
    GDREGISTER_RUNTIME_CLASS(WiimoteManager)

    if (!wiimote_manager_singleton)
    {
        wiimote_manager_singleton = memnew(WiimoteManager);
        Engine::get_singleton()->register_singleton("WiimoteManager", wiimote_manager_singleton);
    }
}

void uninitialize_example_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
        return;

    if (wiimote_manager_singleton)
    {
        Engine::get_singleton()->unregister_singleton("WiimoteManager");
        memdelete(wiimote_manager_singleton);
        wiimote_manager_singleton = nullptr;
    }
}

extern "C"
{
    // Initialization.
    GDExtensionBool GDE_EXPORT gdwiiinput_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
    {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_example_module);
        init_obj.register_terminator(uninitialize_example_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}