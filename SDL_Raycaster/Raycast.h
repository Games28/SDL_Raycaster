#pragma once
#ifndef RAYCAST_H
#define RAYCAST_H
#include "Constants.h"
#include "Map.h"
#include "Player.h"
#include "SGE_Core.h"


class Raycast
{
public:
	Raycast();
	~Raycast();

	bool GetDistanceToWall(Player& player, Map& map, float fRayAngle, float& fHitX, float& fHitY,
		float& fDistIfFound, int& nMapCoordX, int& nMapCoordY);

	void Render(Player& player, Map& map, flc::SDL_GameEngine& sge);
private:
	flc::Sprite* pWallSprite = nullptr;
	flc::Sprite* pFloorSprite = nullptr;
	flc::Sprite* pCeilSprite = nullptr;
};

#endif // !RAYCAST_H



