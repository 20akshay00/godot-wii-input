extends Node2D

var wiimote: GDWiimote
@export_multiline var help: String
@onready var cursor: Sprite2D = $Cursor
var _is_calibrated := false

func init() -> void:
	wiimote = GDWiimoteServer.get_connected_wiimotes()[0]
	wiimote.set_motion_sensing(true)
	wiimote.set_motion_plus(true)
	wiimote.set_motion_processing(true)
	calibrate_gyro()
	cursor.show()

func _ready() -> void:
	process_mode = Node.PROCESS_MODE_DISABLED

var b1 = true
var b2 = true
var b3 = true

func _process(delta: float) -> void:
	if _is_calibrated:
		#var vel := wiimote.get_world_space_gyro(0.1)
		#cursor.global_position += Vector2(-vel.y, vel.x) * delta
		#print(vel)
		print("---")
		print(b1, " ", b2, " ", b3)
		#print(wiimote.get_accel())
		#print(wiimote.get_raw_gyro())
		#print(wiimote.get_fusion_orientation())
	
	if Input.is_action_just_pressed("down_dpad"):
		cursor.global_position = get_viewport().get_visible_rect().size / 2

	#if Input.is_action_just_pressed("A") and _is_calibrated:
		#calibrate_gyro(10.)
		
	if Input.is_action_just_pressed("B"):
		b1 = not b1
		wiimote.set_motion_sensing(b1)
	if Input.is_action_just_pressed("A"):
		b2 = not b2
		print("hi")
		wiimote.set_motion_plus(b2)
	if Input.is_action_just_pressed("home"):
		b3 = not b3
		wiimote.set_motion_processing(b3)
		
func exit():
	wiimote.set_motion_sensing(false)
	wiimote.set_motion_plus(false)
	wiimote.set_motion_processing(false)
	cursor.hide()

func calibrate_gyro(dt: float = 5.) -> void:
	_is_calibrated = false
	wiimote.reset_gyro_calibration()
	wiimote.start_gyro_calibration()
	print("Calibrating...")
	get_tree().create_timer(dt).timeout.connect(
		func(): print("Calibration complete."); wiimote.stop_gyro_calibration(); _is_calibrated = true;
		)
