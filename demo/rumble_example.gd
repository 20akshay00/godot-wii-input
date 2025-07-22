extends Node2D

var _is_rumbling: bool = false
var wiimote: GDWiimote
@export_multiline var help: String

func init() -> void:
	wiimote = GDWiimoteManager.get_connected_wiimotes()[0]

func _ready() -> void:
	process_mode = Node.PROCESS_MODE_DISABLED

func _process(delta: float) -> void:
	if Input.is_action_just_pressed("plus"):
		print("Rumble state set to true")
		wiimote.set_rumble(true)
	elif Input.is_action_just_pressed("minus"):
		print("Rumble state set to false")
		wiimote.set_rumble(false)
		
	if Input.is_action_just_pressed("A"):
		print("Rumble toggled!")
		wiimote.toggle_rumble()

	if Input.is_action_just_pressed("B"):
		print("Rumble pulse started!")
		wiimote.pulse_rumble(300)

func exit():
	pass
