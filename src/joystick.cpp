#include "joystick.h"
#include <cmath>

float GDJoystick::normalize_axis(float raw, float min, float max, float center) const
{
    float delta = raw - center;
    if ((delta < deadzone) && (delta > -deadzone))
        return 0.0f;
    return (delta > 0) ? (delta / (max - center)) : (delta / (center - min));
}

void GDJoystick::calibrate(float raw_x, float raw_y)
{
    if (raw_x < center_x && raw_x < min_x)
        min_x = raw_x;
    if (raw_x > center_x && raw_x > max_x)
        max_x = raw_x;
    if (raw_y < center_y && raw_y < min_y)
        min_y = raw_y;
    if (raw_y > center_y && raw_y > max_y)
        max_y = raw_y;
}

float GDJoystick::normalize_x(float raw_x) const
{
    return normalize_axis(raw_x, min_x, max_x, center_x);
}

float GDJoystick::normalize_y(float raw_y) const
{
    return normalize_axis(raw_y, min_y, max_y, center_y);
}

void GDJoystick::set_center(float raw_x, float raw_y)
{
    center_x = raw_x;
    center_y = raw_y;
}

void GDJoystick::set_deadzone(float dz)
{
    deadzone = dz;
}

void GDJoystick::set_threshold(float th)
{
    threshold = th;
}

void GDJoystick::initialize_calibration(float raw_x, float raw_y)
{
    min_x = 1e9f;
    max_x = -1e9f;
    min_y = 1e9f;
    max_y = -1e9f;
    center_x = raw_x;
    center_y = raw_y;
}

void GDJoystick::update_prev_pos(float raw_x, float raw_y)
{
    prev_x = raw_x;
    prev_y = raw_y;
}

bool GDJoystick::is_motion_detected(float raw_x, float raw_y) const
{
    return (fabs(raw_x - prev_x) > threshold || fabs(raw_y - prev_y) > threshold);
}
