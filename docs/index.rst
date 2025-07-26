:allow_comments: False
GDWiiInput Documentation
============================

`GDWiiInput <https://github.com/20akshay00/godot-wii-input>`__ is a GDExtension for the `Godot Engine <https://godotengine.org>`__ that enables communication with the Wiimote and its accessories using the `Wiiuse <https://github.com/wiiuse/wiiuse>`__ library. It currently supports input from the Wiimote, Nunchuk, and Wii Balance Board.

Features
============

- **Button Input**: All Wiimote button and joystick inputs (including Nunchuk) are mapped into Godot's input system.
- **LED & Rumble**: Control the Wiimote’s LEDs and trigger rumble feedback.
- **IR Data**: Access the positions and sizes of upto 4 IR clusters detected by the IR camera.
- **Motion Data**: Access raw accelerometer and gyroscope data. Basic processing is available using `GamepadMotionHelpers <https://github.com/JibbSmart/GamepadMotionHelpers>`__ (experimental).

Table of Contents
============================

.. Add :hidden: to each to hide them on this page. For now it's better to have them for quick navigation.

.. toctree::
   :maxdepth: 1
   :caption: Setup
   :name: sec-learn

   setup/connecting
   setup/inputs

.. toctree::
   :maxdepth: 1
   :caption: Contribute
   :name: sec-contribute

   contribute/building
   contribute/alternatives

.. toctree::
   :maxdepth: 2
   :caption: Class Reference
   :name: sec-class-ref

   classes/class_gdwiimoteserver
   classes/class_gdwiimote