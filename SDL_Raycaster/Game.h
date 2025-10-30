#pragma once
#ifndef GAME_H
#define GAME_H
#include "Constants.h"
#include "Textures.h"
#include "SGE_Core.h"
#include <fstream>
#include "Map.h"
#include "Player.h"
#include "Raycast.h"




class Game : public flc::SDL_GameEngine
{
public:
    Game() {
        // Name your application
        sAppName = "raycaster";
    }

public:
    bool OnUserCreate() override;

    bool OnUserUpdate(float fElapsedTime) override;

    void processInput(float deltaTime);
private:
    Player player;
    Map map;
    Raycast ray;
};


#endif // !GAME_H



