#pragma once
#include <vector>
#include <algorithm>
#include <list>

struct Node;
bool ExistsInVector(Node* node, std::vector<Node*>* nodes, uint size = 0);
bool RemoveFromVector(Node* node, std::vector<Node*>* nodes);

struct Node
{
    Node() { x = 0; y = 0; G = -1; H = -1; from = NULL; };
    Node(float X, float Y) { x = X; y = Y; G = 1000000000; H = 1000000000; from = NULL; };
    
    bool operator==(const Node& rhs) { if (x == rhs.x && y == rhs.y) return true; return false; };
    float F() { return G + H; };
    float DistanceSquared(const Node* node) { return (node->x - x)*(node->x - x) + (node->y - y)*(node->y - y); };
    float Distance(const Node* node) { return sqrtf(DistanceSquared(node)); };

    bool AddNeighbour(Node* node) 
    {
        if (this == node)
            return false;

        if (!ExistsInVector(node, &neighbours))
        {
            neighbours.push_back(node);
            return true;
        }
        return false;
    };
    bool RemoveNeighbour(Node* node) 
    {
        return RemoveFromVector(node, &neighbours);
    };

    std::vector<Node*> neighbours;
    Node* from;
    float G, H;
    float x, y;
};

//TODO: Maybe change to return Node*
bool ExistsInVector(Node* node, std::vector<Node*>* nodes, uint size)
{
	if (size == 0)
	{
		for (uint i = 0; i < nodes->size(); i++)
		{
			if (node == (*nodes)[i])
			{
				return true;
			}
		}
	}
	else
	{
		for (uint i = 0; i < size; i++)
		{
			if (node == (*nodes)[i])
			{
				return true;
			}
		}
	}
    return false;
}

bool RemoveFromVector(Node* node, std::vector<Node*>* nodes)
{
    for (int i = 0; i < nodes->size(); i++)
    {
        if (node == (*nodes)[i])
        {
            nodes->erase(nodes->begin()+i);
            return true;
        }
    }
    return false;
}


std::vector<Node*> DoPathFinding(Node* start, Node* end)
{
    std::vector<Node*> path;
    if (start == end) return path;

    std::vector<Node*> openList = { start };
	uint openListSize = 1;
    std::vector<Node*> closedList;
	uint closedListSize = 0;

    Node* current;
    start->G = 0;
    start->H = start->DistanceSquared(end);
    start->from = NULL;

    int loops = 0;
    while (openList.size() != 0 && loops < 100)
    {
        loops++;

        current = openList.back();

        std::vector<Node*>::iterator it = openList.begin();
        std::vector<Node*>::iterator res = openList.begin();
        while (it != openList.end())
        {
            if ((*it)->F() < (*res)->F())
            {
                res = it;
            }
            it++;
        }
        current = *res;
        openList.erase(res);
		openListSize--;

        closedList.push_back(current);
		closedListSize++;


        for (int i = 0; i < current->neighbours.size(); i++)
        {
            if (current->neighbours[i] == end)
            {
				std::vector<Node*> reversePath;
				reversePath.push_back(current->neighbours[i]);
                Node* fromNode = current;
                while (fromNode != NULL)
                {
					reversePath.push_back(fromNode);
                    fromNode = fromNode->from;
                }
				for (int i = (int)reversePath.size() - 1; i >= 0; i--)
				{
					path.push_back(reversePath[i]);
				}
                return path;
            }

            bool inOpenList = false;
            bool inClosedList = false;
            if (ExistsInVector(current->neighbours[i], &openList, openListSize))
            {
                inOpenList = true;
            }
            if (ExistsInVector(current->neighbours[i], &closedList, closedListSize))
            {
                inClosedList = true;
            }
            if (!inClosedList)
            {
                if (!inOpenList)
                {
                    current->neighbours[i]->from = current;
                    current->neighbours[i]->G = current->G + current->neighbours[i]->DistanceSquared(current);
                    current->neighbours[i]->H = current->neighbours[i]->DistanceSquared(end);
                    openList.push_back(current->neighbours[i]);
					openListSize++;
                }
            }
        }
    }
    return path;
}

