.. _doc_building:

Building the GDExtension
===========================
There is not much to be gained as a user from compiling this yourself as GDWiiInput is available on the Godot Asset library as well. This page is intended as a reference for contributors.

Requirements: Godot 4.4+, Python, SCons, CMake.

1. Clone the repository:

.. code-block:: bash

   git clone --recursive https://github.com/20akshay00/godot-wii-input

2. Build `godot-cpp` bindings:

.. code-block:: bash

   cd godot-wii-input/godot-cpp
   scons platform=<platform>
   cd ..

3. Build the extension:

.. code-block:: bash

   scons platform=<platform>

After this, the demo project should be runnable from the Godot editor.

Wiiuse Compilation
===========================

This project uses Wiiuse as a static library from the ``/libs`` directory so compiling it again is not necessary.
**Linux dependencies:**

.. code-block:: bash

   sudo apt install libbluetooth-dev libbluetooth3 bluez

**Build Wiiuse:**

.. code-block:: bash

   cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON ..
   make

**Windows build (example using PowerShell):**

.. code-block:: powershell

   & "C:\Program Files\CMake\bin\cmake.exe" -S .. -B build -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS_RELEASE="/MT" -DCMAKE_CXX_FLAGS_RELEASE="/MT"
   & "C:\Program Files\CMake\bin\cmake.exe" --build build --config Release