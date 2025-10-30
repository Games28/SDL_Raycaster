#include "Map.h"

Map::Map()
{
    //            0         1
        //            0123456789012345
    sMap.append("################");
    sMap.append("#..............#");
    sMap.append("#........####..#");
    sMap.append("#..............#");
    sMap.append("#...#.....#....#");
    sMap.append("#...#..........#");
    sMap.append("#...####.......#");
    sMap.append("#..............#");
    sMap.append("#..............#");
    sMap.append("#..............#");
    sMap.append("#......##.##...#");
    sMap.append("#......#...#...#");
    sMap.append("#......#...#...#");
    sMap.append("#.......###....#");
    sMap.append("#..............#");
    sMap.append("################");

    nMapX = 16;
    nMapY = 16;
    fMaxDistance = 25.0f;
    fDistToProjPlane = ((WINDOW_WIDTH / 2.0f) / sin((fPlayerFoV_deg / 2.0f) * PI / 180.0f)) * cos((fPlayerFoV_deg / 2.0f) * PI / 180.0f);
}

Map::~Map()
{
}

int Map::getMapX()
{
	return nMapX;
}

int Map::getMapY()
{
	return nMapY;
}

int Map::getMapIndex(int x,int y)
{
	return sMap[y * nMapX + x];
}

float Map::getDistToProjPlane()
{
    return fDistToProjPlane;
}

float Map::getMaxDistance()
{
    return fMaxDistance;
}
