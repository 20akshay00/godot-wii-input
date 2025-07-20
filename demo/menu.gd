extends Node2D

var _is_calibrating: bool= false
var is_rumbling: bool = false
var led := 1
func _init() -> void:
	GlobalWiimoteManager.nunchuk_inserted.connect(_on_event)
	GlobalWiimoteManager.nunchuk_removed.connect(_on_event)

func _on_event(id: int) -> void:
	print("Hey!")

func _process(delta: float) -> void:
	for action in ["A", "B", "minus", "plus", "home", "C", "Z"]:
		if Input.is_action_just_pressed(action):
			print("Pressed ", action)
	
	if Input.is_action_just_pressed("B"):
		led = led % 4 + 1
		print(led)
		
		GlobalWiimoteManager.set_leds(0, [led])
			#if Input.is_action_just_pressed("B"):
		#GlobalWiimoteManager.pulse_rumble(0, 1);

	var dir_joy = Input.get_vector("left_joy", "right_joy", "down_joy", "up_joy")	
	var dir_dpad = Input.get_vector("left_dpad", "right_dpad", "down_dpad", "up_dpad")	

	#print(dir_joy, " ", dir_dpad)
	#if !_is_calibrating: print(dir)
	
	if Input.is_action_just_pressed("A"):
		if _is_calibrating:
			print("STOP CALIB")
			GlobalWiimoteManager.stop_nunchuk_calibration()
		else:
			print("START CALIB")
			GlobalWiimoteManager.start_nunchuk_calibration()
			
		_is_calibrating = !_is_calibrating
