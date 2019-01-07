#pragma once

enum CustomIDs
{
    ID_VIEW_PERSP = 101,
    ID_VIEW_ORTHO,
    ID_VIEW_BACKFACE,
    ID_ACTION_TRANSLATE,
    ID_ACTION_SCALE,
    ID_ACTION_ROTATE,
    ID_AXIS_X,
    ID_AXIS_Y,
    ID_AXIS_Z,
    ID_SPACE_OBJECT,
    ID_SPACE_WORLD,
    ID_SPACE_VIEW
};

#define INVALIDATE() Refresh(); Update();

class Settings
{
public:
    static bool IsBackFaceCullingEnabled;
    static int SelectedAction;
    static bool SelectedAxis[3];
    static int SelectedSpace;
    static double MouseSensitivity[3];
    static double MinScaleFactor;
};