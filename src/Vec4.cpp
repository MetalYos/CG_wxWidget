#include "Vec4.h"
#include "pch.h"

Vec4::Vec4(double s)
{
	for (int i = 0; i < 4; i++)
	{
		data[i] = s;
	}
}

Vec4::Vec4(double x, double y, double z, double w)
{
	data[0] = x;
	data[1] = y;
	data[2] = z;
	data[3] = w;
}

Vec4::Vec4(const Vec4 & v)
{
	for (int i = 0; i < 4; i++)
	{
		data[i] = v.data[i];
	}
}

Vec4::~Vec4()
{
}

Vec4 & Vec4::operator=(const Vec4 & v)
{
	if (this == &v)
		return *this;

	for (int i = 0; i < 4; i++)
	{
		data[i] = v.data[i];
	}
	return *this;
}

bool Vec4::operator==(const Vec4 & v)
{
	return (Distance3(*this, v) < AL_DBL_EPSILON);
}

Vec4 Vec4::operator-() const
{
	return ((*this) * (-1));
}

Vec4 Vec4::operator+(const Vec4 & v) const
{
	Vec4 result;
	for (int i = 0; i < 4; i++)
	{
		result.data[i] = data[i] + v.data[i];
	}
	return  result;
}

Vec4 & Vec4::operator+=(const Vec4 & v)
{
	return (*this = *this + v);
}

Vec4 Vec4::operator-(const Vec4 & v) const
{
	Vec4 result;
	for (int i = 0; i < 4; i++)
	{
		result.data[i] = data[i] - v.data[i];
	}
	return  result;
}

Vec4 & Vec4::operator-=(const Vec4 & v)
{
	return (*this = *this - v);
}

Vec4 Vec4::operator*(const Vec4 & v) const
{
	Vec4 result;
	for (int i = 0; i < 4; i++)
	{
		result[i] = data[i] * v.data[i];
	}
	return result;
}

Vec4 & Vec4::operator*=(const Vec4 & v)
{
	return (*this = *this * v);
}

Vec4 Vec4::operator*(double c) const
{
	Vec4 result;
	for (int i = 0; i < 4; i++)
	{
		result.data[i] = data[i] * c;
	}
	return  result;
}

Vec4 & Vec4::operator*=(double c)
{
	return (*this = *this * c);
}

Vec4 Vec4::operator*(const Mat4 & m) const
{
	Vec4 result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i] += data[j] * m[j][i];
		}
	}
	return result;
}

Vec4 & Vec4::operator*=(const Mat4 & m)
{
	return (*this = *this * m);
}

Vec4 Vec4::operator/(double c) const
{
	assert(c != 0);
	return *this * (1.0 / c);
}

Vec4 & Vec4::operator/=(double c)
{
	assert(c != 0);
	return (*this = *this / c);
}

bool Vec4::operator==(const Vec4 & v) const
{
	return (Distance3(*this, v) < AL_DBL_EPSILON);
}

const double & Vec4::operator[](int i) const
{
	assert(i >= 0 && i < 4);
	return data[i];
}

double & Vec4::operator[](int i)
{
	assert(i >= 0 && i < 4);
	return data[i];
}

double Vec4::Dot3(const Vec4 & u, const Vec4 & v)
{
	double total = 0.0;
	for (int i = 0; i < 3; i++)
	{
		total += u.data[i] * v.data[i];
	}
	return total;
}

double Vec4::Dot(const Vec4 & u, const Vec4 & v)
{
	double total = 0.0;
	for (int i = 0; i < 4; i++)
	{
		total += u.data[i] * v.data[i];
	}
	return total;
}

Vec4 Vec4::Cross(const Vec4 & u, const Vec4 & v)
{
	Vec4 result;
	result[0] = u[1] * v[2] - u[2] * v[1];
	result[1] = u[2] * v[0] - u[0] * v[2];
	result[2] = u[0] * v[1] - u[1] * v[0];
	result[3] = 0;
	return result;
}

double Vec4::Length3(const Vec4 & u)
{
	return sqrt(pow(u[0], 2) + pow(u[1], 2) + pow(u[2], 2));
}

double Vec4::Length(const Vec4 & u)
{
	return sqrt(pow(u[0], 2) + pow(u[1], 2) + pow(u[2], 2) + pow(u[3], 2));
}

Vec4 Vec4::Normalize3(const Vec4 & u)
{
	Vec4 v(u / Length3(u));
	v[3] = 0; // TODO: maybe change to 1
	return v;
}

Vec4 Vec4::Normalize(const Vec4 & u)
{
	Vec4 v = u / Length(u);
	return v;
}

double Vec4::Distance3(const Vec4 & u, const Vec4 v)
{
	return Length3(u - v);
}

double Vec4::Distance(const Vec4 & u, const Vec4 v)
{
	return Length(u - v);
}

std::ostream & operator<<(std::ostream & os, const Vec4 & v)
{
	os << "(";
	for (int i = 0; i < 4; i++)
	{
		os << v.data[i];
		if (i < 3)
			os << ", ";
	}
	os << ")";
	return os;
}
