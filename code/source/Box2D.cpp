#include "Box2D.h"
/*
Point2 GRAVITY = Point2(0.0f,-10.0f);
float timeMultiplier = 1.0f;

Box2D::Box2D(float x, float y, float w, float h)
	:x(x), y(y), width(w), height(h), angle(0), mass(1), velX(0), velY(0), rotationSpeed(0), forceX(0), forceY(0), torque(0)
{
	angularMass = (mass / 12.0f)*(height*height + width * width);
	isStatic = false;

	//texture = LoadTexture("test.bmp");//TODO: REMOVE
	elasticity = 0.5f;
};

vector<Vertex> Box2D::GetVertices()
{
	vector<Vertex> vertices;
	Vertex pos = Vertex(x, y, 0);
	Vertex vert = Vertex(-width / 2.0f, -height / 2.0f, 0);
	vert = RotateVertex(vert, angle);
	vert += pos;
	vertices.push_back(vert);
	vert = Vertex(+width / 2.0f, -height / 2.0f, 0);
	vert = RotateVertex(vert, angle);
	vert += pos;
	vertices.push_back(vert);
	vert = Vertex(+width / 2.0f, +height / 2.0f, 0);
	vert = RotateVertex(vert, angle);
	vert += pos;
	vertices.push_back(vert);
	vert = Vertex(-width / 2.0f, +height / 2.0f, 0);
	vert = RotateVertex(vert, angle);
	vert += pos;
	vertices.push_back(vert);
	return vertices;
}

vector<LineSegment2> Box2D::GetSides() {
	vector<LineSegment2> sides(4);
	vector<Vertex> vertices = GetVertices();
	Vertex bl = vertices[0];
	Vertex br = vertices[1];
	Vertex tr = vertices[2];
	Vertex tl = vertices[3];
	LineSegment2 top = LineSegment2(tl.x, tl.y, tr.x, tr.y);
	LineSegment2 bottom = LineSegment2(bl.x, bl.y, br.x, br.y);
	LineSegment2 left = LineSegment2(tl.x, tl.y, bl.x, bl.y);
	LineSegment2 right = LineSegment2(br.x, br.y, tr.x, tr.y);
	sides[TOP] = top;
	sides[BOTTOM] = bottom;
	sides[LEFT] = left;
	sides[RIGHT] = right;
	return sides;
};

bool PointInAABB(Point2 point, AABB box)
{
	if (point.x<box.xMin)
		return false;
	if (point.x>box.xMax)
		return false;
	if (point.y<box.yMin)
		return false;
	if (point.y>box.yMax)
		return false;
	return true;
}

bool AABBInAABB(AABB box1, AABB box2)
{
	Point2 p;
	p.x = box1.xMin;
	p.y = box1.yMin;
	if (PointInAABB(p, box2))
		return true;
	p.x = box1.xMax;
	p.y = box1.yMin;
	if (PointInAABB(p, box2))
		return true;
	p.x = box1.xMax;
	p.y = box1.yMax;
	if (PointInAABB(p, box2))
		return true;
	p.x = box1.xMin;
	p.y = box1.yMax;
	if (PointInAABB(p, box2))
		return true;

	return false;
}

CollisionData* PointInBox2D(Point2 p, Box2D* box, Point2 pVel)
{
	float boxArea = box->width*box->height;

	float pointArea = 0;

	vector<Vertex> vertices = box->GetVertices();

	Point2 vel = box->GetVelocityAt(p) - pVel;
	RotatePoint(vel, box->angle);

	float bottom = TriangleArea(p, vertices[0].point(), vertices[1].point());
	float right = TriangleArea(p, vertices[1].point(), vertices[2].point());
	float top = TriangleArea(p, vertices[2].point(), vertices[3].point());
	float left = TriangleArea(p, vertices[3].point(), vertices[0].point());

	pointArea += TriangleArea(p, vertices[0].point(), vertices[1].point());
	pointArea += TriangleArea(p, vertices[1].point(), vertices[2].point());
	pointArea += TriangleArea(p, vertices[2].point(), vertices[3].point());
	pointArea += TriangleArea(p, vertices[3].point(), vertices[0].point());

	CollisionData* result = new CollisionData;
	result->point = p;
	result->angle = box->angle;

	float distance = 1.0f;

	if (pointArea - 0.1f > boxArea)
	{
		//printf("BoxArea:%f SMALLER THAN PointArea:%f\n", boxArea, pointArea);
		return NULL;
	}
	else
	{
		//printf("BoxArea:%f LARGER THAN PointArea:%f\n", boxArea, pointArea);
		vector<LineSegment2> sides = box->GetSides();
		if (top < bottom && top < left && top < right)
		{
			distance = DistanceToLine(p, sides[TOP].GetLine());
			result->side = TOP;
		}
		else if (bottom < left && bottom < right)
		{
			distance = DistanceToLine(p, sides[BOTTOM].GetLine());
			result->side = BOTTOM;
		}
		else if (left < right)
		{
			distance = DistanceToLine(p, sides[LEFT].GetLine());
			result->side = LEFT;
		}
		else
		{
			distance = DistanceToLine(p, sides[RIGHT].GetLine());
			result->side = RIGHT;
		}
		//printf("D:%f\n", distance);
		result->distance = distance;//TODO
		switch (result->side)
		{
		case TOP:
			printf("TOP\n");
			break;
		case BOTTOM:
			printf("BOTTOM\n");
			break;
		case LEFT:
			printf("LEFT\n");
			break;
		case RIGHT:
			printf("RIGHT\n");
			break;
		default:
			printf("NO?\n");
			break;
		}
		return result;
	}

}

vector<CollisionData*>* Box2DInBox2D(Box2D* box1, Box2D* box2)
{
	if (!AABBInAABB(box1->GetAABB(), box2->GetAABB()))
		return false;

	//printf("AABB Collision(%X,%X)! (Time:%f)\n", box1, box2, time);
	vector<CollisionData*>* result = new vector<CollisionData*>;

	bool collision = false;
	for (Vertex v : box1->GetVertices())
	{
		CollisionData* data = PointInBox2D(Point2(v.x, v.y), box2, box1->GetVelocityAt(Point2(v.x,v.y)));
		if (data)
		{
			collision = true;
			result->push_back(data);
		}
	}
	if (collision)
		return result;

	return NULL;
}

void HandleCollisions(vector<Box2D*> colliders)
{
	bool collision;
	int i;
	for (i = 0; i < 10; i++)
	{
		collision = false;
		for (Box2D* box : colliders)
		{
			if (box->isStatic) continue;
			for (Box2D* box2 : colliders)
			{
				if (box == box2) continue;

				vector<CollisionData*>* collision_data = Box2DInBox2D(box, box2);
				if (collision_data)
				{
					for each (CollisionData* data in *collision_data)
					{

						if (data)
						{
							data->distance *= 1.2f;

							//LinearMomentum2 lm(Force2(box->mass, -box->GetVelocityAt(data->point)), 1.9f);
							//Point2 velAt = box->GetVelocityAt(data->point);

							//LinearMomentum2 M1 = box->GetLinearMomentum();
							//LinearMomentum2 M2 = box2->GetLinearMomentum();

							Point2 p1 = box->GetPosition();
							Point2 p2 = box2->GetPosition();

							Point2 v1 = box->GetLinearVelocity();
							Point2 v2 = box2->GetLinearVelocity();

							Point2 v12 = v1 - v2;

							Point2 n;// = Point2(cos(data->angle), sin(data->angle));

							collision = true;
							switch (data->side)
							{
							case TOP:
								n = Point2(-sin(data->angle), cos(data->angle));

								box->Translate(-sin(data->angle)*data->distance, cos(data->angle)*data->distance);
								box2->Translate(sin(data->angle)*data->distance, -cos(data->angle)*data->distance);

								break;
							case BOTTOM:
								n = Point2(-sin(data->angle), -cos(data->angle));
								
								box->Translate(sin(data->angle)*data->distance, -cos(data->angle)*data->distance);
								box2->Translate(-sin(data->angle)*data->distance, cos(data->angle)*data->distance);
							
								break;
							case LEFT:
								n = Point2(-cos(data->angle), -sin(data->angle));
								
								box->Translate(-cos(data->angle)*data->distance, -sin(data->angle)*data->distance);
								box2->Translate(cos(data->angle)*data->distance, sin(data->angle)*data->distance);

								break;
							case RIGHT:
								n = Point2(cos(data->angle), sin(data->angle));
								
								box->Translate(cos(data->angle)*data->distance, -sin(data->angle)*data->distance);
								box2->Translate(-cos(data->angle)*data->distance, sin(data->angle)*data->distance);

								break;
							default:
								break;
							}
							Point2 r1 = data->point - box->GetPosition();
							Point2 r1n = Point2(-r1.y,r1.x);
							Point2 r2 = data->point - box2->GetPosition();
							Point2 r2n = Point2(-r2.y, r2.x);

							float w1 = box->GetRotationSpeed();
							float w2 = box2->GetRotationSpeed();

							float m1 = box->GetMass();
							float m2 = box2->GetMass();

							float J1 = box->GetAngularMass();
							float J2 = box2->GetAngularMass();

							float e = box->elasticity < box2->elasticity ? box->elasticity : box2->elasticity;

							float j;
							if(box2->isStatic)
								j = -(1 + e)*DotPoint2(v1, n) / (DotPoint2(n, n*(1 / m1)) + (DotPoint2(r1n, n)*DotPoint2(r1n, n)) / J1);
							else
								j = -(1 + e)*DotPoint2(v12, n) / (DotPoint2(n, n*(1 / m1 + 1 / m2)) + (DotPoint2(r1n, n)*DotPoint2(r1n, n)) / J1 + (DotPoint2(r2n, n)*DotPoint2(r2n, n)) / J2);

							v1 = v1 + j / m1*n;
							w1 = w1 + DotPoint2(r1n, j*n) / J1;

							v2 = v2 - j / m2*n;
							w2 = w2 + DotPoint2(r2n, -j*n) / J2;

							box->SetLinearVelocity(v1);
							box->SetRotationSpeed(w1);

							box2->SetLinearVelocity(v2);
							box2->SetRotationSpeed(w2);

							//box->AddAngularMomentum(t,0.9f);
							//box->AddLinearMomentum(lm);

							//box->SetRotationSpeed(-box->rotationSpeed);

							//box2->SetVelocity(-box2->velX, -box2->velY);
							//box2->SetRotationSpeed(-box2->rotationSpeed);
							//printf("Collision! (Time:%f)\n", time);
						}
						break; //TODO Remove and add rotation
					}
				}
			}
		}
		if (!collision)break;
	}
	if (i > 1)
	{
		printf("%d Collisions!\n", i);
	}
}*/