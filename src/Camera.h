#pragma once

#include "pch.h"

struct OrthographicParameters
{
    double Left;
    double Right;
    double Top;
    double Bottom;
    double Near;
    double Far;

    OrthographicParameters()
        : Left(-5.0), Right(5.0), Top(5.0), Bottom(-5.0), Near(0.1), Far(1000.0)
    {
    }
};

struct PerspectiveParameters
{
    double Left;
    double Right;
    double Top;
    double Bottom;
    double Near;
    double Far;
    double FOV;
    double AspectRatio;

    PerspectiveParameters()
        : Near(0.1), Far(1000.0), FOV(45.0), AspectRatio(1.0)
    {
    }
};

struct CameraParameters
{
    Vec4 Eye;
    Vec4 Forward;
    Vec4 Right;
    Vec4 Up;
    Vec4 WorldUp;

    double Yaw;
    double Pitch;

    CameraParameters()
        : Yaw(-90.0), Pitch(0.0) {}
};

class Camera
{
public:
    Camera();
    ~Camera() {}

    const Mat4& GetProjection() const;
    const Mat4& GetInverseProjection() const;
    void SetOrthographic(double left, double right, double top, double bottom,
        double near, double far);
    void SetOrthographic(double width, double aspectRatio, double near, double far);
    void SetPerspective(double left, double right, double top, double bottom,
        double near, double far);
    void SetPerspective(double fov, double aspectRatio, double near, double far);
    void LookAt(const Vec4& eye, const Vec4& at, const Vec4& up);

    void RotateCamera(double yawOffset, double pitchOffset);
    void ZoomCamera(double zoomOffset);
    void PanCamera(double xOffset, double yOffset);

    const OrthographicParameters& GetOrthographicParameters() const;
    const PerspectiveParameters& GetPerspectiveParameters() const;
    const CameraParameters& GetCameraParameters() const;

    bool IsPerspective() const;
    void SwitchToProjection(bool perspective);

    const Mat4& GetWorldToViewTransform() const;

private:
    void buildInversePerspective(double left, double right, double top, double bottom,
        double near, double far);
    void buildInverseOrthographic(double left, double right, double top, double bottom,
        double near, double far);

private:
    Mat4 orthoProjection;
    Mat4 orthoInverseProjection;
    Mat4 perspProjection;
    Mat4 perspInverseProjection;
    bool isPerspective;

    Mat4 worldToView;

    OrthographicParameters orthoParams;
    PerspectiveParameters perspParams;
    CameraParameters camParams;
};