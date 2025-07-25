extends Node2D

var wiimote: GDWiimote
@export_multiline var help: String
@onready var cursor: Sprite2D = $Cursor
var _is_calibrated = true
@export var SPEED: float = 500.

func init() -> void:
	wiimote = GDWiimoteManager.get_connected_wiimotes()[0]
	wiimote.set_motion_sensing(true)
	cursor.show()

func _ready() -> void:
	process_mode = Node.PROCESS_MODE_DISABLED
	cursor.global_position = get_viewport().get_visible_rect().size / 2

func _process(delta: float) -> void:
	var acc = wiimote.get_accel()
	cursor.global_position.x -= acc.y * delta * SPEED
	cursor.global_position.y -= acc.x * delta * SPEED

func exit():
	wiimote.set_motion_sensing(false)
	cursor.hide()

func calibrate():
	_is_calibrated = false
	
