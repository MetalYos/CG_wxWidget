#pragma once

#include "pch.h"

struct Frame
{
public:
    Mat4 ObjectToWorldTransform;
    Mat4 ViewTransform;
    int FrameNum;
    int OriginalFrame;

    Vec4 Translation;
    Vec4 Scale;
    Vec4 Rotation;

    bool Action[3];
    int Space;

    Frame()
        : FrameNum(0), OriginalFrame(0), Space(ID_SPACE_OBJECT)
    {
        Action[0] = Action[1] = Action[2] = true;
    }
};

class Animation
{
public:
    Animation()
        : maxFrame(-1), currentFrame(NULL), speedFactor(1.0), 
        isOnlyTranslation(false) {}
    ~Animation() { ClearAnimation(); }

    void AddKeyFrame(Frame* keyFrame);
    Frame* GetFrame(int frame);
    int GetLastFrameNumber() const;

    void ResetAnimation();
    void StepToNextFrame();
    const Frame* GetCurrentFrame() const;

    void IncreasePlaybackSpeed(double percentage);
    void DecreasePlaybackSpeed(double percentage);
    void NormalPlaybackSpeed();

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
    double speedFactor;
    bool isOnlyTranslation;
};