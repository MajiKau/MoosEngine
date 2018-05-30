#pragma once

#include <stdio.h>
#include <vector>
#include "Geometry.h"
#include "RenderFunctions.h"

struct Rectangle2D
{
	Rectangle2D() {};
	Rectangle2D(float X, float Y, float width, float height) :x(X), y(Y), w(width), h(height) {};
	float x, y, w, h;
};

//Generic Quadtree that uses rectangles for objects
class Quadtree {

public:

	Quadtree(int pLevel, Rectangle2D pBounds) 
	{
		//printf("Quadtree Constructor\n");
		level = pLevel;
		objects = std::vector<Rectangle2D*>();
		bounds = pBounds;
		nodes = std::vector<Quadtree*>(4);
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
		std::vector<Point2> linestrip =
		{
			{ bounds.x,bounds.y },
		{ bounds.x + bounds.w,bounds.y  },
		{ bounds.x + bounds.w,bounds.y + bounds.h },
		{ bounds.x ,bounds.y + bounds.h },
		{ bounds.x ,bounds.y  }
		};
		renderer->RenderLineStrip(linestrip, BLUE);

		for (int i = 0; i < objects.size(); i++)
		{
			if (objects[i] != NULL)
			{
				std::vector<Point2> linestrip =
				{
					{ objects[i]->x,objects[i]->y  },
				{ objects[i]->x + objects[i]->w,objects[i]->y  },
				{ objects[i]->x + objects[i]->w,objects[i]->y + objects[i]->h },
				{ objects[i]->x ,objects[i]->y + objects[i]->h },
				{ objects[i]->x ,objects[i]->y  }
				};
				renderer->RenderLineStrip(linestrip, YELLOW);
				renderer->RenderRegularTriangle({ objects[i]->x + objects[i]->w / 2.0f,objects[i]->y + objects[i]->h / 2.0f }, 0.1f, 0, YELLOW);
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

	void insert(Rectangle2D* pRect) 
	{
		//printf("Quadtree Insert(%.1f,%.1f,%.1f,%.1f)\n",pRect.x, pRect.y, pRect.w, pRect.h);
		if (nodes[0] != NULL) {
			int index = GetIndex(*pRect);

			if (index != -1) {
				nodes[index]->insert(pRect);

				return;
			}
		}

		objects.push_back(pRect);

		if (objects.size() > MAX_OBJECTS && level < MAX_LEVELS) {
			if (nodes[0] == NULL) {
				Split();
			}

			int i = 0;
			while (i < objects.size()) {
				int index = GetIndex(*objects[i]);
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

	std::vector<Rectangle2D*> retrieve(std::vector<Rectangle2D*> returnObjects, Rectangle2D pRect)
	{
		//printf("Quadtree Retrieve\n");
		int index = GetIndex(pRect);
		if (index != -1 && nodes[0] != NULL) {
			nodes[index]->retrieve(returnObjects, pRect);
		}

		returnObjects.insert(returnObjects.end(), objects.begin(),objects.end());

		return returnObjects;
	}

private:

	void Split() 
	{
		//printf("Quadtree Split\n");
		float subWidth = (bounds.w / 2.0f);
		float subHeight = (bounds.h / 2.0f);
		float x = bounds.x;
		float y = bounds.y;

		nodes[0] = new Quadtree(level + 1, Rectangle2D(x + subWidth, y, subWidth, subHeight));
		nodes[1] = new Quadtree(level + 1, Rectangle2D(x, y, subWidth, subHeight));
		nodes[2] = new Quadtree(level + 1, Rectangle2D(x, y + subHeight, subWidth, subHeight));
		nodes[3] = new Quadtree(level + 1, Rectangle2D(x + subWidth, y + subHeight, subWidth, subHeight));
	}

	int GetIndex(Rectangle2D pRect)
	{
		//printf("Quadtree GetIndex\n");
		int index = -1;
		double verticalMidpoint = bounds.x + (bounds.w / 2);
		double horizontalMidpoint = bounds.y + (bounds.h / 2);

		// Object can completely fit within the top quadrants
		bool topQuadrant = (pRect.y < horizontalMidpoint && pRect.y + pRect.h < horizontalMidpoint);
		// Object can completely fit within the bottom quadrants
		bool bottomQuadrant = (pRect.y > horizontalMidpoint);

		// Object can completely fit within the left quadrants
		if (pRect.x < verticalMidpoint && pRect.x + pRect.w < verticalMidpoint) {
			if (topQuadrant) {
				index = 1;
			}
			else if (bottomQuadrant) {
				index = 2;
			}
		}
		// Object can completely fit within the right quadrants
		else if (pRect.x > verticalMidpoint) {
			if (topQuadrant) {
				index = 0;
			}
			else if (bottomQuadrant) {
				index = 3;
			}
		}

		return index;
	}

	int MAX_OBJECTS = 10;
	int MAX_LEVELS = 5;

	int level;
	std::vector<Rectangle2D*> objects;
	Rectangle2D bounds;
	std::vector<Quadtree*> nodes;

};
