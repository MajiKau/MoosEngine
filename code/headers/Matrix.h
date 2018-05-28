#pragma once
#include "Vector.h"

struct Mat2
{
    Mat2() {};
    Mat2(Vec2 vec1, Vec2 vec2) : v1(vec1), v2(vec2) {};

    float Det() { return v1.x*v2.y - v1.y*v2.x; };

    Vec2 v1, v2;
};

struct Mat3
{
    Mat3() {};
    Mat3(Vec3 vec1, Vec3 vec2, Vec3 vec3) : v1(vec1), v2(vec2), v3(vec3) {};

    float Det() 
    { 
        return v1.x*Mat2({ v2.y, v2.z }, { v3.y, v3.z }).Det()
            - v1.y*Mat2({ v2.x, v2.z }, { v3.x, v3.z }).Det()
            + v1.z*Mat2({ v2.x, v2.y }, { v3.x, v3.y }).Det();
    };

    Vec3 v1, v2, v3;
};

struct Mat4
{
    Mat4() {};
    Mat4(Vec4 vec1, Vec4 vec2, Vec4 vec3, Vec4 vec4) : v1(vec1), v2(vec2), v3(vec3), v4(vec4) {};

    Mat4 operator*(float rhs) { return Mat4(v1*rhs, v2*rhs, v3*rhs, v4*rhs); };

    Mat4 operator*(Mat4 rhs)
    {
        return Mat4(
            *this*Vec4(rhs.v1.x, rhs.v2.x, rhs.v3.x, rhs.v4.x),
            *this*Vec4(rhs.v1.y, rhs.v2.y, rhs.v3.y, rhs.v4.y),
            *this*Vec4(rhs.v1.z, rhs.v2.z, rhs.v3.z, rhs.v4.z),
            *this*Vec4(rhs.v1.w, rhs.v2.w, rhs.v3.w, rhs.v4.w)
            ).Transpose();//TODO: FIX?
    }

    Vec4 operator*(Vec4 rhs)
    {
        return Vec4(
            rhs.x*v1.x + rhs.y*v2.x + rhs.z*v3.x + rhs.w*v4.x,
            rhs.x*v1.y + rhs.y*v2.y + rhs.z*v3.y + rhs.w*v4.y,
            rhs.x*v1.z + rhs.y*v2.z + rhs.z*v3.z + rhs.w*v4.z,
            rhs.x*v1.w + rhs.y*v2.w + rhs.z*v3.w + rhs.w*v4.w);
    }//TODO: FIX?

    float Det() 
    {
        return v1.x*Mat3({ v2.y, v2.z,v2.w }, { v3.y, v3.z,v3.w }, { v4.y, v4.z,v4.w }).Det()
            - v1.y*Mat3({ v2.x, v2.z,v2.w }, { v3.x, v3.z,v3.w }, { v4.x, v4.z,v4.w }).Det()
            + v1.z*Mat3({ v2.x, v2.y,v2.w }, { v3.x, v3.y,v3.w }, { v4.x, v4.y,v4.w }).Det()
            - v1.w*Mat3({ v2.x, v2.y,v2.z }, { v3.x, v3.y,v3.z }, { v4.x, v4.y,v4.z }).Det();
    };

    Mat4 Transpose() { return Mat4({ v1.x,v2.x,v3.x,v4.x }, { v1.y,v2.y,v3.y,v4.y }, { v1.z,v2.z,v3.z,v4.z }, { v1.w,v2.w,v3.w,v4.w }); };

