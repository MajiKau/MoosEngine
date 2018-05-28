#include "code/headers/Geometry.h"

#include <algorithm>
Point2 operator*(const float m, const Point2 p) { return Point2(p.x * m, p.y * m); };

float DistanceSquared(Point2 point1, Point2 point2)
{
	float dx = point2.x - point1.x;
	float dy = point2.y - point1.y;
	float dist = (dx*dx) + (dy*dy);
	return dist;
}

float Distance(Point2 point1, Point2 point2)
{
	return sqrtf(DistanceSquared(point1, point2));
}

float DotPoint2(Point2 point1, Point2 point2)
{
	return (point1.x*point2.x + point1.y*point2.y);
}

float CrossPoint2(Point2 point1, Point2 point2)
{
	return (point1.x*point2.y - point1.y*point2.x);
}

Point2 CrossPoint2(Point2 point1, float point2)
{
	return Point2(point1.y*point2, -point1.x*point2);
}

float DistanceToLine(Point2 p, Line2 l)
{
	return fabsf(l.a*p.x + l.b*p.y - l.c) / sqrtf(l.a*l.a + l.b*l.b);
}

Point2 ClosestPointOnLine(Point2 p, Line2 l)
{
	float x = (l.b*(l.b*p.x - l.a*p.y) + l.a*l.c) / (l.a*l.a + l.b*l.b);
	float y = (l.a*(-l.b*p.x + l.a*p.y) + l.b*l.c) / (l.a*l.a + l.b*l.b);
	return Point2(x, y);//TODO
}

bool DoLinesIntersect(Line2 l1, Line2 l2)
{
    if (l1.a*l2.b - l2.a*l1.b == 0)
    {
        return false;
    }
    return true; //TODO: Might not work
}
Point2 LineIntersection(Line2 l1, Line2 l2)
{
   return Point2(l1.b*(-l2.c) - l2.b*(-l1.c), l2.a*(-l1.c) - l1.a*(-l2.c)); //TODO: Doesn't Work?
}

bool DoLineSegmentsIntersect(LineSegment2 l1, LineSegment2 l2)
{
	Point2 p1 = { l1.x1,l1.y1 };
	Point2 q1 = { l1.x2,l1.y2 };
	Point2 p2 = { l2.x1,l2.y1 };
	Point2 q2 = { l2.x2,l2.y2 };
	// Find the four orientations needed for general and
	// special cases
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and q2 are colinear and q2 lies on segment p1q1
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases

}
Point2 LineSegmentIntersection(LineSegment2 l1, LineSegment2 l2)
{
    float x1 = l1.x1, x2 = l1.x2, x3 = l2.x1, x4 = l2.x2;
    float y1 = l1.y1, y2 = l1.y2, y3 = l2.y1, y4 = l2.y2;

    float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if (d == 0) return Point2();

    float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
    float x = (pre * (x3 - x4) - (x1 - x2) * post) / d;
    float y = (pre * (y3 - y4) - (y1 - y2) * post) / d;

    if (x < std::min(x1, x2) || x > std::max(x1, x2) ||
        x < std::min(x3, x4) || x > std::max(x3, x4)) return Point2();
    if (y < std::min(y1, y2) || y > std::max(y1, y2) ||
        y < std::min(y3, y4) || y > std::max(y3, y4)) return Point2();

    return{ x,y };
}

float TriangleArea(const Point2 a, const Point2 b, const Point2 c)
{
	float area = fabsf((a.x*(b.y - c.y) + b.x*(c.y - a.y) + c.x*(a.y - b.y)) / 2.0f);

	return area;
}

Point2 ClosestPointOnLineSegment(Point2 p, LineSegment2 l)
{
    Point2 a = { l.x1, l.y1 };
    Point2 b = { l.x2, l.y2 };
    Point2 c = p - a;	// Vector from a to Point
    Point2 v = (b - a).Direction();	// Unit Vector from a to b
    float d = (b - a).Distance();	// Length of the line segment
    float t = DotPoint2(v, c);	// Intersection point Distance from a

                                // Check to see if the point is on the line
                                // if not then return the endpoint
    if (t < 0) return a;
    if (t > d) return b;

    // get the distance to move from point a
    v = v * t;

    // move from point a to the nearest point on the segment
    return a + v;
}

float Point2::DistanceSquared()
{
	float dist = (x*x) + (y*y);
	return dist;
}

float Point2::Distance()
{
    float dist = DistanceSquared();
    if (dist == 0.0f) return dist;
	return sqrtf(dist);
}

float Point2::DistanceToSquared(Point2 point)
{
    return (point.x - x)*(point.x - x) + (point.y - y)*(point.y - y);
}

float Point2::DistanceTo(Point2 point)
{
    return sqrtf(DistanceToSquared(point));
}

Point2 RotatePoint(Point2 point, float angle)
{
	return Point2(cosf(angle)*point.x - sinf(angle)*point.y, sinf(angle)*point.x + cosf(angle)*point.y);
}

bool onSegment(Point2 p, Point2 q, Point2 r)
{
	if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
		q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
		return true;

	return false;
}

int orientation(Point2 p, Point2 q, Point2 r)
{
	int val = (int)((q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y));

	if (val == 0) return 0;  // colinear

	return (val > 0) ? 1 : 2; // clock or counterclock wise
}