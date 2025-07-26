extends Node3D

var connection_thread := Thread.new()
var connection_complete := false
var wiimote: GDWiimote

@onready var loading_circle := $LoadingCircle
@onready var model := $Model
var _is_calibrated: bool = false

func _ready() -> void:
	loading_circle.show()
	connection_thread.start(Callable(self, "_thread_connect_wiimotes"))

func _process(delta: float) -> void:
	if _is_calibrated and Input.is_action_just_pressed("A"):
		_is_calibrated = false 
		wiimote.reset_gyro_calibration()
		wiimote.start_gyro_calibration()
		print("Calibrating...")
		get_tree().create_timer(15).timeout.connect(
			func(): print("Calibration complete."); wiimote.stop_gyro_calibration(); _is_calibrated = true;
			)

	if connection_complete and _is_calibrated:
		model.transform.basis = Basis(wiimote.get_fusion_orientation())
		print(wiimote.get_accel())
	
func _thread_connect_wiimotes():
	# assigns wiiuse structures
	WiimoteManager.connect_wiimotes()
	call_deferred("_on_connection_complete")

func _on_connection_complete():
	# allocate GDWiimote objects
	wiimote = WiimoteManager.finalize_connection()[0]
	loading_circle.hide()
	connection_complete = true
	
	wiimote.set_motion_sensing(true)
	wiimote.set_motion_plus(true)
	wiimote.set_motion_processing(true)
	
	wiimote.reset_gyro_calibration()
	wiimote.start_gyro_calibration()
	print("Calibrating...")
	get_tree().create_timer(6).timeout.connect(
		func(): print("Calibration complete."); wiimote.stop_gyro_calibration(); _is_calibrated = true;
		)
	
