.. _doc_connecting:

Connecting the Wiimotes
=======================

To begin using the extension, we first need to setup a global autoload which is a :ref:`WiimoteManager <class_WiimoteManager>` node. This node is responsible for managing all Wiimote connections and polling.

An important thing to note is that GDWiiInput expects all remotes (and their expansions) to be connected to the device **before the session starts**. The session then begins by calling :ref:`WiimoteManager.connect_wiimotes() <class_WiimoteManager_method_connect_wiimotes>` which scans for any Wiimotes already paired at that time.  
You can also configure the maximum number of allowed remotes using the exported property :ref:`max_wiimotes <class_WiimoteManager_property_max_wiimotes>`.

So before launching the session, the Wiimotes have to be connected to the device. The Wiimote uses Bluetooth, making PC connection possible. However, it communicates using some non-standard and outdated Bluetooth protocols, which can cause inconsistent behavior across operating systems.

Linux
-----
On Linux, the BlueZ Bluetooth stack is required:

.. code-block:: bash

   sudo apt install libbluetooth3 bluez

After calling :ref:`WiimoteManager.connect_wiimotes() <class_WiimoteManager_method_connect_wiimotes>`, press 1+2 on the Wiimote to initiate pairing.

Windows
-------
Windows treats the Wiimote as a standard Bluetooth device by default, which often leads to broken or partial pairing. This occurs because the Windows Bluetooth stack does not fully support the HID extensions the Wiimote uses, preventing proper communication.

One solution is to use Bluetooth passthrough devices such as the `Mayflash DolphinBar <https://www.mayflash.com/product/W010.html>`__ to bypass the OS’s default handling, allowing the process to work similarly to Linux.

Alternatively, custom drivers like `WiiPair <https://github.com/jordanbtucker/WiiPair>`__ or `HID Wiimote <https://www.julianloehr.de/educational-work/hid-wiimote/>`__ can handle pairing correctly. Another reliable method is to pair the remote using the `Dolphin Emulator <https://github.com/dolphin-emu/dolphin>`__ with continuous scanning enabled, then close the emulator. Since the remote remains connected, calling :ref:`WiimoteManager.connect_wiimotes() <class_WiimoteManager_method_connect_wiimotes>` afterwards will detect it without requiring any button presses.

Finding the connected Wiimotes
==============================
Once detected by the application, the Wiimote will briefly rumble and light an LED, just like on the original console. The next step is to call :ref:`WiimoteManager.finalize_wiimotes() <class_WiimoteManager_method_finalize_connection>` to initialize the :ref:`GDWiimote <class_GDWiimote>` objects and begin using the remotes in your project. A typical initialization would look something like this.

.. code-block:: gdscript

    func _ready():
        # WiimoteManager.connect_wiimotes is blocking!
        Thread.new().start(self, "_connect_wiimotes_thread")

    func _connect_wiimotes_thread():
        # Initialize loading screen
        # ... 

        WiimoteManager.connect_wiimotes()
        call_deferred("_on_connection_complete")

    func _on_connection_complete():
        # Hide loading screen
        # ...

        # Retrieve connected Wiimotes 
        var connected_wiimotes = WiimoteManager.finalize_connection()
        ## can also retrieve later on with WiimoteManager.get_connected_wiimotes()

