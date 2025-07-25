.. _doc_inputs:

Reading Wiimote inputs
=========================

All button presses are routed through Godot's Input system. Since some Wiimote buttons are not standard, they have been arbitrarily mapped. For completeness, all the mappings are listed as follows:

+-------------------+-------------------+---------------------------------------------------------------+
| Wiimote Button    | Godot Input       | Description                                                   |
+===================+===================+===============================================================+
| A                 | Joypad Button 0   | Bottom Action (Xbox: A, Sony: Cross, Nintendo: B)             |
+-------------------+-------------------+---------------------------------------------------------------+
| B                 | Joypad Button 1   | Right Action (Xbox: B, Sony: Circle, Nintendo: A)             |
+-------------------+-------------------+---------------------------------------------------------------+
| Plus              | Joypad Button 6   | Start (Xbox: Menu, Sony: Options, Nintendo: +)                |
+-------------------+-------------------+---------------------------------------------------------------+
| Minus             | Joypad Button 4   | Select (Xbox: Back, Sony: Select, Nintendo: -)                |
+-------------------+-------------------+---------------------------------------------------------------+
| Home              | Joypad Button 5   | Guide (Xbox: Home, Sony: PS, Nintendo: Home)                  |
+-------------------+-------------------+---------------------------------------------------------------+
| Nunchuk C Button  | Joypad Button 9   | Left Shoulder (Xbox: LB, Sony: L1, Nintendo: L)               |
+-------------------+-------------------+---------------------------------------------------------------+
| Nunchuk Z Button  | Joypad Button 10  | Right Shoulder (Xbox: RB, Sony: R1, Nintendo: R)              |
+-------------------+-------------------+---------------------------------------------------------------+
| D-Pad Up          | Joypad Button 11  |                                                               |
+-------------------+-------------------+---------------------------------------------------------------+
| D-Pad Down        | Joypad Button 12  |                                                               |
+-------------------+-------------------+---------------------------------------------------------------+
| D-Pad Left        | Joypad Button 13  |                                                               |
+-------------------+-------------------+---------------------------------------------------------------+
| D-Pad Right       | Joypad Button 14  |                                                               |
+-------------------+-------------------+---------------------------------------------------------------+

The Nunchuk joystick motion is reported as an ``InputEventJoypadMotion``, so it can be accessed through the standard ``Joypad Axis`` fields in the InputMap. All other input such as accelerometer and gyroscope readings do not have a counterpart in the Input system and must be retrieved manually (although they are polled automatically) through the various functions of :ref:`GDWiimote <class_GDWiimote>`.