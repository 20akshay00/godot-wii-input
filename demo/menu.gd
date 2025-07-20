extends Node2D

var _is_calibrating: bool= false

func _init() -> void:
	GlobalWiimoteManager.nunchuk_inserted.connect(_on_event)
	GlobalWiimoteManager.nunchuk_removed.connect(_on_event)

func _on_event(id: int) -> void:
	print("Hey!")

func _process(delta: float) -> void:
	for action in ["a", "b", "minus", "plus", "home"]:
		if Input.is_action_just_pressed(action):
			print("Pressed ", action)
		
	var dir = Input.get_vector("left_joy", "right_joy", "down_joy", "up_joy")	
	if !_is_calibrating: print(dir)
	
	if Input.is_action_just_pressed("a"):
		if _is_calibrating:
			print("STOP CALIB")
			GlobalWiimoteManager.stop_nunchuk_calibration()
		else:
			print("START CALIB")
			GlobalWiimoteManager.start_nunchuk_calibration()
			
		_is_calibrating = !_is_calibrating
