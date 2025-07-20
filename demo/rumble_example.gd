extends Node2D

var _is_rumbling: bool = false

@export_multiline var help: String

func init() -> void:
	pass

func _ready() -> void:
	process_mode = Node.PROCESS_MODE_DISABLED

func _process(delta: float) -> void:
	if Input.is_action_just_pressed("plus"):
		print("Rumble state set to true")
		GlobalWiimoteManager.set_rumble(0, true)
	elif Input.is_action_just_pressed("minus"):
		print("Rumble state set to false")
		GlobalWiimoteManager.set_rumble(0, false)
		
	if Input.is_action_just_pressed("A"):
		print("Rumble toggled!")
		GlobalWiimoteManager.toggle_rumble(0)

	if Input.is_action_just_pressed("B"):
		print("Rumble pulse started!")
		# returns a timer node; connections should be cleared manually since timer is persistent
		GlobalWiimoteManager.pulse_rumble(0, 0.2)
