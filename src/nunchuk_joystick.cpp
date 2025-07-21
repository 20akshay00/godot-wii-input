#include "nunchuk_joystick.h"
#include <cmath>

void NunchukJoystick::initialize_joystick(nunchuk_t *nc, float deadzone, float threshold)
{
    deadzone = deadzone;
    threshold = threshold;

    min_x = nc->js.min.x;
    max_x = nc->js.max.x;
    min_y = nc->js.min.y;
    max_y = nc->js.max.y;
    center_x = nc->js.center.x;
    center_y = nc->js.center.y;
    prev_x = nc->js.x;
    prev_y = nc->js.y;
}

float NunchukJoystick::normalize_axis(float raw, float min, float max, float center) const
{
    float delta = raw - center;
    if ((delta < deadzone) && (delta > -deadzone))
        return 0.0f;
    return (delta > 0) ? (delta / (max - center)) : (delta / (center - min));
}

void NunchukJoystick::calibrate(float raw_x, float raw_y)
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

float NunchukJoystick::normalize_x(float raw_x) const
{
    return normalize_axis(raw_x, min_x, max_x, center_x);
}

float NunchukJoystick::normalize_y(float raw_y) const
{
    return normalize_axis(raw_y, min_y, max_y, center_y);
}

void NunchukJoystick::set_center(float raw_x, float raw_y)
{
    center_x = raw_x;
    center_y = raw_y;
}

void NunchukJoystick::initialize_calibration(float raw_x, float raw_y)
{
    min_x = 1e9f;
    max_x = -1e9f;
    min_y = 1e9f;
    max_y = -1e9f;
    center_x = raw_x;
    center_y = raw_y;
}

void NunchukJoystick::update_prev_pos(float raw_x, float raw_y)
{
    prev_x = raw_x;
    prev_y = raw_y;
}

bool NunchukJoystick::is_motion_detected(float raw_x, float raw_y) const
{
    return (fabs(raw_x - prev_x) > threshold || fabs(raw_y - prev_y) > threshold);
}
