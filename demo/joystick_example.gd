extends Node2D

var _is_calibrating: bool= false
var _is_connected: bool = false
var _can_print: bool = false
var wiimote: GDWiimote

@export_multiline var help: String

func init() -> void:
	wiimote = GDWiimoteManager.get_connected_wiimotes()[0]
	_is_connected = wiimote.is_nunchuk_connected()
	if not _is_connected:
		print("Nunchuk not connected!")

	# Only connect if not already connected
	if not wiimote.nunchuk_inserted.is_connected(_on_nunchuk_inserted):
		wiimote.nunchuk_inserted.connect(_on_nunchuk_inserted)

	if not wiimote.nunchuk_removed.is_connected(_on_nunchuk_removed):
		wiimote.nunchuk_removed.connect(_on_nunchuk_removed)

func _ready() -> void:
	process_mode = Node.PROCESS_MODE_DISABLED

func _process(delta: float) -> void:
	var dir_joy = Input.get_vector("left_joy", "right_joy", "down_joy", "up_joy")	
	if !_is_calibrating && _is_connected && _can_print:
		print(dir_joy)

	if Input.is_action_just_pressed("A"):
		if _is_connected:
			if _is_calibrating:
				print("Calibration completed.")
				wiimote.stop_nunchuk_calibration()
			else:
				print("Calibration begun. Please rotate the joystick to maximum limits for a few seconds. Terminate calibration with A.")
				wiimote.start_nunchuk_calibration()
			_is_calibrating = !_is_calibrating
		else:
			print("Nunchuk not connected! Unable to initiate calibration.")
	
	if Input.is_action_just_pressed("minus"):
		if not _is_connected: print("Nunchuk not connected! Unable to poll data.")
		_can_print = false
	elif Input.is_action_just_pressed("plus"):
		if not _is_connected: print("Nunchuk not connected! Unable to poll data.")
		_can_print = true

func _on_nunchuk_inserted(id: int) -> void:
	_is_connected = true

func _on_nunchuk_removed(id: int) -> void:
	_is_connected = false

func exit():
	pass
