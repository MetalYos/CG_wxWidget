#include "Animation.h"

void Animation::AddKeyFrame(Frame* keyFrame)
{
    if (keyFrame == NULL)
        return;
    
    if (keyFrame->FrameNum > maxFrame)
    {
        keyFrames.push_back(keyFrame);
        maxFrame = keyFrame->FrameNum;
    }

    if (keyFrame->FrameNum == 0)
        currentFrame = GetFrame(0);
}

Frame* Animation::GetFrame(int frame)
{
    if ((frame < 0) || (frame > maxFrame) || (keyFrames.size() == 0))
        return NULL;

    Frame* frameToReturn = NULL;

    if (Settings::FramesInterpolation[0])
    {
        for (unsigned int i = 0; i < keyFrames.size(); i++)
        {
            if (keyFrames[i]->FrameNum == frame)
            {
                frameToReturn = new Frame();
                frameToReturn->ModelTransform = keyFrames[i]->ModelTransform;
                frameToReturn->CamTransform = keyFrames[i]->CamTransform;
                frameToReturn->FrameNum = frame;
                break;
            }

            if ((keyFrames[i]->FrameNum < frame) && 
                (keyFrames[i + 1]->FrameNum > frame))
            {
                frameToReturn = GetFrameLinearInterpolation(keyFrames[i], 
                    keyFrames[i + 1], frame);
                break;
            }
        }
    }
    else if (Settings::FramesInterpolation[1])
    {
        frameToReturn = GetFrameBezierInterpolation(frame);
    }
    else
    {
        // Piecewise Bezier
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
    if ((keyFrames.size() == 0) || (currentFrame->FrameNum == maxFrame))
        return;
    
    int currentFrameNum = currentFrame->FrameNum;
    delete currentFrame;
    currentFrame = GetFrame(++currentFrameNum);
    LOG_TRACE("Animation::StepToNextFrame: Stepped to frame {0}", currentFrame->FrameNum);
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
    double t = (double)(frame - before->FrameNum) / (after->FrameNum - before->FrameNum);
    LOG_TRACE("Animation::GetFrameLinearInterpolation: t = {:03.2f}", t);

    result->ModelTransform = before->ModelTransform * (1.0 - t) + after->ModelTransform * t;
    result->CamTransform = before->CamTransform * (1.0 - t) + after->CamTransform * t;
    result->FrameNum = frame;

    return result;
}

Frame* Animation::GetFrameBezierInterpolationPW(Frame* before, Frame* after, int frame) const
{
    return NULL;
}

Frame* Animation::GetFrameBezierInterpolation(int frameNum) const
{
    double t = frameNum / (double)maxFrame;
    Mat4 sumModel(0.0);
    Mat4 sumCam(0.0);
    int n = keyFrames.size() - 1;

    for (unsigned int i = 0; i < keyFrames.size(); i++)
    {
        double temp1 = Factorial(n);
        double temp2 = pow(1.0 - t, n - i);
        double temp3 = pow(t, i);
        double temp4 = Factorial(i) * Factorial(n - i);

        double fact = (temp1 * temp2 * temp3) / temp4;

        sumModel = sumModel + keyFrames[i]->ModelTransform * fact;
        sumCam = sumCam + keyFrames[i]->CamTransform * fact;
    }
    sumModel[3][3] = 1.0;
    sumCam[3][3] = 1.0;

    Frame* frame = new Frame();
    frame->ModelTransform = sumModel;
    frame->CamTransform = sumCam;
    frame->FrameNum = frameNum;

    return frame;
}