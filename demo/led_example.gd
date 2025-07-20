extends Node2D

var led: int = 1
@export_multiline var help: String

func init():
	pass

func _ready() -> void:
	process_mode = Node.PROCESS_MODE_DISABLED
	
func _process(delta: float) -> void:
	if Input.is_action_just_pressed("A"):
		led = led % 4 + 1
		GlobalWiimoteManager.set_leds(0, [led])
		
	if Input.is_action_just_pressed("B"):
		for i in 4:
			for led in [1, 2, 3, 4]:
				GlobalWiimoteManager.set_leds(0, [led])
				await get_tree().create_timer(0.1).timeout

	if Input.is_action_just_pressed("minus"):
		GlobalWiimoteManager.set_leds(0, [1, 2, 3, 4])

	if Input.is_action_just_pressed("plus"):
		print("\n-------\nLED Status: ")
		for led in [1, 2, 3, 4]:
			if GlobalWiimoteManager.get_led(0, led):
				print("LED ", led, " is turned on.")
		print("-------\n")
