# GDWiiInput – WIP!  

A scrappy little attempt to bring [Wiiuse](https://github.com/wiiuse/wiiuse) into Godot so you can play around with the Wii Remote (and some of its accessories) directly from GDScript. I honestly have no idea what I'm doing here, so if this interests you, contributions and tips are *very* welcome!  

## What works right now  
- All button presses (including the Nunchuk joystick) are routed through Godot's Input system  
- Manual joystick calibration (the default from Wiiuse didn't match my controller)
- LED control and rumble feedback  

## Roadmap  
- Make Nunchuk/Wiimote connect/disconnect detection more reliable  
- Hook up accelerometer (Wiimote + Nunchuk) and gyro (MotionPlus)  
- Async initial connection  
- Speaker support  
- Read IR data  
  
- Motion plus attachment
- Motion board

## Current limitations  
- **No built-in connection handling yet.** On Windows especially, Bluetooth can be a pain. For now, I just open Dolphin Emulator with continuous scanning on, pair the Wiimote, close Dolphin, and *then* run my Godot game. Eventually I'll dig into whatever Dolphin uses and see if it can be reused here.  
- **Windows-only for now.** Wiiuse *does* support Linux and macOS, so it should be doable once all features are added.  
- **Limited hardware testing.** I only have a MotionPlus Wiimote and a cheap third-party Nunchuk (no sensor bar), so I can't test how other setups behave.  

## Why!?

Because motion control deserves more love <3 and VR shouldn't be the only way to access it through a PC. 