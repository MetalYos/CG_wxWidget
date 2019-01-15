#include "Camera.h"

Camera::Camera()
    : isPerspective(false)
{

}

const Mat4& Camera::GetProjection() const
{
    return projection;
}

void Camera::SetOrthographic(double left, double right, double top, double bottom,
    double near, double far)
{
    // Create OpenGL Orthographic Matrix
    Mat4 result;
    result[0][0] = 2.0 / (right - left);
    result[1][1] = 2.0 / (top - bottom);
    result[2][2] = -2.0 / (far - near);
    result[0][3] = -(right + left) / (right - left);
    result[1][3] = -(top + bottom) / (top - bottom);
    result[2][3] = -(far + near) / (far - near);

    // Transform matrix to be row major
    result.Transpose();

    // Save parameters
    orthoParams.Left = left;
    orthoParams.Right = right;
    orthoParams.Top = top;
    orthoParams.Bottom = bottom;
    orthoParams.Near = near;
    orthoParams.Far = far;

    // Set projection to be the newly created matrix
    orthoProjection = result;
    projection = result;
    isPerspective = false;
}

void Camera::SetOrthographic(double width, double aspectRatio, double near, double far)
{
    // Calculate height
    double height = width / aspectRatio;

    // Set Orthographic using the regular matrix
    SetOrthographic(-width / 2.0, width / 2.0, height / 2.0, -height / 2.0,
        near, far);
}

void Camera::SetPerspective(double left, double right, double top, double bottom,
    double near, double far)
{
    // Create OpenGL Orthographic Matrix
    Mat4 result;
    result[0][0] = (2.0 * near) / (right - left);
    result[1][1] = (2.0 * near) / (top - bottom);
    result[2][2] = -(far + near) / (far - near);
    result[3][3] = 0.0;
    result[0][2] = (right + left) / (right - left);
    result[1][2] = (top + bottom) / (top - bottom);
    result[3][2] = -1.0;

    // Transform matrix to be row major
    result.Transpose();

    // Save parameters
    perspParams.Left = left;
    perspParams.Right = right;
    perspParams.Top = top;
    perspParams.Bottom = bottom;
    perspParams.Near = near;
    perspParams.Far = far;
    perspParams.FOV = 2.0 * atan(top / near);
    perspParams.AspectRatio = abs((right - left) / (top - bottom));

    // Set projection to be the newly created matrix
    perspProjection = result;
    projection = result;
    isPerspective = true;
}

void Camera::SetPerspective(double fov, double aspectRatio, double near, double far)
{
    // Calculate Top and Right
    double top = tan(ToRadians(fov) / 2.0) * near;
    double right = top * aspectRatio;

    // Set Perspective using the regular matrix
    SetPerspective(-right, right, top, -top, near, far);

    // Update relevant parameters
    perspParams.FOV = fov;
    perspParams.AspectRatio = aspectRatio;
}

void Camera::LookAt(const Vec4& eye, const Vec4& at, const Vec4& up)
{
    camParams.Eye = eye;

    // Calculate Forward normal vector
    Vec4 forward = Vec4::Normalize3(eye - at);
    camParams.Forward = forward;
    
    // Calculate Side normal vector
    Vec4::Normalize3(up);
    Vec4 right = Vec4::Normalize3(Vec4::Cross(up, forward));
    camParams.Right = right;

    // Calculate real Up normal vector
    Vec4 upReal = Vec4::Normalize3(Vec4::Cross(forward, right));
    camParams.Up = upReal;

    Mat4 rotate;
    rotate[0] = right;
    rotate[1] = upReal;
    rotate[2] = forward;

    Mat4 translate = Mat4::Translate(-eye);

    worldToView = translate * rotate;
}

const OrthographicParameters& Camera::GetOrthographicParameters() const
{
    return orthoParams;
}

const PerspectiveParameters& Camera::GetPerspectiveParameters() const
{
    return perspParams;
}

bool Camera::IsPerspective() const
{
    return isPerspective;
}

void Camera::SwitchToProjection(bool perspective)
{
    if (perspective && !isPerspective)
    {
        projection = perspProjection;
        isPerspective = true;
        LOG_TRACE("Camera::SwitchToProjection: Switched to Perspective Projection");
    }
    if(!perspective && isPerspective)
    {
        projection = orthoProjection;
        isPerspective = false;
        LOG_TRACE("Camera::SwitchToProjection: Switched to Orthographic Projection");
    }
}

const Mat4& Camera::GetWorldToViewTransform() const
{
    return worldToView;
}
