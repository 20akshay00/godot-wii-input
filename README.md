# GDWiiInput – WIP!  

A scrappy little attempt to bring [Wiiuse](https://github.com/wiiuse/wiiuse) into Godot so you can play around with the Wii Remote (and some of its accessories) directly from GDScript. I honestly have no idea what I'm doing here as its my first GDExtension, so if this interests you, contributions and tips are *very* welcome!  

## Features 
- All button presses (including the Nunchuk joystick) are routed through Godot's Input system  
- Manual joystick calibration (in case the default from Wiiuse does not match, typically for third party nunchuks)
- LED control and rumble feedback  

### Roadmap
- Add build instructions
- Hook up accelerometer (Wiimote + Nunchuk) and gyro (MotionPlus) w/ calibration
- Speaker support

- Add basic motion control utilities through GamepadMotionHelpers
- Automatic releases w/ Github actions

### Wishlist
- IR data
- Out of the box gyro-based pointer controls
- Standalone bluetooth connection w/o external programs
- Motion plus attachment
- Motion board
- Better nunchuk deadzone detection

## Compiling 

## Connecting Wiimotes
- **Linux:** After `WiimoteManager.connect_wiimotes()` is called, press 1+2 on all the Wiimotes.
- **Windows:** The bluetooth stack of Windows is a bit quirky so the procedure is a bit more involved. If you use a bluetooth passthrough device such as the Dolphinbar, it should work similar to Linux. If not, you may use something like [WiiPair](https://github.com/jordanbtucker/WiiPair) beforehand and then launch the Godot session. Alternatively, you can use the continuous scanning option in [Dolphin emulator](https://github.com/dolphin-emu/dolphin). Apparently there are also ways to connect it via the native Windows bluetooth prompts, but I've not had any luck with these. Ideally a port of whatever Dolphin emulator is using would be the long-term solution.
 
## Current limitations
- **No built-in pairing handling for Windows.** 
- **Tested only on Windows and Linux.** I just don't have access to a Mac.
- **Limited hardware testing.** I only have a MotionPlus Wiimote and a cheap third-party Nunchuk (no sensor bar), so I can't test how other setups behave. 

## Permanent limitations
- **Third party accessories cannot be reliably supported.** They use all sorts of non-standard ways to initialize unfortunately.
- **Controllers cannot be added/removed mid session.** This includes Wiimotes and their extensions. For whatever reason, wiiuse runs on the assumption that all devices of interest are paired prior to launching the application and connected only once at the start. This is not a strict limitation of the Wiimote and a solution can be found if one just directly uses a HID layer to communicate with it, but that goes beyond the scope of this project. 

## Why!?  

This started as a simple way to get familiar with writing GDExtensions. The Wiimote felt like a fun target since it is one of the most affordable and accessible motion-centric controllers, even if it’s a bit dated, and it comes with plenty of quirky accessories to experiment with.

For modern, less motion-focused controllers, Godot already has a lot of [ongoing work](https://github.com/godotengine/godot-proposals/issues/2829) to improve support. So this isn't meant for serious usage by any means. It's more of a nostalgia-driven side project than anything else :')


### Stuff to check out
- https://github.com/JibbSmart/GamepadMotionHelpers
- https://wiiuse.net/?nav=api