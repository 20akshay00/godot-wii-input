extends Node2D

var led: int = 1
@export_multiline var help: String 
var wiimote: GDWiimote

func init():
	wiimote = GDWiimoteServer.get_connected_wiimotes()[0]

func _ready() -> void:
	process_mode = Node.PROCESS_MODE_DISABLED
	
func _process(delta: float) -> void:
	if Input.is_action_just_pressed("A"):
		led = led % 4 + 1
		wiimote.set_leds([led])
		
	if Input.is_action_just_pressed("B"):
		for i in 4:
			for led in [1, 2, 3, 4]:
				wiimote.set_leds([led])
				await get_tree().create_timer(0.1).timeout

	if Input.is_action_just_pressed("minus"):
		wiimote.set_leds([1, 2, 3, 4])

	if Input.is_action_just_pressed("plus"):
		print("\n-------\nLED Status: ")
		for led in [1, 2, 3, 4]:
			if wiimote.get_led(led):
				print("LED ", led, " is turned on.")
		print("-------\n")

func exit():
	pass
