#include "Mat4.h"
#include "ALMath.h"
#include <assert.h>

// Constructors
Mat4::Mat4(double d)
{
	data[0] = Vec4(d, 0.0, 0.0, 0.0);
	data[1] = Vec4(0.0, d, 0.0, 0.0);
	data[2] = Vec4(0.0, 0.0, d, 0.0);
	data[3] = Vec4(0.0, 0.0, 0.0, d);
}

Mat4::Mat4(const Vec4 & a, const Vec4 & b, const Vec4 & c, const Vec4 & d)
{
	data[0] = a;
	data[1] = b;
	data[2] = c;
	data[3] = d;
}

Mat4::Mat4(double m00, double m01, double m02, double m03, 
		   double m10, double m11, double m12, double m13, 
		   double m20, double m21, double m22, double m23, 
		   double m30, double m31, double m32, double m33)
{
	data[0] = Vec4(m00, m01, m02, m03);
	data[1] = Vec4(m10, m11, m12, m13);
	data[2] = Vec4(m20, m21, m22, m23);
	data[3] = Vec4(m30, m31, m32, m33);
}

// Copy Constructor
Mat4::Mat4(const Mat4 & m)
{
	for (int i = 0; i < 4; i++)
	{
		data[i] = m.data[i];
	}
}

// Assignment operator overloading
Mat4 & Mat4::operator=(const Mat4 & m)
{
	if (this == &m)
		return *this;

	for (int i = 0; i < 4; i++)
	{
		data[i] = m.data[i];
	}
	return *this;
}

// Addition operator overloading
Mat4 Mat4::operator+(const Mat4 & m) const
{
	Mat4 result;
	for (int i = 0; i < 4; i++)
		result.data[i] = data[i] + m.data[i];
	return result;
}

Mat4 & Mat4::operator+=(const Mat4 & m)
{
	return (*this = *this + m);
}

// Subtraction operator overloading
Mat4 Mat4::operator-(const Mat4 & m) const
{
	Mat4 result;
	for (int i = 0; i < 4; i++)
		result.data[i] = data[i] - m.data[i];
	return result;
}

Mat4 & Mat4::operator-=(const Mat4 & m)
{
	return (*this = *this - m);
}

// Multiplication operator overloading
Mat4 Mat4::operator*(double c) const
{
	Mat4 result;
	for (int i = 0; i < 4; i++)
		result.data[i] = data[i] * c;
	return result;
}

Mat4 & Mat4::operator*=(double c)
{
	return (*this = *this * c);
}

Mat4 Mat4::operator*(const Mat4 & m) const
{
	Mat4 result(0.0);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				result[i][j] += data[i][k] * m[k][j];
			}
		}
	}
	return result;
}

Mat4 & Mat4::operator*=(const Mat4 & m)
{
	return (*this = *this * m);
}

Vec4 Mat4::operator*(const Vec4 & v) const
{
	Vec4 result;
	for (int i = 0; i < 4; i++)
	{
		result[i] = Vec4::Dot(data[i], v);
	}
	return result;
}

// Division operator overloading
Mat4 Mat4::operator/(double c) const
{
	assert(c != 0);
	return ((*this) * (1 / c));
}

Mat4 & Mat4::operator/=(double c)
{
	assert(c != 0);
	return (*this = *this / c);
}

// Subscript operator overloading
const Vec4 & Mat4::operator[](int i) const
{
	assert(i >= 0 && i < 4);
	return data[i];
}

Vec4 & Mat4::operator[](int i)
{
	assert(i >= 0 && i < 4);
	return data[i];
}

// Cout overloading
std::ostream & operator<<(std::ostream & os, const Mat4 & v)
{
	for (int i = 0; i < 4; i++)
		os << v.data[i] << std::endl;
	return os;
}

void Mat4::Transpose()
{
	std::swap(this->data[0][1], this->data[1][0]);
	std::swap(this->data[0][2], this->data[2][0]);
	std::swap(this->data[0][3], this->data[3][0]);
	std::swap(this->data[1][2], this->data[2][1]);
	std::swap(this->data[1][3], this->data[3][1]);
	std::swap(this->data[2][3], this->data[3][2]);
}

// Static Methods
Mat4 Mat4::Translate(double x, double y, double z)
{
	Mat4 result;
	result[3][0] = x;
	result[3][1] = y;
	result[3][2] = z;
	return result;
}

Mat4 Mat4::Translate(const Vec4 & v)
{
	Mat4 result;
	result[3][0] = v[0];
	result[3][1] = v[1];
	result[3][2] = v[2];
	return result;
}

Mat4 Mat4::Scale(double s)
{
	Mat4 result(s);
	result[3][3] = 1.0;
	return result;
}

Mat4 Mat4::Scale(double sx, double sy, double sz)
{
	Mat4 result;
	result[0][0] = sx;
	result[1][1] = sy;
	result[2][2] = sz;
	return result;
}

Mat4 Mat4::Scale(const Vec4 & v)
{
	Mat4 result;
	result[0][0] = v[0];
	result[1][1] = v[1];
	result[2][2] = v[2];
	return result;
}

Mat4 Mat4::RotateX(double angleInDegrees)
{
	double angleInRad = ToRadians(angleInDegrees);
	Mat4 result;
	result[1][1] = result[2][2] = cos(angleInRad);
	result[1][2] = sin(angleInRad);
	result[2][1] = -result[1][2];
	return result;
}

Mat4 Mat4::RotateY(double angleInDegrees)
{
	double angleInRad = ToRadians(angleInDegrees);
	Mat4 result;
	result[0][0] = result[2][2] = cos(angleInRad);
	result[0][2] = sin(angleInRad);
	result[2][0] = -result[0][2];
	return result;
}

Mat4 Mat4::RotateZ(double angleInDegrees)
{
	double angleInRad = ToRadians(angleInDegrees);
	Mat4 result;
	result[0][0] = result[1][1] = cos(angleInRad);
	result[0][1] = sin(angleInRad);
	result[1][0] = -result[0][1];
	return result;
}
