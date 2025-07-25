#ifdef DEBUG_ENABLED
#define DEBUG_PRINT(...) godot::UtilityFunctions::print(__VA_ARGS__)
#else
#define DEBUG_PRINT(...) ((void)0)
#endif