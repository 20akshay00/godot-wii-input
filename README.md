# GDWiiInput – WIP!  

A scrappy little attempt to bring [Wiiuse](https://github.com/wiiuse/wiiuse) into Godot so you can play around with the Wii Remote (and some of its accessories) directly from GDScript. I honestly have no idea what I'm doing here as its my first GDExtension, so if this interests you, contributions and tips are *very* welcome!  

## What works right now  
- All button presses (including the Nunchuk joystick) are routed through Godot's Input system  
- Manual joystick calibration (the default from Wiiuse didn't match my controller)
- LED control and rumble feedback  

## Roadmap  
- Hook up accelerometer (Wiimote + Nunchuk) and gyro (MotionPlus) w/ calibration
- Make initial wiimote connection async w/ loading bar
- Automatic mac/linux builds
- Make Nunchuk/Wiimote connect/disconnect detection more reliable?  

### Wishlist
- Standalone bluetooth connection w/o external programs
- Speaker support
- Read IR data  
- Motion plus attachment
- Motion board

## Current limitations  
- **No built-in connection handling yet.** On Windows especially, Bluetooth can be a pain. For now, I just open Dolphin Emulator with continuous scanning on, pair the Wiimote, close Dolphin, and *then* run my Godot game. Eventually I'll dig into whatever Dolphin uses and see if it can be reused here.
- **Short initial delay during connection.** Right now, it takes a few seconds for wiiuse to find a paired wiimote, so the Godot preview window hangs for a couple of seconds. I'll probably make this a manual function to be called that can be added by the user along with a loading bar or some such.
- **Windows-only for now.** Wiiuse *does* support Linux and macOS, so it should be doable once all features are added.  
- **Limited hardware testing.** I only have a MotionPlus Wiimote and a cheap third-party Nunchuk (no sensor bar), so I can't test how other setups behave.  

## Why!?  

This started as a simple way to get familiar with writing GDExtensions. The Wiimote felt like a fun target since it is one of the most affordable and accessible motion-centric controllers, even if it’s a bit dated, and it comes with plenty of quirky accessories to experiment with.

For modern, less motion-focused controllers, Godot already has a lot of [ongoing work](https://github.com/godotengine/godot-proposals/issues/2829) to improve support. So this isn't meant for serious usage by any means. It's more of a nostalgia-driven side project than anything else :')
