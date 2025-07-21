shader_type canvas_item;

uniform float thickness : hint_range(0.01, 0.2) = 0.1;
uniform vec4 color : source_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform float speed = 1.0; // rotation speed in radians per second
uniform float arc_width = 5.5;

void fragment() {
    vec2 center = UV - vec2(0.5);
    float dist = length(center);

    // ring bounds
    if (dist > 0.5 || dist < 0.5 - thickness) {
        discard;
    }

    float angle = atan(center.y, center.x);
    float time = TIME * speed;
    float spin_pos = mod(angle + time, 6.2831853); // 2 * PI

    // Draw a bright arc segment that moves around the ring
    float intensity = smoothstep(arc_width, 0.0, abs(spin_pos - 0.0));

    COLOR = vec4(color.rgb, intensity * color.a);
}
