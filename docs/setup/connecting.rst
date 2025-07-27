.. _doc_connecting:

Connecting the Wiimotes
=======================

In what follows, we refer to any instance of the game whether debug or exported as the *session*.

GDWiiInput manages all Wiimote connections and input polling through the engine singleton :ref:`GDWiimoteServer <class_GDWiimoteServer>.`. An important thing to note is that the server expects all remotes (and their expansions) to be connected to the PC **at the start of the session**. 

The session typically begins by scanning for connected (or discoverable, based on the OS) Wiimotes using :ref:`GDWiimoteServer.connect_wiimotes() <class_GDWiimoteServer_method_connect_wiimotes>` over a few seconds. The Wiimote uses Bluetooth, making PC connection possible. However, it communicates using some non-standard and outdated Bluetooth protocols, which can cause inconsistent behavior across operating systems. 

Linux
-----
Linux supports reading the Wiimote as a native controller so its the easiest the set up. The only requirement is the BlueZ Bluetooth stack.

.. code-block:: bash

   sudo apt install libbluetooth3 bluez

After calling :ref:`GDWiimoteServer.connect_wiimotes() <class_GDWiimoteServer_method_connect_wiimotes>`, press 1+2 on the Wiimote to initiate connection.

Windows
-------
Windows treats the Wiimote as a standard Bluetooth device by default, which often leads to broken or partial pairing. This occurs because the Windows Bluetooth stack does not fully support the HID extensions the Wiimote uses, preventing proper communication.

One solution is to use Bluetooth passthrough devices such as the `Mayflash DolphinBar <https://www.mayflash.com/product/W010.html>`__ to bypass the OS’s default handling, allowing the process to work similarly to Linux.

Alternatively, custom drivers like `WiiPair <https://github.com/jordanbtucker/WiiPair>`__ or `HID Wiimote <https://www.julianloehr.de/educational-work/hid-wiimote/>`__ can handle pairing correctly. Another reliable method is to pair the remote using the `Dolphin Emulator <https://github.com/dolphin-emu/dolphin>`__ with continuous scanning enabled, then close the emulator. Since the remote is already connected, calling :ref:`GDWiimoteServer.connect_wiimotes() <class_GDWiimoteServer_method_connect_wiimotes>` afterwards will detect it without requiring any additional button presses.

Finding the connected Wiimotes
==============================
Once detected by the session, the Wiimote will briefly rumble and light an LED, just like on the original console. The next step is to call :ref:`GDWiimoteServer.finalize_wiimotes() <class_GDWiimoteServer_method_finalize_connection>` to initialize the :ref:`GDWiimote <class_GDWiimote>` objects and begin using the remotes in your project. A typical initialization would look something like this.

.. code-block:: gdscript

    func _ready():
        # GDWiimoteServer.connect_wiimotes is blocking!
        Thread.new().start(self, "_connect_wiimotes_thread")

    func _connect_wiimotes_thread():
        # Initialize loading screen
        # ... 

        GDWiimoteServer.connect_wiimotes()
        call_deferred("_on_connection_complete")

    func _on_connection_complete():
        # Hide loading screen
        # ...

        # Retrieve connected Wiimotes 
        var connected_wiimotes = GDWiimoteServer.finalize_connection()
        ## can also retrieve later on with GDWiimoteServer.get_connected_wiimotes()

