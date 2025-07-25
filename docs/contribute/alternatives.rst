.. _doc_alternative:

Limitations of GDWiiInput
===========================

Current
--------
- No built-in pairing for Windows.
- High latency on Linux (possibly system-specific).
- Limited hardware testing (only MotionPlus Wiimote and third-party Nunchuk tested).

Permanent
---------
- Third-party accessories may not work reliably due to non-standard protocols.
- Devices cannot be added or removed mid-session since Wiiuse requires all devices to be connected at startup.

Alternatives to GDWiiInput
==========================

For reference, here are some alternative Wii interfaces for Godot which may circumvent certain limitations:

1. `godot-wiimote <https://github.com/Computational-Geometry-G1/godot-wiimote>`__  
   Designed for Godot 4.x using `Wiiuse <https://github.com/wiiuse/wiiuse>`__ as its backend. It currently only exposes IR functionality.

2. `Xwiimote-godot <https://github.com/Clueninja/xwiimote-godot>`__  
   Designed for Godot 3.x with `XWiimote <https://github.com/xwiimote/xwiimote>`__ as its backend. It supports all Wiimote and Nunchuk features but only works on Linux. This may be a better choice if targeting Linux exclusively.
