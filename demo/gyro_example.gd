extends Node2D

var wiimote: GDWiimote
@export_multiline var help: String

## for some reason, setting motion plus disconnects the nunchuk
func init() -> void:
	wiimote = GDWiimoteManager.get_connected_wiimotes()[0]
	wiimote.set_motion_sensing(true)
	wiimote.set_motion_plus(true)
	wiimote.set_motion_processing(true)

func _ready() -> void:
	process_mode = Node.PROCESS_MODE_DISABLED

func _process(delta: float) -> void:
	print(wiimote.get_raw_gyro())
	print(wiimote.get_world_space_gyro(0.1))
	print(wiimote.get_fusion_orientation())
	
func exit():
	wiimote.set_motion_sensing(false)
	wiimote.set_motion_plus(false)
