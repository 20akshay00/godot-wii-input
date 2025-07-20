extends Node2D

var current_example: int = 0

func _ready() -> void:
	GlobalWiimoteManager.wiimote_disconnected.connect(_on_wiimote_disconnected)
	_set_text()
	get_child(current_example).process_mode = Node.PROCESS_MODE_ALWAYS

func _on_wiimote_disconnected(id: int) -> void:
	print("Wiimote disconnected!")

func _process(delta: float) -> void:
	if Input.is_action_just_pressed("left_dpad") or Input.is_action_just_pressed("right_dpad"):
		_switch_example((current_example + int(Input.get_axis("left_dpad", "right_dpad")))%3)

func _switch_example(idx: int) -> void:
	get_child(current_example).process_mode = Node.PROCESS_MODE_DISABLED
	get_child(idx).process_mode = Node.PROCESS_MODE_ALWAYS
	get_child(idx).init()
	current_example = idx
	_set_text()

func _set_text() -> void:
	$Title.text = get_child(current_example).get_name()
	$Help.text = get_child(current_example).help
