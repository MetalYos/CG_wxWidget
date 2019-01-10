#pragma once

#include "pch.h"

struct Frame
{
    Mat4 ModelTransform;
    Mat4 CamTransform;
    int FrameNum;

    Vec4 Translation;
    Vec4 Scale;
    Vec4 Rotation;

    bool Action[3];
    bool ObjectSpace;

    Frame()
        : FrameNum(0), ObjectSpace(true)
    {
        Action[0] = Action[1] = Action[2] = true;
    }
};

class Animation
{
public:
    Animation()
        : maxFrame(-1), currentFrame(NULL) {}
    ~Animation() { ClearAnimation(); }

    void AddKeyFrame(Frame* keyFrame);
    Frame* GetFrame(int frame);
    int GetLastFrameNumber() const;

    void ResetAnimation();
    void StepToNextFrame();
    const Frame* GetCurrentFrame() const;

    void ClearAnimation();

private:
    Frame* GetFrameLinearInterpolation(Frame* before, Frame* after, int frame) const;
    Frame* GetFrameLinearInterpolation2(Frame* before, Frame* after, int frame) const;
    Frame* GetFrameBezierInterpolationPW(Frame* before, Frame* after, int frame) const;
    Frame* GetFrameBezierInterpolation(int frame) const;

private:
    std::vector<Frame*> keyFrames;
    int maxFrame;
    Frame* currentFrame;
};