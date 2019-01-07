#pragma once

const int ID_SWITCH_TO_PERSP = 101;
const int ID_SWITCH_TO_ORTHO = 102;
const int ID_ACTION_TRANSLATE = 103;
const int ID_ACTION_SCALE = 104;
const int ID_ACTION_ROTATE = 105;
const int ID_AXIS_X = 106;
const int ID_AXIS_Y = 107;
const int ID_AXIS_Z = 108;
const int ID_SPACE_OBJECT = 109;
const int ID_SPACE_WORLD = 110;
const int ID_SPACE_VIEW = 111;


class Settings
{
public:
    static int SelectedAction;
    static bool SelectedAxis[3];
    static int SelectedSpace;
};