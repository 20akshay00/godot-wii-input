extends Node2D

var wiimote: GDWiimote
@export_multiline var help: String

func init() -> void:
	wiimote = GDWiimoteManager.get_connected_wiimotes()[0]
	wiimote.set_motion_sensing(true)
	wiimote.set_motion_plus(true)

func _ready() -> void:
	process_mode = Node.PROCESS_MODE_DISABLED

func _process(delta: float) -> void:
	print(wiimote.get_gforce())

func exit():
	wiimote.set_motion_sensing(false)
	wiimote.set_motion_plus(false)
