#pragma once
/*#include "Vertex.h"
#include <vector>
#include "RenderFunctions.h"

extern Point2 GRAVITY;
extern float timeMultiplier;

using namespace std;

struct Force2 : Point2
{
	Force2() :Point2() {};
	Force2(float mass, Point2 acceleration) :Point2(acceleration*mass) {};
	Force2(Point2 force) :Point2(force) {};
	Force2(float x, float y) :Point2(x,y) {};
	Force2(Point2 force, Point2 position) :Point2(position.Direction()*DotPoint2(force, position.Direction())) {};
};

struct Torque2
{
	Torque2() { amount = 0; };
	Torque2(float rotationalMass, float angularAcceleration) { amount = rotationalMass * angularAcceleration; };
	Torque2(Force2 force, float lever) { amount = (force*lever).Distance(); };//Unnecessary?
	Torque2(Force2 force, Point2 position) { amount = CrossPoint2(position, force); };
	float amount;
};

struct LinearMomentum2 : Point2
{
	LinearMomentum2(Point2 momentum) :Point2(momentum) {};
	LinearMomentum2(float x = 0, float y = 0) :Point2(x, y) {};
	//LinearMomentum2() :Point2() {};
	LinearMomentum2(float mass, Point2 velocity):Point2(velocity*mass) {};
	LinearMomentum2(Force2 force, float time) :Point2(force*time) {};

	LinearMomentum2 operator+=(const LinearMomentum2 rhs) { this->x += rhs.x; this->y += rhs.y; return *this; };

	LinearMomentum2 operator-() { return LinearMomentum2(-x, -y); };

	LinearMomentum2 inv() { return LinearMomentum2(1.0f/x, 1.0f/y); };
};

struct AngularMomentum2
{
	AngularMomentum2(float angularMomentum = 0) :amount(angularMomentum) {};
	AngularMomentum2(float rotationalMass, float angularVelocity) { amount = rotationalMass * angularVelocity; };
	AngularMomentum2(float mass, Point2 velocity, Point2 position) { amount = CrossPoint2(position, LinearMomentum2(mass, velocity)); };
	AngularMomentum2(LinearMomentum2 momentum, Point2 position) { amount = CrossPoint2(position, momentum); };
	AngularMomentum2(Torque2 torque, float time) :amount(torque.amount*time) {};

	AngularMomentum2 operator+(const AngularMomentum2 rhs) { return AngularMomentum2(this->amount + rhs.amount); };
	AngularMomentum2 operator-() { return AngularMomentum2(-amount); };
	AngularMomentum2& operator+=(const AngularMomentum2& rhs) { this->amount += rhs.amount; return *this; };

	float amount;
};



struct AABB
{
	AABB(vector<Vertex> vertices)
	{
		float xmin = INFINITY;
		float ymin = INFINITY;
		float xmax = -INFINITY;
		float ymax = -INFINITY;
		for (Vertex v : vertices)
		{
			if (xmin > v.x) xmin = v.x;
			if (ymin > v.y) ymin = v.y;
			if (xmax < v.x) xmax = v.x;
			if (ymax < v.y) ymax = v.y;
		}
		xMin = xmin;
		yMin = ymin;
		xMax = xmax;
		yMax = ymax;
	};
	AABB(vector<Point2> points)
	{
		float xmin = INFINITY;
		float ymin = INFINITY;
		float xmax = -INFINITY;
		float ymax = -INFINITY;
		for (Point2 p : points)
		{
			if (xmin > p.x) xmin = p.x;
			if (ymin > p.y) ymin = p.y;
			if (xmax < p.x) xmax = p.x;
			if (ymax < p.y) ymax = p.y;
		}
		xMin = xmin;
		yMin = ymin;
		xMax = xmax;
		yMax = ymax;
	};
	float xMin, yMin, xMax, yMax;
};

class Box2D
{
public:
	//Rectangle2D() :_x(0), _y(0), _width(1), _height(1), _angle(0) {};
	Box2D(float x = 0.0f, float y = 0.0f, float w = 1.0f, float h = 1.0f);
	
	vector<Vertex> GetVertices();
	vector<LineSegment2> GetSides();

	void SetPosition(float xPos, float yPos) { x = xPos; y = yPos; };
	void SetPosition(Point2 p) { x = p.x; y = p.y; };
	Point2 GetPosition() { return Point2(x, y); };
	void Translate(float dX, float dY)
	{
		if (isStatic)return;
		x += dX; y += dY;
	};
	void Translate(Point2 dP) { x += dP.x; y += dP.y; };

	void SetSize(float w, float h) { width = w; height = h; };
	Point2 GetSize() { return Point2(width, height); };

	void SetMass(float m)
	{
		mass = m;
		angularMass = (mass / 12.0f)*(height*height + width * width);
	};
	float GetMass() { return mass; };

	void SetRotation(float a) { angle = a; };
	float GetRotation() { return angle; };
	void Rotate(float a) { angle += a; };
	void SetRotationSpeed(float s) { rotationSpeed = s; };
	float GetRotationSpeed() { return rotationSpeed; };

	void SetLinearVelocity(float xVel, float yVel) { velX = xVel; velY = yVel; };
	void SetLinearVelocity(Point2 newVel) { velX = newVel.x; velY = newVel.y; };
	Point2 GetLinearVelocity() { return Point2(velX, velY); };

	Point2 GetVelocityAt(Point2 position) { return -CrossPoint2(position - GetPosition(), rotationSpeed)+GetLinearVelocity(); };
	float GetRotationSpeedAt(Point2 position) { return DotPoint2(CrossPoint2(position, 1).Direction(), GetVelocityAt(position)); };
	float GetAngularMass() { return angularMass; };

	AABB GetAABB() { return AABB(GetVertices()); };

	void AddForce(Force2 force) { forceX += force.x; forceY += force.y; };
	Force2 GetForce() { return Force2(forceX, forceY); };

	void AddTorque(Torque2 t) { torque += t.amount; };
	float GetTorque() { return torque; };

	void AddForceAt(Force2 force, Point2 position) 
	{ 
		AddForce(Force2  (force, position - GetPosition()));
		AddTorque(Torque2(force, position - GetPosition()));
	}
	void AddImpulseAt(Force2 force, Point2 position, float time)
	{
		AddLinearMomentum(LinearMomentum2(Force2(force, position), time));
		AddAngularMomentum(AngularMomentum2(Torque2(force, position), time));
	}

	void AddLinearMomentum(LinearMomentum2 momentum) { velX += momentum.x / mass; velY += momentum.y / mass; };
	void AddLinearMomentum(Force2 f, float time) { velX += time * f.x / mass; velY += time * f.y / mass; };
	void AddAngularMomentum(AngularMomentum2 momentum) { rotationSpeed += momentum.amount / angularMass; };
	void AddAngularMomentum(Torque2 t, float time) { rotationSpeed += time * t.amount / angularMass; };

	LinearMomentum2 GetLinearMomentum() { return LinearMomentum2(mass, GetLinearVelocity()); };
	AngularMomentum2 GetAngularMomentum() { return AngularMomentum2(angularMass, rotationSpeed); };
	AngularMomentum2 GetAngularMomentumAroundOrigin() { return AngularMomentum2(angularMass, rotationSpeed) + AngularMomentum2(GetLinearMomentum(), GetPosition()); };

	void SetStatic(bool setStatic)
	{
		SetMass(10000000);
		isStatic = setStatic;
	}

	

	void Render()
	{



		glBegin(GL_TRIANGLE_FAN);

		vector<Vertex> vertices = GetVertices();

		for (Vertex v : vertices)
		{
			//printf("Vertex at: X:%.2f, Y:%.2f\n", v.x, v.y);

			//glTexCoord2f(x, y);
			glVertex3f(v.x, v.y, v.z);
			//glColor4f(1, 1, 1, 1);

		}
		glEnd();


		RenderMomentums();


		//RenderArrow(LineSegment2(GetPosition(), GetPosition() + GetForce()/mass), RED);

	}
	void RenderMomentums()
	{
		//RenderArrow(LineSegment2(GetPosition(), GetPosition() + GetLinearMomentum()/mass), YELLOW);
		//RenderCircleArrow(GetPosition(), 2, PI / 2, GetAngularMomentum().amount / angularMass + PI / 2, YELLOW);
	}
	void RenderForce()
	{

	}

	void Update(float dt)
	{
		if (isStatic)return;
		dt *= timeMultiplier;

		x += velX * dt;
		y += velY * dt;
		angle += rotationSpeed * dt;

		velX += dt * forceX / mass;
		velY += dt * forceY / mass;

		forceX = 0;
		forceY = 0;

		rotationSpeed += dt * torque / angularMass;
		if (fabsf(rotationSpeed) > 500)rotationSpeed = 0;
		torque = 0;

		//velY += dt * GRAVITY;

		AddForce(GRAVITY*mass);

		//printf("Updating Rigidbody: X:%.2f, Y:%.2f\n", x, y);
	}

	float x, y, width, height, mass, velX, velY, angle, rotationSpeed;
	float forceX, forceY, torque, angularMass;
	bool isStatic;
	GLuint texture;
	float elasticity;
}; 



enum Side
{
	TOP,
	BOTTOM,
	LEFT,
	RIGHT
};

struct CollisionData
{
	//Which side of the object the collision happened on?
	Side side;
	//At what world coordinate did the collision happen?
	Point2 point;
	//How deep was the penetration?
	float distance;
	//At what angle was the object?
	float angle;
};

bool PointInAABB(Point2 point, AABB box);
bool AABBInAABB(AABB box1, AABB box2);
CollisionData* PointInBox2D(Point2 p, Box2D* box, Point2 pVel);
vector<CollisionData*>* Box2DInBox2D(Box2D* box1, Box2D* box2);
void HandleCollisions(vector<Box2D*> colliders);*/
