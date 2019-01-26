#pragma once

#include "pch.h"

#include "Vec4.h"
#include "Mat4.h"

#define AL_DBL_EPSILON 0.00000001
#define AL_PI 3.14159265359

struct Vec4Line
{
	Vec4 P1;
	Vec4 P2;

	Vec4Line(const Vec4& p1 = Vec4(0.0), const Vec4& p2 = Vec4(0.0)) 
		: P1(p1), P2(p2) {}
};

inline bool IsMinDbl(double a, double b)
{
	return a < b;
}

inline bool IsMaxDbl(double a, double b)
{
	return a >= b;
}

inline int MinDbl(int a, int b)
{
	if (a < b) return a;
	return b;
}

inline int MaxDbl(int a, int b)
{
	if (a > b) return a;
	return b;
}

inline int MinInt(int a, int b)
{
	if (a < b) return a;
	return b;
}

inline int MaxInt(int a, int b)
{
	if (a > b) return a;
	return b;
}

inline double ToRadians(double angleInDegrees)
{
	return (angleInDegrees / 180.0) * AL_PI;
}

inline double ToDegrees(double angleInRadians)
{
	return (angleInRadians * 180.0) / AL_PI;
}

inline double Factorial(int n)
{
	int product = 1;
  	for (int i = 1; i <= n; i++)
    	product *= i;
  	return product;
}

inline bool LineLineIntersection(const Vec4Line& lineA,
	const Vec4Line& lineB)
{
	// Poly points
	double x1 = lineA.P1[0];
	double y1 = lineA.P1[1];
	double x2 = lineA.P2[0];
	double y2 = lineA.P2[1];

	// Semi infinite line points
	double x3 = lineB.P1[0];
	double y3 = lineB.P1[1];
	double x4 = lineB.P2[0];
	double y4 = lineB.P2[1];

	double slope1 = (y2 - y1) / (x2 - x1);
	double slope2 = (y4 - y3) / (x4 - x3);
	if (abs(slope1 - slope2) < AL_DBL_EPSILON)
		return false;

	double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / 
		((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
	double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / 
		((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));

	if (t < 0.0 || t > 1.0 || u < 0.0 || u > 1.0)
		return false;

	double x1_ans = x1 + (x2 - x1) * t;
	double y1_ans = y1 + (y2 - y1) * t;
	double x2_ans = x3 + (x4 - x3) * u;
	double y2_ans = y3 + (y4 - y3) * u;

	Vec4 p1_ans(x1_ans, y1_ans, 0.0, 0.0);
	Vec4 p2_ans(x2_ans, y2_ans, 0.0, 0.0);

	if (Vec4::Distance3(p1_ans, p2_ans) < AL_DBL_EPSILON)
		return true;

	return false;
}

inline bool PointPolyIntersection(const std::vector<double>& point, 
	const std::vector<Vec4Line>& poly)
{
	int counter = 0;
	for (unsigned int i = 0; i < poly.size(); i++)
	{
		Vec4Line line_x;
		line_x.P1 = Vec4(point[0], point[1], 0.0);
		line_x.P2 = Vec4(100000.0, point[1], 0.0);
		
		if (LineLineIntersection(poly[i], line_x))
			counter++;
	}
	
	if (counter % 2 == 0)
		return false;
	
	return true;
}
