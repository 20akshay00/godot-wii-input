#ifndef JOYSTICK_CALIBRATION_H
#define JOYSTICK_CALIBRATION_H

class GDJoystick
{
private:
    // Normalize a single axis
    float normalize_axis(float raw, float min, float max, float center) const;

public:
    float min_x = 1.0f;
    float max_x = -1.0f;
    float min_y = 1.0f;
    float max_y = -1.0f;
    float center_x = 0.0f;
    float center_y = 0.0f;
    float deadzone = 0.05f; // default deadzone

    float prev_x = 0.0f; // last raw X value
    float prev_y = 0.0f; // last raw Y value

    // Update calibration with new raw values
    void calibrate(float raw_x, float raw_y);

    float normalize_x(float raw_x) const;
    float normalize_y(float raw_y) const;
    void set_center(float raw_x, float raw_y);
    void set_deadzone(float dz);
    void initialize_calibration(float raw_x, float raw_y);
};

#endif // JOYSTICK_CALIBRATION_H
