#pragma once
#include <math.h>

class Vec2
{
public:
    Vec2() :x(0), y(0) {};
    Vec2(float x, float y) { this->x = x; this->y = y; };

    Vec2 operator+=(Vec2 rhs) { this->x += rhs.x; this->y += rhs.y; return *this; };
    Vec2 operator+(Vec2 rhs) { return{ this->x + rhs.x, this->y + rhs.y }; };

    Vec2 operator-=(Vec2 rhs) { this->x -= rhs.x; this->y -= rhs.y; return *this; };
    Vec2 operator-(Vec2 rhs) { return{ this->x - rhs.x, this->y - rhs.y }; };

    Vec2 operator*=(float rhs) { this->x *= rhs; this->y *= rhs; return *this; };
    Vec2 operator*(float rhs) { return{ this->x * rhs, this->y * rhs }; };

    float Dot(Vec2 rhs) { return x*rhs.x + y*rhs.y; };
    float Cross(Vec2 rhs) { return x*rhs.y - y*rhs.x; };

    float DistanceSquared() { return x*x + y*y; };
    float Distance() { return sqrtf(DistanceSquared()); };

    float x, y;
};

class Vec3
{
public:
    Vec3() :x(0), y(0), z(0) {};
    Vec3(float x, float y, float z) { this->x = x; this->y = y; this->z = z; };

    Vec3 operator+=(Vec3 rhs) { this->x += rhs.x; this->y += rhs.y; this->z += rhs.z; return *this; };
    Vec3 operator+(Vec3 rhs) { return{ this->x + rhs.x, this->y + rhs.y, this->z + rhs.z }; };

    Vec3 operator-=(Vec3 rhs) { this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z; return *this; };
    Vec3 operator-(Vec3 rhs) { return{ this->x - rhs.x, this->y - rhs.y , this->z - rhs.z }; };

    Vec3 operator*=(float rhs) { this->x *= rhs; this->y *= rhs; this->z *= rhs; return *this; };
    Vec3 operator*(float rhs) { return{ this->x * rhs, this->y * rhs , this->z * rhs }; };

    float Dot(Vec3 rhs) { return x*rhs.x + y*rhs.y + z*rhs.z; };
    Vec3 Cross(Vec3 rhs) { return{ y*rhs.z - z*rhs.y,z*rhs.x - x*rhs.z,x*rhs.y - y*rhs.x }; };

    float DistanceSquared() { return x*x + y*y + z*z; };
    float Distance() { return sqrtf(DistanceSquared()); };

    float x, y, z;
};

class Vec4
{
public:
    Vec4() :x(0), y(0), z(0), w(0) {};
    Vec4(float x, float y, float z, float w) { this->x = x; this->y = y; this->z = z; this->w = w; };

    Vec4 operator+=(Vec4 rhs) { this->x += rhs.x; this->y += rhs.y; this->z += rhs.z; this->w += rhs.w; return *this; };
    Vec4 operator+(Vec4 rhs) { return{ this->x + rhs.x, this->y + rhs.y, this->z + rhs.z, this->w + rhs.w }; };

    Vec4 operator-=(Vec4 rhs) { this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z; this->w += rhs.w; return *this; };
    Vec4 operator-(Vec4 rhs) { return{ this->x - rhs.x, this->y - rhs.y , this->z - rhs.z,this->w - rhs.w }; };

    Vec4 operator*=(float rhs) { this->x *= rhs; this->y *= rhs; this->z *= rhs; this->w *= rhs; return *this; };
    Vec4 operator*(float rhs) { return{ this->x * rhs, this->y * rhs , this->z * rhs,this->w * rhs }; };

    Vec4 operator*(Vec4 rhs) { return{ x*rhs.x,y*rhs.y, z*rhs.z, w*rhs.w }; };

    float Dot(Vec4 rhs) { return x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w; };
    
    float DistanceSquared() { return x*x + y*y + z*z + w*w; };
    float Distance() { return sqrtf(DistanceSquared()); };

    float x, y, z, w;
};