    float Cofactor(int i) 
    { 
        int x = i % 4;
        int y = i / 4;
        Mat3 res;
        if (x == 0)
        {
            if (y == 0)
            {
                res = Mat3({v2.y,v2.z, v2.w }, { v3.y,v3.z, v3.w }, { v4.y,v4.z, v4.w });
            }
            else if (y == 1)
            {
                res = Mat3({ v1.y,v1.z, v1.w }, { v3.y,v3.z, v3.w }, { v4.y,v4.z, v4.w });
            }
            else if (y == 2)
            {
                res = Mat3({ v1.y,v1.z, v1.w }, { v2.y,v2.z, v2.w }, { v4.y,v4.z, v4.w });
            }
            else if (y == 3)
            {
                res = Mat3({ v1.y,v1.z, v1.w }, { v2.y,v2.z, v2.w }, { v3.y,v3.z, v3.w });
            }
        }
        else  if (x == 1)
        {
            if (y == 0)
            {
                res = Mat3({ v2.x,v2.z, v2.w }, { v3.x,v3.z, v3.w }, { v4.x,v4.z, v4.w });
            }
            else if (y == 1)
            {
                res = Mat3({ v1.x,v1.z, v1.w }, { v3.x,v3.z, v3.w }, { v4.x,v4.z, v4.w });
            }
            else if (y == 2)
            {
                res = Mat3({ v1.x,v1.z, v1.w }, { v2.x,v2.z, v2.w }, { v4.x,v4.z, v4.w });
            }
            else if (y == 3)
            {
                res = Mat3({ v1.x,v1.z, v1.w }, { v2.x,v2.z, v2.w }, { v3.x,v3.z, v3.w });
            }
        }
        else  if (x == 2)
        {
            if (y == 0)
            {
                res = Mat3({ v2.x,v2.y, v2.w }, { v3.x,v3.y, v3.w }, { v4.x,v4.y, v4.w });
            }
            else if (y == 1)
            {
                res = Mat3({ v1.x,v1.y, v1.w }, { v3.x,v3.y, v3.w }, { v4.x,v4.y, v4.w });
            }
            else if (y == 2)
            {
                res = Mat3({ v1.x,v1.y, v1.w }, { v2.x,v2.y, v2.w }, { v4.x,v4.y, v4.w });
            }
            else if (y == 3)
            {
                res = Mat3({ v1.x,v1.y, v1.w }, { v2.x,v2.y, v2.w }, { v3.x,v3.y, v3.w });
            }
        }
        else  if (x == 3)
        {
            if (y == 0)
            {
                res = Mat3({ v2.x,v2.y, v2.z }, { v3.x,v3.y, v3.z }, { v4.x,v4.y, v4.z });
            }
            else if (y == 1)
            {
                res = Mat3({ v1.x,v1.y, v1.z }, { v3.x,v3.y, v3.z }, { v4.x,v4.y, v4.z });
            }
            else if (y == 2)
            {
                res = Mat3({ v1.x,v1.y, v1.z }, { v2.x,v2.y, v2.z }, { v4.x,v4.y, v4.z });
            }
            else if (y == 3)
            {
                res = Mat3({ v1.x,v1.y, v1.z }, { v2.x,v2.y, v2.z }, { v3.x,v3.y, v3.z });
            }
        }

        return res.Det(); 
    };

    Mat4 Adjugate() 
    {
        return Mat4(
        { Cofactor(0),Cofactor(1),Cofactor(2),Cofactor(3) },
        { Cofactor(4),Cofactor(5),Cofactor(6),Cofactor(7) },
        { Cofactor(8),Cofactor(9),Cofactor(10),Cofactor(11) },
        { Cofactor(12),Cofactor(13),Cofactor(14),Cofactor(5) });
    };

    Mat4 Inverse() { return Adjugate()*(1.0f / Det()); }

    static Mat4 RotationX(float a)
    {
        return Mat4(
        { 1, 0,      0,       0 },
        { 0, cosf(a),-sinf(a),0 },
        { 0, sinf(a),cosf(a), 0 }, 
        { 0, 0,      0,       1 }
        );
    }
    static Mat4 RotationY(float a)
    {
        return Mat4(
        { cosf(a), 0,        sinf(a),0 },
        { 0,       1,        0,      0 },
        { -sinf(a),0,        cosf(a),0 },
        { 0,       0,        0,      1 }
        );
    }
    static Mat4 RotationZ(float a)
    {
        return Mat4(
        { cosf(a),-sinf(a),0,0 },
        { sinf(a),cosf(a), 0,0 },
        { 0,      0,       1,0 },
        { 0,      0,       0,1 }
        );
    }

    Vec4 v1, v2, v3, v4;
};