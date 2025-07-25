#!/usr/bin/env python
import os
from SCons.Script import ARGUMENTS

env = SConscript("godot-cpp/SConstruct")

env.Append(CPPPATH=["src/", "include/"])
env.Append(LIBPATH=["libs/"])
env.Append(CPPDEFINES=["WIIUSE_STATIC"])

sources = Glob("src/*.cpp")

# Add documentation if targeting Godot 4.3+
try:
    doc_data = env.GodotCPPDocData(
        "src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml")
    )
    sources.append(doc_data)
except AttributeError:
    print("Not including class reference as we're targeting a pre-4.3 baseline.")

# Platform-specific libraries
platform = env["platform"]
if platform == "windows":
    env.Append(LIBS=["wiiuse", "ws2_32", "setupapi", "hid"])
else:
    env.Append(LIBS=["wiiuse", "bluetooth"])

# Output directory (default: demo/bin)
out_dir = ARGUMENTS.get("out", "demo/bin")
target = env["target"]
suffix = env["suffix"]
shlib = env["SHLIBSUFFIX"]

# Output path setup
if platform == "macos":
    lib_path = f"{out_dir}/gdwiiinput.{platform}.{target}.framework/gdwiiinput.{platform}.{target}"
    library = env.SharedLibrary(lib_path, source=sources)
elif platform == "ios":
    if env.get("ios_simulator", False):
        lib_path = f"{out_dir}/gdwiiinput.{platform}.{target}.simulator"
    else:
        lib_path = f"{out_dir}/gdwiiinput.{platform}.{target}"
    library = env.StaticLibrary(lib_path, source=sources)
else:
    lib_path = f"{out_dir}/gdwiiinput{suffix}{shlib}"
    library = env.SharedLibrary(lib_path, source=sources)

Default(library)
