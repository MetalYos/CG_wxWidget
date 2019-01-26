#include "Settings.h"

double Settings::CameraFrameOffset = 1.3;
bool Settings::IsBoundingBoxOn = false;
bool Settings::IsBoundingBoxGeo = false;
bool Settings::IsBackFaceCullingEnabled = true;
int Settings::SelectedAction = ID_ACTION_SELECT;
bool Settings::SelectedAxis[3] { true, false, false };
int Settings::SelectedSpace = ID_SPACE_OBJECT;
double Settings::MouseSensitivity[3] { 100.0, 100.0, 20.0 }; // Translate, Scale, Rotate
double Settings::MinScaleFactor = 0.05;
bool Settings::IsRecording = false;
bool Settings::IsPlayingAnimation = false;
int Settings::FramesPerSeconds = 30;
bool Settings::FramesInterpolation[2] = { true, false };
bool Settings::IsBackgroundOn = false;
bool Settings::IsBackgroundStretched = true;
int Settings::BackgroundInterpolation = 0;
std::string Settings::BackgroundImage = "";