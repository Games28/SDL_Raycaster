#pragma once
#ifndef TEXTURES_H
#define TEXTURES_H
#include <stdint.h>
#include "Constants.h"
#include "upng.h"


class Textures
{
public:
	Textures() = default;
	~Textures() = default;

	void LoadTextures();
	void freeTextures();
public:
	upng_t* textures[NUM_TEXTURES];
	const char* textureFileNames[NUM_TEXTURES] = {
		"./textures/colorstone.png",
		"./textures/grey stone wall.png",
		"./textures/mossystone.png",
		"./textures/wood.png"
	};


};

#endif // !TEXTURES_H


