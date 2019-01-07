#pragma once

#include "pch.h"

struct Frame
{
    Mat4 ModelTransform;
    Mat4 CamTransform;
    int Frame;
};

class Animation
{
public:
    Frame* GetFrame(int frame);

public:
    std::vector<Frame*> KeyFrames;
};