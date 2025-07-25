:allow_comments: False
GDWiiInput Documentation
============================

`GDWiiInput <https://github.com/20akshay00/godot-wii-input>`__ is a GDExtension for the `Godot Engine <https://godotengine.org>`__ that enables communication with Wii Remotes and their accessories using the `Wiiuse <https://github.com/wiiuse/wiiuse>`__ library.

It supports input from the Wiimote, Nunchuk, and Wii Balance Board. Core features include button mapping into Godot’s input system, motion and rumble support, LED control, and access to motion sensors.

Features
============

- **Button Input**: All Wiimote button and joystick inputs (including Nunchuk) are mapped into Godot's input system.
- **Motion Data**: Access raw accelerometer and gyroscope data. Basic processing is available using `GamepadMotionHelpers <https://github.com/JibbSmart/GamepadMotionHelpers>`__ (experimental).
- **LED & Rumble**: Control the Wiimote’s LEDs and trigger rumble feedback.

Table of Contents
============================

.. Add :hidden: to each to hide them on this page. For now it's better to have them for quick navigation.

.. toctree::
   :maxdepth: 2
   :caption: Setup
   :name: sec-learn

   setup/connecting
   setup/building
   setup/alternatives

.. toctree::
   :maxdepth: 2
   :caption: Class Reference
   :name: sec-class-ref

   classes/class_gdwiimote
   classes/class_wiimotemanager
