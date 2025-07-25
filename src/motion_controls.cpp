#include "gdwiimote.h"
#include <godot_cpp/variant/quaternion.hpp>

// Enable/disable motion sensing capabilities
void GDWiimote::set_motion_sensing(bool enable)
{
    if (wm)
        wiiuse_motion_sensing(wm, enable ? 1 : 0);
}

void GDWiimote::set_motion_plus(bool enable)
{
    if (wm)
    {
        if (enable)
        {
            if ((wm->exp.type == EXP_NUNCHUK) || (wm->exp.type == EXP_MOTION_PLUS_NUNCHUK))
            {
                wiiuse_set_motion_plus(wm, 2); // nunchuck pass-through
            }
            else
            {
                wiiuse_set_motion_plus(wm, 1); // standalone
            }
        }
        else
        {
            wiiuse_set_motion_plus(wm, 0); // disable
        }
    }
}

void GDWiimote::set_motion_processing(bool enable)
{
    process_motion = enable;
}

// Motion thresholds

void GDWiimote::set_orient_threshold(float threshold)
{
    orient_threshold = threshold;
    if (wm)
        wiiuse_set_orient_threshold(wm, threshold);
}

void GDWiimote::set_accel_threshold(int threshold)
{
    accel_threshold = threshold;
    if (wm)
        wiiuse_set_accel_threshold(wm, threshold);
}

void GDWiimote::set_nunchuk_orient_threshold(float th) { nunchuk_orient_threshold = th; }

void GDWiimote::set_nunchuk_accel_threshold(int th) { nunchuk_accel_threshold = th; }

// Accelerometer
godot::Vector3 GDWiimote::get_raw_accel() const
{
    if (!wm)
        return godot::Vector3();

    return godot::Vector3(wm->accel.x, wm->accel.y, wm->accel.z);
}

godot::Vector3 GDWiimote::get_accel() const
{
    if (!wm)
        return godot::Vector3();

    return godot::Vector3(wm->gforce.x, wm->gforce.y, wm->gforce.z);
}

godot::Vector3 GDWiimote::get_processed_accel()
{
    if (!wm)
        return godot::Vector3();

    float x, y, z;
    motion_state.GetProcessedAcceleration(x, y, z);
    return godot::Vector3(x, y, z);
}

godot::Vector3 GDWiimote::get_nunchuk_raw_accel() const
{
    if (!is_nunchuk_connected())
        return godot::Vector3();

    return godot::Vector3(wm->exp.nunchuk.accel.x, wm->exp.nunchuk.accel.y, wm->exp.nunchuk.accel.z);
}

godot::Vector3 GDWiimote::get_nunchuk_accel() const
{
    if (!wm)
        return godot::Vector3();

    return godot::Vector3(wm->exp.nunchuk.gforce.x, wm->exp.nunchuk.gforce.y, wm->exp.nunchuk.gforce.z);
}

// Gyroscope

void GDWiimote::start_gyro_calibration()
{
    if (process_motion)
    {
        motion_state.StartContinuousCalibration();
    }
    else
    {
        godot::UtilityFunctions::print("Motion sensing is not enabled, cannot start gyro calibration.");
    }
}

void GDWiimote::stop_gyro_calibration()
{
    if (process_motion)
    {
        motion_state.PauseContinuousCalibration();
    }
    else
    {
        godot::UtilityFunctions::print("Motion sensing is not enabled, cannot stop gyro calibration.");
    }
}

void GDWiimote::reset_gyro_calibration()
{
    if (process_motion)
    {
        motion_state.ResetContinuousCalibration();
    }
    else
    {
        godot::UtilityFunctions::print("Motion sensing is not enabled, cannot reset gyro calibration.");
    }
}

void GDWiimote::set_gyro_calibration_mode(int mode)
{
    if (process_motion)
    {
        switch (mode)
        {
        case 0:
            motion_state.SetCalibrationMode(GamepadMotionHelpers::CalibrationMode::Manual);
            break;
        case 1:
            motion_state.SetCalibrationMode(GamepadMotionHelpers::CalibrationMode::Stillness);
            break;
        case 2:
            motion_state.SetCalibrationMode(GamepadMotionHelpers::CalibrationMode::SensorFusion);
            break;
        }
    }
    else
    {
        godot::UtilityFunctions::print("Motion sensing is not enabled, cannot set gyro calibration mode.");
    }
}

godot::Vector3 GDWiimote::get_raw_gyro() const
{
    if (!wm || !(wm->exp.type == EXP_MOTION_PLUS || wm->exp.type == EXP_MOTION_PLUS_NUNCHUK))
        return godot::Vector3();

    return godot::Vector3(wm->exp.mp.angle_rate_gyro.yaw,
                          wm->exp.mp.angle_rate_gyro.pitch,
                          wm->exp.mp.angle_rate_gyro.roll);
}

godot::Vector2 GDWiimote::get_world_space_gyro(float sideReductionThreshold = 0.125f)
{
    if (!wm || !(wm->exp.type == EXP_MOTION_PLUS || wm->exp.type == EXP_MOTION_PLUS_NUNCHUK))
        return godot::Vector2();

    float x, y;
    motion_state.GetWorldSpaceGyro(x, y, sideReductionThreshold);
    return godot::Vector2(x, y);
}

godot::Vector2 GDWiimote::get_player_space_gyro(float yawRelaxFactor = 1.41f)
{
    if (!wm || !(wm->exp.type == EXP_MOTION_PLUS || wm->exp.type == EXP_MOTION_PLUS_NUNCHUK))
        return godot::Vector2();

    float x, y;
    motion_state.GetWorldSpaceGyro(x, y, yawRelaxFactor);
    return godot::Vector2(x, y);
}

// Orientation

godot::Quaternion GDWiimote::get_fusion_orientation()
{
    // Convert the Euler angles to a quaternion
    float w, x, y, z;
    motion_state.GetOrientation(w, x, y, z);
    return godot::Quaternion(x, y, z, w);
}

godot::Vector3 GDWiimote::get_raw_orientation() const
{
    if (!wm)
        return godot::Vector3();

    return godot::Vector3(wm->orient.yaw, wm->orient.pitch, wm->orient.roll);
}

godot::Vector3 GDWiimote::get_smoothed_orientation() const
{
    if (!wm)
        return godot::Vector3();

    return godot::Vector3(0.f, wm->orient.a_pitch, wm->orient.a_roll);
}

godot::Vector3 GDWiimote::get_nunchuk_raw_orientation() const
{
    if (!is_nunchuk_connected())
        return godot::Vector3();

    return godot::Vector3(wm->exp.nunchuk.orient.yaw, wm->exp.nunchuk.orient.pitch, wm->exp.nunchuk.orient.roll);
}

godot::Vector3 GDWiimote::get_nunchuk_smoothed_orientation() const
{
    if (!is_nunchuk_connected())
        return godot::Vector3();

    return godot::Vector3(0.f, wm->exp.nunchuk.orient.a_pitch, wm->exp.nunchuk.orient.a_roll);
}