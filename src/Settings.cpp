#include "Settings.h"

bool Settings::IsBackFaceCullingEnabled = true;
int Settings::SelectedAction = ID_ACTION_TRANSLATE;
bool Settings::SelectedAxis[3] { true, false, false };
int Settings::SelectedSpace = ID_SPACE_OBJECT;
double Settings::MouseSensitivity[3] { 100.0, 100.0, 100.0 };
double Settings::MinScaleFactor = 0.05;