extends Node2D

var current_example: int = 0
var connection_thread := Thread.new()
var connection_complete := false

@onready var loading_circle := $LoadingCircle

func _ready() -> void:
	loading_circle.show()
	connection_thread.start(Callable(self, "_thread_connect_wiimotes"))

func _process(delta: float) -> void:
	if Input.is_action_just_pressed("left_dpad") or Input.is_action_just_pressed("right_dpad"):
		_switch_example(posmod(current_example + int(Input.get_axis("left_dpad", "right_dpad")), 4))

func _thread_connect_wiimotes():
	# assigns wiiuse structures
	GDWiimoteManager.connect_wiimotes()
	call_deferred("_on_connection_complete")

func _on_connection_complete():
	# allocate GDWiimote objects
	var wiimotes = GDWiimoteManager.finalize_connection()
	loading_circle.hide()
	$Title.show()
	$Help.show()
	_set_text()
	get_child(current_example).process_mode = Node.PROCESS_MODE_ALWAYS
	get_child(current_example).init()

func _switch_example(idx: int) -> void:
	get_child(current_example).process_mode = Node.PROCESS_MODE_DISABLED
	get_child(current_example).exit()
	get_child(idx).process_mode = Node.PROCESS_MODE_ALWAYS
	get_child(idx).init()
	current_example = idx
	_set_text()

func _set_text() -> void:
	$Title.text = get_child(current_example).get_name()
	$Help.text = get_child(current_example).help
