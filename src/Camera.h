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
};

class Camera
{
public:
    Camera();
    ~Camera() {}

    const Mat4& GetProjection() const;
    void SetOrthographic(double left, double right, double top, double bottom,
        double near, double far);
    void SetOrthographic(double width, double aspectRatio, double near, double far);
    void SetPerspective(double left, double right, double top, double bottom,
        double near, double far);
    void SetPerspective(double fov, double aspectRatio, double near, double far);
    void LookAt(const Vec4& eye, const Vec4& at, const Vec4& up);

    const OrthographicParameters& GetOrthographicParameters() const;
    const PerspectiveParameters& GetPerspectiveParameters() const;

    bool IsPerpsective() const;
    void SwitchToProjection(bool perspective);

    void Translate(const Mat4& T);
    void Scale(const Mat4& S);
    void Rotate(const Mat4& R);

    const Mat4& GetTransform() const;

private:
    Mat4 projection;
    Mat4 orthoProjection;
    Mat4 perspProjection;
    bool isPerspective;

    Mat4 transform;

    OrthographicParameters orthoParams;
    PerspectiveParameters perspParams;
    CameraParameters camParams;
};