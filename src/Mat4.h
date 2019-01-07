#pragma once

#include "Vec4.h"

class Mat4
{
private:
	Vec4 data[4];

public:
	// Constructors
	explicit Mat4(double d = 1.0);
	Mat4(const Vec4& a, const Vec4& b, const Vec4& c, const Vec4& d);
	Mat4(double m00, double m01, double m02, double m03,
		 double m10, double m11, double m12, double m13,
		 double m20, double m21, double m22, double m23,
		 double m30, double m31, double m32, double m33);

	// Destructor
	~Mat4() { }

	// Copy Constructor
	Mat4(const Mat4& m);

	// Assignment operator overloading
	Mat4& operator =(const Mat4& m);

	// Addition operator overloading
	Mat4 operator +(const Mat4& m) const;
	Mat4& operator +=(const Mat4& m);

	// Subtraction operator overloading
	Mat4 operator -(const Mat4& m) const;
	Mat4& operator -=(const Mat4& m);

	// Multiplication operator overloading
	Mat4 operator *(double c) const;
	Mat4& operator *=(double c);
	Mat4 operator *(const Mat4& m) const;
	Mat4& operator *=(const Mat4& m);
	Vec4 operator *(const Vec4& v) const;

	// Division operator overloading
	Mat4 operator /(double c) const;
	Mat4& operator /=(double c);

	// Subscript operator overloading
	const Vec4& operator [](int i) const;
	Vec4& operator [](int i);

	// Cout overloading
	friend std::ostream& operator<<(std::ostream& os, const Mat4& v);

	// Public methods
	void Transpose();

	// Static Methods
	static Mat4 Translate(double x, double y, double z);
	static Mat4 Translate(const Vec4& v);
	static Mat4 Scale(double s);
	static Mat4 Scale(double sx, double sy, double sz);
	static Mat4 Scale(const Vec4& v);
	static Mat4 RotateX(double angleInDegrees);
	static Mat4 RotateY(double angleInDegrees);
	static Mat4 RotateZ(double angleInDegrees);
};

