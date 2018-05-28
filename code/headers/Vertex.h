#pragma once
/*#include "Geometry.h"

struct Vertex
{
	Vertex() :x(0), y(0), z(0) {};
	Vertex(float x, float y, float z) :x(x), y(y), z(z) {};

	Vertex& operator-=(const Vertex& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}
	Vertex& operator+=(const Vertex& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	Point2 point()
	{
		return Point2(x, y);
	}

	float x, y, z;
};

Vertex operator+(const Vertex& lhs, const Vertex& rhs);

Vertex RotateVertex(Vertex vertex, float angle);*/