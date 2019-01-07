#pragma once

#include <iostream>

// Forward decleration of Mat4 class
class Mat4;

class Vec4
{
private:
	double data[4];

public:
	// Constructors
	explicit Vec4(double s = 0.0);
	Vec4(double x, double y, double z, double w = 1.0);

	// Destructor
	~Vec4();

	// Copy constructor
	Vec4(const Vec4& v);

	// Assignment operator overloading
	Vec4& operator =(const Vec4& v);

	// Equals operator overloading
	bool operator ==(const Vec4& v);

	// Negation operator overloading
	Vec4 operator -() const;

	// Addition operator overloading
	Vec4 operator +(const Vec4& v) const;
	Vec4& operator +=(const Vec4& v);

	// Subtraction operator overloading
	Vec4 operator -(const Vec4& v) const;
	Vec4& operator -=(const Vec4& v);

	// Multiplication with Vec4 overloading
	// Multiplication with scalar operator overloading
	Vec4 operator *(const Vec4& v) const;
	Vec4& operator *=(const Vec4& v);

	// Multiplication with scalar operator overloading
	Vec4 operator *(double c) const;
	Vec4& operator *=(double c);

	// Multiplication with Mat4 operator overloading
	Vec4 operator *(const Mat4& m) const;
	Vec4& operator *=(const Mat4& m);

	// Division with scalar operator overloading
	Vec4 operator /(double c) const;
	Vec4& operator /=(double c);

	bool operator ==(const Vec4& v) const;

	// Subscript operator overloading
	const double& operator [](int i) const;
	double& operator [](int i);

	// Cout overloading
	friend std::ostream& operator<<(std::ostream& os, const Vec4& v);

	// Static methods
	static double Dot3(const Vec4& u, const Vec4& v);
	static double Dot(const Vec4& u, const Vec4& v);
	static Vec4 Cross(const Vec4& u, const Vec4& v);
	static double Length3(const Vec4& u);
	static double Length(const Vec4& u);
	static Vec4 Normalize3(const Vec4& u);
	static Vec4 Normalize(const Vec4& u);
	static double Distance3(const Vec4& u, const Vec4 v);
	static double Distance(const Vec4& u, const Vec4 v);
};

