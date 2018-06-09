#pragma once

#include <stdio.h>
#include <vector>
#include "Geometry.h"
#include "RenderFunctions.h"


//Generic Quadtree that uses rectangles for objects. Objects need a Rect() function.
template <class T>
class Octree {

public:

	Octree(int pLevel, Cuboid pBounds)
	{
		//printf("Quadtree Constructor\n");
		level = pLevel;
		objects = std::vector<T*>();
		bounds = pBounds;
		nodes = std::vector<Octree*>(8);
	}

	void Render(BatchRenderer* renderer)
	{
		//printf("Quadtree Render(%.1f,%.1f,%.1f,%.1f)\n", bounds.x, bounds.y, bounds.w, bounds.h);
		for (int i = 0; i < nodes.size(); i++)
		{
			if (nodes[i] != NULL)
			{
				nodes[i]->Render(renderer);
			}
		}
		std::vector<Vector3f> linestrip =
		{
			{ bounds.x,bounds.y,bounds.z },
		{ bounds.x + bounds.w,bounds.y,bounds.z },
		{ bounds.x + bounds.w,bounds.y + bounds.h,bounds.z },
		{ bounds.x ,bounds.y + bounds.h,bounds.z },
		{ bounds.x ,bounds.y,bounds.z }
		};
		renderer->RenderLineStrip(linestrip, BLUE);

		std::vector<Vector3f> linestrip2 =
		{
			{ bounds.x,bounds.y,bounds.z + bounds.d },
		{ bounds.x + bounds.w,bounds.y,bounds.z + bounds.d },
		{ bounds.x + bounds.w,bounds.y + bounds.h,bounds.z + bounds.d },
		{ bounds.x ,bounds.y + bounds.h,bounds.z + bounds.d },
		{ bounds.x ,bounds.y,bounds.z + bounds.d }
		};
		renderer->RenderLineStrip(linestrip2, BLUE);
		renderer->RenderLine(Vector3f( bounds.x,bounds.y,bounds.z ), Vector3f( bounds.x,bounds.y,bounds.z+bounds.d ), BLUE);
		renderer->RenderLine(Vector3f( bounds.x + bounds.w,bounds.y,bounds.z ), Vector3f( bounds.x + bounds.w,bounds.y,bounds.z + bounds.d ), BLUE);
		renderer->RenderLine(Vector3f( bounds.x + bounds.w,bounds.y + bounds.h,bounds.z ), Vector3f( bounds.x + bounds.w,bounds.y + bounds.h,bounds.z + bounds.d ), BLUE);
		renderer->RenderLine(Vector3f( bounds.x,bounds.y + bounds.h,bounds.z ), Vector3f( bounds.x,bounds.y + bounds.h,bounds.z + bounds.d ), BLUE);


		for (int i = 0; i < objects.size(); i++)
		{
			if (objects[i] != NULL)
			{
				Cuboid cube = objects[i]->Cube();

				renderer->RenderCuboid(cube, YELLOW);
				
			}
		}
	}

	void clear()
	{
		//printf("Quadtree Clear\n");
		objects.clear();

		for (int i = 0; i < nodes.size(); i++) {
			if (nodes[i] != NULL) {
				nodes[i]->clear();
				delete nodes[i];
				nodes[i] = NULL;
			}
		}
	}

	void insert(T* obj)
	{
		//printf("Quadtree Insert(%.1f,%.1f,%.1f,%.1f)\n",pRect.x, pRect.y, pRect.w, pRect.h);
		if (nodes[0] != NULL) {
			int index = GetIndex(obj->Cube());

			if (index != -1) {
				nodes[index]->insert(obj);

				return;
			}
		}

		objects.push_back(obj);

		if (objects.size() > MAX_OBJECTS && level < MAX_LEVELS) {
			if (nodes[0] == NULL) {
				Split();
			}

			int i = 0;
			while (i < objects.size()) {
				int index = GetIndex(objects[i]->Cube());
				if (index != -1) {
					nodes[index]->insert(objects[i]);
					objects.erase(objects.begin() + i);
				}
				else {
					i++;
				}
			}
		}
	}

	std::vector<T*> retrieve(std::vector<T*> returnObjects, Cuboid pRect)
	{
		//printf("Quadtree Retrieve\n");
		int index = GetIndex(pRect);
		if (index != -1 && nodes[0] != NULL) {
			nodes[index]->retrieve(returnObjects, pRect);
		}

		returnObjects.insert(returnObjects.end(), objects.begin(), objects.end());

		return returnObjects;
	}

private:

	void Split()
	{
		//printf("Quadtree Split\n");
		float subWidth = (bounds.w / 2.0f);
		float subHeight = (bounds.h / 2.0f);
		float subDepth = (bounds.d / 2.0f);
		float x = bounds.x;
		float y = bounds.y;
		float z = bounds.z;

		nodes[0] = new Octree(level + 1, Cuboid(x + subWidth, y, z, subWidth, subHeight, subDepth));
		nodes[1] = new Octree(level + 1, Cuboid(x, y, z, subWidth, subHeight, subDepth));
		nodes[2] = new Octree(level + 1, Cuboid(x, y + subHeight, z, subWidth, subHeight, subDepth));
		nodes[3] = new Octree(level + 1, Cuboid(x + subWidth, y + subHeight, z, subWidth, subHeight, subDepth));

		nodes[4] = new Octree(level + 1, Cuboid(x + subWidth, y, z + subDepth, subWidth, subHeight, subDepth));
		nodes[5] = new Octree(level + 1, Cuboid(x, y, z + subDepth, subWidth, subHeight, subDepth));
		nodes[6] = new Octree(level + 1, Cuboid(x, y + subHeight, z + subDepth, subWidth, subHeight, subDepth));
		nodes[7] = new Octree(level + 1, Cuboid(x + subWidth, y + subHeight, z + subDepth, subWidth, subHeight, subDepth));
	}

	int GetIndex(Cuboid pRect)
	{
		//printf("Quadtree GetIndex\n");
		int index = -1;
		double xMidpoint = bounds.x + (bounds.w / 2);
		double yMidpoint = bounds.y + (bounds.h / 2);
		double zMidpoint = bounds.z + (bounds.d / 2);

		// Object can completely fit within the top quadrants
		bool topQuadrant = (pRect.y < yMidpoint && pRect.y + pRect.h < yMidpoint);
		// Object can completely fit within the bottom quadrants
		bool bottomQuadrant = (pRect.y > yMidpoint);

		bool frontQuadrant = (pRect.z < zMidpoint && pRect.z + pRect.d < zMidpoint);

		bool backQuadrant = (pRect.z > zMidpoint);

		// Object can completely fit within the left quadrants
		if (pRect.x < xMidpoint && pRect.x + pRect.w < xMidpoint) {
			if (topQuadrant) {
				if (frontQuadrant)
					index = 1;
				else if (backQuadrant)
					index = 5;
			}
			else if (bottomQuadrant) {
				if (frontQuadrant)
					index = 2;
				else if (backQuadrant)
					index = 6;
			}
		}
		// Object can completely fit within the right quadrants
		else if (pRect.x > xMidpoint) {
			if (topQuadrant) {
				if (frontQuadrant)
					index = 0;
				else if (backQuadrant)
					index = 4;
			}
			else if (bottomQuadrant) {
				if (frontQuadrant)
					index = 3;
				else if (backQuadrant)
					index = 7;
			}
		}

		return index;
	}

	int MAX_OBJECTS = 10;
	int MAX_LEVELS = 10;

	int level;
	std::vector<T*> objects;
	Cuboid bounds;
	std::vector<Octree*> nodes;

};
