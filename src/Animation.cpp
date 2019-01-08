#include "Animation.h"

void Animation::AddKeyFrame(Frame* keyFrame)
{
    if (keyFrame == NULL)
        return;
    
    if (keyFrame->Frame > maxFrame)
    {
        keyFrames.push_back(keyFrame);
        maxFrame = keyFrame->Frame;
    }

    if (keyFrame->Frame == 0)
        currentFrame = GetFrame(0);
}

Frame* Animation::GetFrame(int frame)
{
    if ((frame < 0) || (frame > maxFrame) || (keyFrames.size() == 0))
        return NULL;

    Frame* frameToReturn = NULL;

    for (unsigned int i = 0; i < keyFrames.size(); i++)
    {
        if (keyFrames[i]->Frame == frame)
        {
            frameToReturn = new Frame();
            frameToReturn->ModelTransform = keyFrames[i]->ModelTransform;
            frameToReturn->CamTransform = keyFrames[i]->CamTransform;
            frameToReturn->Frame = frame;
            return frameToReturn;
        }
        else if ((keyFrames[i]->Frame < frame) && 
                (keyFrames[i + 1]->Frame > frame))
        {
            if (Settings::FramesInterpolation[0])
                frameToReturn = GetFrameLinearInterpolation(keyFrames[i], keyFrames[i + 1], frame);
            else if (Settings::FramesInterpolation[1])
            {
                // Bezier
            }
            else
            {
                // Do nothing
            }
            
            return frameToReturn;
        }
    }

    return frameToReturn;
}

int Animation::GetLastFrameNumber() const
{
    return maxFrame;
}

void Animation::ResetAnimation()
{
    if (keyFrames.size() == 0)
        return;
    
    currentFrame = GetFrame(0);
}

void Animation::StepToNextFrame()
{
    if ((keyFrames.size() == 0) || (currentFrame->Frame == maxFrame))
        return;
    
    int currentFrameNum = currentFrame->Frame;
    delete currentFrame;
    currentFrame = GetFrame(++currentFrameNum);
    LOG_TRACE("Animation::StepToNextFrame: Stepped to frame {0}", currentFrame->Frame);
}

const Frame* Animation::GetCurrentFrame() const
{
    return currentFrame;
}

void Animation::ClearAnimation()
{
    while (keyFrames.size() > 0)
    {
        Frame* frame = keyFrames.back();
        keyFrames.pop_back();
        delete frame;
    }
    currentFrame = NULL;
}

Frame* Animation::GetFrameLinearInterpolation(Frame* before, Frame* after, int frame) const
{
    Frame* result = new Frame();
    double t = (double)(frame - before->Frame) / (after->Frame - before->Frame);
    LOG_TRACE("Animation::GetFrameLinearInterpolation: t = {:03.2f}", t);

    result->ModelTransform = before->ModelTransform * (1.0 - t) + after->ModelTransform * t;
    result->CamTransform = before->CamTransform * (1.0 - t) + after->CamTransform * t;
    result->Frame = frame;

    return result;
}