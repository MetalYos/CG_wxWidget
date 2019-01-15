#include "Animation.h"

void Animation::AddKeyFrame(Frame* keyFrame)
{
    if (keyFrame == NULL)
        return;
    
    if (keyFrame->FrameNum > maxFrame)
    {
        keyFrames.push_back(keyFrame);
        if (keyFrame->FrameNum > 0)
        {
            Frame* prevKF = keyFrames[keyFrames.size() - 2];
            keyFrame->OriginalFrame = prevKF->OriginalFrame + (keyFrame->FrameNum - prevKF->FrameNum);
            keyFrame->FrameNum = (int)(keyFrame->OriginalFrame * speedFactor) + keyFrames[0]->OriginalFrame;
            LOG_TRACE("Animation::AddKeyFrame: Original Frame: {0}", keyFrame->OriginalFrame);
            LOG_TRACE("Animation::AddKeyFrame: Frame: {0}", keyFrame->FrameNum);
        }
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
                frameToReturn->ObjectToWorldTransform = keyFrames[i]->ObjectToWorldTransform;
                frameToReturn->ViewTransform = keyFrames[i]->ViewTransform;
                frameToReturn->FrameNum = frame;
                break;
            }

            if ((keyFrames[i]->FrameNum < frame) && 
                (keyFrames[i + 1]->FrameNum > frame))
            {
                frameToReturn = GetFrameLinearInterpolation2(keyFrames[i], 
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

void Animation::IncreasePlaybackSpeed(double percentage)
{
    double factor = 1.0 - percentage / 100.0;
    for (unsigned int i = 1; i < keyFrames.size(); i++)
    {
        keyFrames[i]->FrameNum = (int)(keyFrames[i]->FrameNum * factor) + keyFrames[0]->OriginalFrame;
    }
    maxFrame = keyFrames[keyFrames.size() - 1]->FrameNum;
    speedFactor *= factor;
}

void Animation::DecreasePlaybackSpeed(double percentage)
{
    double factor = 1.0 + percentage / 100.0;
    for (unsigned int i = 1; i < keyFrames.size(); i++)
    {
        keyFrames[i]->FrameNum = (int)(keyFrames[i]->FrameNum * factor) + keyFrames[0]->OriginalFrame;
    }
    maxFrame = keyFrames[keyFrames.size() - 1]->FrameNum;
    speedFactor *= factor;
}

void Animation::NormalPlaybackSpeed()
{
    for (Frame* keyFrame : keyFrames)
    {
        keyFrame->FrameNum = keyFrame->OriginalFrame;
    }
    maxFrame = keyFrames[keyFrames.size() - 1]->FrameNum;
    speedFactor = 1.0;
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
    maxFrame = -1;
    speedFactor = 1.0;
}

Frame* Animation::GetFrameLinearInterpolation(Frame* before, Frame* after, int frame) const
{
    Frame* result = new Frame();
    double t = (double)(frame - before->FrameNum) / (after->FrameNum - before->FrameNum);

    result->ObjectToWorldTransform = before->ObjectToWorldTransform * (1.0 - t) + after->ObjectToWorldTransform * t;
    result->ViewTransform = before->ViewTransform * (1.0 - t) + after->ViewTransform * t;
    result->FrameNum = frame;

    return result;
}

Frame* Animation::GetFrameLinearInterpolation2(Frame* before, Frame* after, int frame) const
{
    Frame* result = new Frame();
    double t = (double)(frame - before->FrameNum) / (after->FrameNum - before->FrameNum);
    
    Mat4 ObjectToWorldTransform = before->ObjectToWorldTransform;
    Mat4 ViewTransform = before->ViewTransform;

    Vec4 transform = Vec4(0.0, 0.0, 0.0, 1.0) * (1.0 - t) + after->Translation * t;
    Vec4 scale = Vec4(1.0, 1.0, 1.0, 1.0) * (1.0 - t) + after->Scale * t;
    Vec4 rotation = Vec4(0.0, 0.0, 0.0, 1.0) * (1.0 - t) + after->Rotation * t;

    if (after->Space == ID_SPACE_OBJECT)
    {
        if (after->Action[0])
            ObjectToWorldTransform = Mat4::Translate(transform) * ObjectToWorldTransform;
        else if (after->Action[1])
            ObjectToWorldTransform = Mat4::Scale(scale) * ObjectToWorldTransform;
        else
            ObjectToWorldTransform = Mat4::RotateX(rotation[0]) * Mat4::RotateY(rotation[1]) 
                * Mat4::RotateZ(rotation[2]) * ObjectToWorldTransform;
    }
    else if (after->Space == ID_SPACE_WORLD)
    {
        if (after->Action[0])
            ObjectToWorldTransform = ObjectToWorldTransform * Mat4::Translate(transform);
        else if (after->Action[1])
            ObjectToWorldTransform = ObjectToWorldTransform * Mat4::Scale(scale);
        else
            ObjectToWorldTransform = ObjectToWorldTransform * Mat4::RotateX(rotation[0]) * Mat4::RotateY(rotation[1]) 
                * Mat4::RotateZ(rotation[2]);
    }
    else
    {
        if (after->Action[0])
            ViewTransform = Mat4::Translate(transform) * ViewTransform;
        else if (after->Action[1])
            ViewTransform = Mat4::Scale(scale) * ViewTransform;
        else
            ViewTransform = Mat4::RotateX(rotation[0]) 
                * Mat4::RotateY(rotation[1]) * Mat4::RotateZ(rotation[2]) * ViewTransform;
    }

    result->ObjectToWorldTransform = ObjectToWorldTransform;
    result->ViewTransform = ViewTransform;
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

        sumModel = sumModel + keyFrames[i]->ObjectToWorldTransform * fact;
        sumCam = sumCam + keyFrames[i]->ViewTransform * fact;
    }
    sumModel[3][3] = 1.0;
    sumCam[3][3] = 1.0;

    Frame* frame = new Frame();

    if (!isOnlyTranslation)
    {
        frame->ObjectToWorldTransform = sumModel;
        frame->ViewTransform = sumCam;
    }
    else
    {
        frame->ObjectToWorldTransform = keyFrames[0]->ObjectToWorldTransform;
        frame->ViewTransform = keyFrames[0]->ViewTransform;

        frame->ObjectToWorldTransform[3] = sumModel[3];
        frame->ViewTransform[3] = sumCam[3];
    }
    frame->FrameNum = frameNum;

    return frame;
}