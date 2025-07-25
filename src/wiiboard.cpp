#include "gdwiimote.h"

godot::Array GDWiimote::get_board_raw_data() const
{
    godot::Array out_weights;
    if (wm->exp.type == EXP_WII_BOARD)
    {
        out_weights.resize(4);
        auto wb = (wii_board_t *)&wm->exp.wb;
        out_weights[0] = wb->rtl;
        out_weights[1] = wb->rtr;
        out_weights[2] = wb->rbl;
        out_weights[3] = wb->rbr;
    }
    else
    {
        godot::UtilityFunctions::print("Wii Board not connected!");
    }
    return out_weights;
}

godot::Array GDWiimote::get_board_interpolated_weights() const
{
    godot::Array out_weights;
    if (wm->exp.type == EXP_WII_BOARD)
    {
        out_weights.resize(4);
        auto wb = (wii_board_t *)&wm->exp.wb;
        out_weights[0] = wb->tl;
        out_weights[1] = wb->tr;
        out_weights[2] = wb->bl;
        out_weights[3] = wb->br;
    }
    else
    {
        godot::UtilityFunctions::print("Wii Board not connected!");
    }
    return out_weights;
}
