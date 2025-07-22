#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

# documentation
try:
    doc_data = env.GodotCPPDocData(
        "src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml")
    )
    sources.append(doc_data)
except AttributeError:
    print("Not including class reference as we're targeting a pre-4.3 baseline.")


env.Append(CPPPATH=["include/"])
env.Append(LIBPATH=["libs/"])
env.Append(LIBS=["wiiuse"])

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "demo/bin/gdwiiinput.{}.{}.framework/gdwiiinput.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
elif env["platform"] == "ios":
    if env["ios_simulator"]:
        library = env.StaticLibrary(
            "demo/bin/gdwiiinput.{}.{}.simulator.a".format(
                env["platform"], env["target"]
            ),
            source=sources,
        )
    else:
        library = env.StaticLibrary(
            "demo/bin/gdwiiinput.{}.{}.a".format(env["platform"], env["target"]),
            source=sources,
        )
else:
    library = env.SharedLibrary(
        "demo/bin/gdwiiinput{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
