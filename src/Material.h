#pragma once

#include "pch.h"

struct Material
{
    Vec4 Color;
    Vec4 Ka;
    Vec4 Kd;
    Vec4 Ks;
    double Specular;

    Material()
        : Color(255, 255, 255), Ka(0.2, 0.2, 0.2),
        Kd(0.8, 0.8, 0.8), Ks(1.0, 1.0, 1.0), Specular(1.0)
    {
    }
};