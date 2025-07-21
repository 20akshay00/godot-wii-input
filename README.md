# GDWiiInput – WIP!  

A scrappy little attempt to bring [Wiiuse](https://github.com/wiiuse/wiiuse) into Godot so you can play around with the Wii Remote (and some of its accessories) directly from GDScript. I honestly have no idea what I'm doing here as its my first GDExtension, so if this interests you, contributions and tips are *very* welcome!  

## What works right now  
- All button presses (including the Nunchuk joystick) are routed through Godot's Input system  
- Manual joystick calibration (in case the default from Wiiuse does not match, typically for third party nunchuks)
- LED control and rumble feedback  

## Roadmap
- Hook up accelerometer (Wiimote + Nunchuk) and gyro (MotionPlus) w/ calibration
- Automatic mac/linux builds
- Better deadzone detection

### Wishlist
- Standalone bluetooth connection w/o external programs
- Speaker support
- Read IR data  
- Motion plus attachment
- Motion board

## Current limitations
- **No built-in connection handling yet.** The Wii remote uses somewhat non-standard bluetooth protocols to communicate with devices, so there is a degree of variability in how the remote must be paired with the OS. On Linux, simply press 1+2 and then launch the Godot game to connect. On Windows, this doesn't really work. My workaround for now is to open the Dolphin Emulator with continuous scanning on, pair the Wiimote, close Dolphin, and *then* run my Godot game. Eventually I'll dig into whatever Dolphin uses and see if it can be reused here.
- **Windows-only for now.** Wiiuse *does* support Linux and macOS, so it should be doable once all features are added.  
- **Limited hardware testing.** I only have a MotionPlus Wiimote and a cheap third-party Nunchuk (no sensor bar), so I can't test how other setups behave.

## Permanent limitations

- **Third party accessories cannot be reliably supported.** They use all sorts of non-standard ways to initialize unfortunately.
- **Controllers cannot be added/removed mid session.** This includes Wiimotes and their extensions. Wiiuse runs on the assumption that all devices of interest are paired prior to launching the application and connected only once at the start. Perhaps a solution can be found if one just directly uses a HID layer to communicate with the Wiimote, but that goes beyond the scope of this project. 

## Why!?  

This started as a simple way to get familiar with writing GDExtensions. The Wiimote felt like a fun target since it is one of the most affordable and accessible motion-centric controllers, even if it’s a bit dated, and it comes with plenty of quirky accessories to experiment with.

For modern, less motion-focused controllers, Godot already has a lot of [ongoing work](https://github.com/godotengine/godot-proposals/issues/2829) to improve support. So this isn't meant for serious usage by any means. It's more of a nostalgia-driven side project than anything else :')


### Stuff to check out
- https://github.com/JibbSmart/GamepadMotionHelpers
- https://wiiuse.net/?nav=api