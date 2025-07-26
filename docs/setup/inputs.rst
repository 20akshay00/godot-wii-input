.. _doc_inputs:

Reading Wiimote inputs
=========================

Button presses
--------------

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
| D-Pad Up          | Joypad Button 11  | D-Pad Up                                                      |
+-------------------+-------------------+---------------------------------------------------------------+
| D-Pad Down        | Joypad Button 12  | D-Pad Down                                                    |
+-------------------+-------------------+---------------------------------------------------------------+
| D-Pad Left        | Joypad Button 13  | D-Pad Left                                                    |
+-------------------+-------------------+---------------------------------------------------------------+
| D-Pad Right       | Joypad Button 14  | D-Pad Right                                                   |
+-------------------+-------------------+---------------------------------------------------------------+

The Nunchuk joystick motion is reported as an ``InputEventJoypadMotion``, so it can be accessed through the standard ``Joypad Axis`` fields in the InputMap.

Sensor data
-----------

All other input such as IR, accelerometer and gyroscope readings do not have a counterpart in the Input system. As a result, they must be retrieved manually through the various functions of :ref:`GDWiimote <class_GDWiimote>` such as :ref:`get_accel() <class_GDWiimote_method_get_accel>` :ref:`get_raw_gyro() <class_GDWiimote_method_get_raw_gyro>`, :ref:`get_ir_cursor_calculated_position() <class_GDWiimote_method_get_ir_cursor_calculated_position>`, etc. Note that none of these are polled by default and must be explicitly enabled through functions like :ref:`set_motion_sensing() <class_GDWiimote_method_set_motion_sensing>`, :ref:`set_motion_plus() <class_GDWiimote_method_set_motion_plus>`, :ref:`set_ir() <class_GDWiimote_method_set_ir>`, etc. 