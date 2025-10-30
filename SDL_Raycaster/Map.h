#pragma once
#ifndef MAP_H
#define MAP_H
#include <string>
#include "Constants.h"

class Map
{
public:
	Map();
	~Map();

	int getMapX();
	int getMapY();
	int getMapIndex(int x,int y);
	float getDistToProjPlane();
	float getMaxDistance();

private:
	std::string sMap;
	float fDistToProjPlane;
	int nMapX;
	int nMapY;
	float fMaxDistance;
};

#endif // !MAP_H



