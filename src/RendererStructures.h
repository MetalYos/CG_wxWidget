#pragma once

#include "pch.h"

struct Point
{
    int x;
    int y;

	Point(int x = 0, int y = 0) : x(x), y(y) {}
	explicit Point(const Vec4& p) : x((int)p[0]), y((int)p[1]) {}

    friend bool operator==(const Point& lhs, const Point& rhs)
    {
        return ((lhs.x == rhs.x) && (lhs.y == rhs.y));
    }
    
    friend bool operator!=(const Point& lhs, const Point& rhs)
    {
        return !(lhs == rhs);
    }
};

struct DVertex
{
    Point Pixel;
    Vec4 PosVS;
    Vec4 NormalVS;
    double Z;
    Vec4 Color;
};

struct Edge
{
    DVertex A;
    DVertex B;
};

class EdgeSorterY
{
public:
	bool operator()(const Edge& e1, const Edge& e2)
	{
		return (((e1.A.Pixel.y < e2.A.Pixel.y) && (e1.A.Pixel.y < e2.B.Pixel.y)) ||
			((e1.B.Pixel.y < e2.A.Pixel.y) && (e1.B.Pixel.y < e2.B.Pixel.y)));
	}
};

class EdgeComparer
{
public:
	bool operator()(const Edge& e1, const Edge& e2)
	{
		return ((e1.A.Pixel == e2.A.Pixel) && (e1.B.Pixel == e2.B.Pixel));
	}
};

struct Intersection {
	int x;
	double z;
	Vec4 color;
	Vec4 normal;
	Vec4 pos;
};

class IntersectionsSorter
{
public:
	bool operator()(const Intersection& i1, const Intersection& i2)
	{
		auto less = std::less<int>();
		return less(i1.x, i2.x);
	}
};

class IntersectionsComparer
{
public:
	bool operator()(const Intersection& i1, const Intersection& i2)
	{
		return (i1.x == i2.x);
	}
};