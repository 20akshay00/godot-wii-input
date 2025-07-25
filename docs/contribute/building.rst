.. _doc_building:

Building the GDExtension
===========================
If you are interested in contributing to this project, following are the steps to compile GDWiiInput locally.

**Requirements:** Godot 4.4+, Python, SCons, CMake.

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

Compiling Wiiuse
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

Building documentation
======================

Navigate to the ``demo`` project folder inside the repository and run the following to generate the XML documentation for all the exposed classes and methods.

.. code-block:: bash

   # Replace "godot" with the full path to a Godot editor binary
   # if Godot is not installed in your `PATH`.
   godot --doctool ../ --gdextension-docs

Add the documentation to the files generated in ``/doc_classes`` and generate the RST files for the online documentation like so.

.. code-block:: bash
   
   curl -sSL https://raw.githubusercontent.com/godotengine/godot/master/doc/tools/make_rst.py | python3 - -o "docs/classes" -l "en" doc_classes