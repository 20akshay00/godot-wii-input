#include "gdwiimote.h"

bool GDWiimote::is_ir_active()
{
    return WIIUSE_USING_IR(wm);
}

void GDWiimote::set_ir(bool enable)
{
    if (wm)
        wiiuse_set_ir(wm, enable ? 1 : 0);
}

float GDWiimote::get_ir_cursor_distance()
{
    if (!(wm && WIIUSE_USING_IR(wm)))
    {
        return -1.;
    }

    return wm->ir.z;
}

godot::Vector2 GDWiimote::get_ir_cursor_calculated_position()
{
    if (!(wm && WIIUSE_USING_IR(wm)))
    {
        return godot::Vector2();
    }

    return godot::Vector2(wm->ir.x, wm->ir.y);
}

godot::Vector2 GDWiimote::get_ir_cursor_absolute_coordinate()
{
    if (!(wm && WIIUSE_USING_IR(wm)))
    {
        return godot::Vector2();
    }

    return godot::Vector2(wm->ir.ax, wm->ir.ay);
}

int GDWiimote::get_ir_num_dots()
{
    if (!(wm && WIIUSE_USING_IR(wm)))
    {
        return -1;
    }

    return wm->ir.num_dots;
}

godot::Vector2 GDWiimote::get_ir_dot_raw_position(int index)
{
    if (!(wm && WIIUSE_USING_IR(wm) && (index >= 0) && (index < 4)))
    {
        return godot::Vector2();
    }

    return godot::Vector2(wm->ir.dot[index].rx, wm->ir.dot[index].ry);
}

godot::Vector2 GDWiimote::get_ir_dot_interpolated_position(int index)
{
    if (!(wm && WIIUSE_USING_IR(wm) && (index >= 0) && (index < 4)))
    {
        return godot::Vector2();
    }

    return godot::Vector2(wm->ir.dot[index].x, wm->ir.dot[index].y);
}

float GDWiimote::get_ir_dot_size(int index)
{

    if (!(wm && WIIUSE_USING_IR(wm) && (index >= 0) && (index < 4)))
    {
        return -1;
    }

    return wm->ir.dot[index].size;
}

bool GDWiimote::is_ir_dot_visible(int index)
{

    if (!(wm && WIIUSE_USING_IR(wm) && (index >= 0) && (index < 4)))
    {
        return false;
    }

    return wm->ir.dot[index].visible;
}
