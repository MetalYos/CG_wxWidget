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
    Animation()
        : maxFrame(0) {}
    ~Animation() { ClearAnimation(); }

    void AddKeyFrame(Frame* keyFrame);
    Frame* GetFrame(int frame);

    void ClearAnimation();

private:
    Frame* GetFrameLinearInterpolation(Frame* before, Frame* after, int frame) const;

private:
    std::vector<Frame*> keyFrames;
    int maxFrame;
};