#include "Game.h"

bool Game::OnUserCreate()
{

	return true;
}

bool Game::OnUserUpdate(float fElapsedTime)
{
    processInput(fElapsedTime);
    
    ray.Render(player, map, *this);
    
    return true;
}

void Game::processInput(float deltaTime)
{

    float fNewX = player.getPlayerX();
    float fNewY = player.getPlayerY();

    float fPlayerA_deg = player.getPlayerAngle();
    if (GetKey(flc::D).bHeld) { fPlayerA_deg += SPEED_ROTATE * deltaTime; if (fPlayerA_deg >= 360.0f) fPlayerA_deg -= 360.0f; }
    if (GetKey(flc::A).bHeld) { fPlayerA_deg -= SPEED_ROTATE * deltaTime; if (fPlayerA_deg < 0.0f) fPlayerA_deg += 360.0f; }

   

    // walk forward - CD checked
    if (GetKey(flc::W).bHeld) {
        fNewX += cos(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * deltaTime;
        fNewY += sin(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * deltaTime;
    }
    // walk backwards - CD checked
    if (GetKey(flc::S).bHeld) {
        fNewX -= cos(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * deltaTime;
        fNewY -= sin(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * deltaTime;
    }

    if (GetKey(flc::Q).bHeld) {
        fNewX += sin(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * deltaTime;
        fNewY -= cos(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * deltaTime;
    }
    if (GetKey(flc::E).bHeld) {
        fNewX -= sin(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * deltaTime;
        fNewY += cos(fPlayerA_deg * PI / 180.0f) * SPEED_MOVE * deltaTime;
    }
   
    // collision detection - check if out of bounds or inside occupied tile
    // only update position if no collision
    if (fNewX >= 0 && fNewX < map.getMapX() &&
        fNewY >= 0 && fNewY < map.getMapY() &&
        map.getMapIndex(int(fNewX),int(fNewY)) != '#') {
        player.setPlayerX(fNewX);
        player.setPlayerY(fNewY);
        player.setPlayerAngle(fPlayerA_deg);
    }
}
