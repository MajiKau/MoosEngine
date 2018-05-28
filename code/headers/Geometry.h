#pragma once
#include <math.h>
#include <vector>


struct Point2
{
	Point2() { x = 0; y = 0; };
	Point2(float x, float y) :x(x), y(y) { };

	Point2 operator+(const Point2 rhs) { return Point2(this->x + rhs.x, this->y + rhs.y); };
	Point2 operator-(const Point2 rhs) { return Point2(this->x - rhs.x, this->y - rhs.y); };



	Point2 operator*(const float m) { return Point2(this->x * m, this->y * m); };
	Point2 operator/(const float m) { return Point2(this->x / m, this->y / m); };

	Point2 operator+=(const Point2 rhs) { this->x += rhs.x; this->y += rhs.y; return *this; }

	Point2 operator-() { return Point2(-this->x, -this->y); };

	float DistanceSquared();
	float Distance();

    float DistanceToSquared(Point2 point);
    float DistanceTo(Point2 point);

	Point2 Direction() 
    { 
        float dist = Distance();
        if (dist == 0) return Point2(0, 0);
        return ((*this) / dist); 
    };

	float x, y;
};

Point2 operator*(const float m, const Point2 p);

Point2 RotatePoint(Point2 point, float angle);

float DistanceSquared(Point2 point1, Point2 point2);

float Distance(Point2 point1, Point2 point2);

float DotPoint2(Point2 point1, Point2 point2);

float CrossPoint2(Point2 point1, Point2 point2);
Point2 CrossPoint2(Point2 point1, float point2);

struct Line2
{
	Line2(float x1 = 0, float y1 = 0, float x2 = 1, float y2 = 1)
	{
		float dy = y2 - y1;
		float dx = x2 - x1;
		if (dx == 0)
		{
			a = 1.0f;
			b = 0.0f;
			c = x1;
		}
		else
		{
			float k = dy / dx;
			a = -k;
			b = 1.0f;
			c = -k * x1 + y1;
		}
	};
	float y(float x) { return (c - a * x) / b; };
	float x(float y) { return (c - b * y) / a; };
	float slope()
	{
		if (b != 0) return a / b;
		return INFINITY;
	};
	float angle()
	{
		atan2f(b, a);//TODO Maybe wrong
	};
	bool IsPointOnLine(Point2 p)
	{
		if (a*p.x + b * p.y == c)return true;
		return false;
	};


	float a, b, c;
};

float DistanceToLine(Point2 p, Line2 l);

Point2 ClosestPointOnLine(Point2 p, Line2 l);

bool DoLinesIntersect(Line2 l1, Line2 l2);
Point2 LineIntersection(Line2 l1, Line2 l2);


float TriangleArea(Point2 a, Point2 b, Point2 c);

struct LineSegment2
{
	LineSegment2(float x1 = 0, float y1 = 0, float x2 = 1, float y2 = 1) :x1(x1), y1(y1), x2(x2), y2(y2) {};
	LineSegment2(Point2 p1, Point2 p2) :x1(p1.x), y1(p1.y), x2(p2.x), y2(p2.y) {};
	Line2 GetLine() { return Line2(x1, y1, x2, y2); };
	float x1, y1, x2, y2;
};

Point2 ClosestPointOnLineSegment(Point2 p, LineSegment2 l);

bool DoLineSegmentsIntersect(LineSegment2 l1, LineSegment2 l2);
Point2 LineSegmentIntersection(LineSegment2 l1, LineSegment2 l2);

struct Shape2
{
	Shape2() { position = { 0,0 }; points = {}; };
	void AddPoint(Point2 point) { points.push_back(point); }
	std::vector<Point2> points;
	Point2 position;
};

template<class T1>
bool Within(T1 a, T1 b, T1 c)
{
    return (a <= b  && b <= c) || (c <= b && b <= a);
}

bool onSegment(Point2 p, Point2 q, Point2 r);

int orientation(Point2 p, Point2 q, Point2 r);