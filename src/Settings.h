#pragma once

#include <string>

enum CustomIDs
{
    ID_VIEW_PERSP = 101,
    ID_VIEW_ORTHO,
    ID_VIEW_BOUNDING_BOX,
    ID_VIEW_BACKFACE,
    ID_VIEW_BACKGROUND_OPEN,
    ID_VIEW_BACKGROUND_VIEW,
    ID_VIEW_BACKGROUND_STRETCH,
    ID_VIEW_BACKGROUND_REPEAT,
    ID_VIEW_BACKGROUND_INTERPOLATION_LINEAR,
    ID_VIEW_BACKGROUND_INTERPOLATION_BILINEAR,
    ID_ACTION_TRANSLATE,
    ID_ACTION_SCALE,
    ID_ACTION_ROTATE,
    ID_AXIS_X,
    ID_AXIS_Y,
    ID_AXIS_Z,
    ID_SPACE_OBJECT,
    ID_SPACE_WORLD,
    ID_SPACE_VIEW,
    ID_ANIMATION_SETTINGS,
    ID_ANIMATION_RECORD,
    ID_ANIMATION_START_PLAYING,
    ID_ANIMATION_SPEED_INCREASE,
    ID_ANIMATION_SPEED_DECREASE,
    ID_ANIMATION_SPEED_NORMAL,
    ID_ANIMATION_KEYFRAMES_FIRST,
    ID_ANIMATION_KEYFRAMES_LAST,
    ID_ANIMATION_KEYFRAMES_NEXT,
    ID_ANIMATION_KEYFRAMES_PREV,
    IDC_ANIMATION_TEXTCTRL_FPS,
    IDC_ANIMATION_RADIO_LINEAR,
    IDC_ANIMATION_RADIO_BEZIER
};

#define INVALIDATE() Refresh(); Update();

class Settings
{
public:
    static double CameraFrameOffset;
    static bool IsBoundingBoxOn;
    static bool IsBoundingBoxGeo;
    static bool IsBackFaceCullingEnabled;
    static int SelectedAction;
    static bool SelectedAxis[3];
    static int SelectedSpace;
    static double MouseSensitivity[3];
    static double MinScaleFactor;
    static bool IsRecording;
    static bool IsPlayingAnimation;
    static int FramesPerSeconds;
    static bool FramesInterpolation[2]; // Linear, Bezier
    static bool IsBackgroundOn;
    static bool IsBackgroundStretched;
    static int BackgroundInterpolation;
    static std::string BackgroundImage;
};