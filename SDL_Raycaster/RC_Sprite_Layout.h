#pragma once
#ifndef RC_SPRITE_LAYOUT_H
#define RC_SPRITE_LAYOUT_H
#include <vector>
#include <string>

struct Sprite_Layout
{
	std::vector<std::string> vWallSpritesFiles = {
		"textures/Metal_07.png",
		"textures/grey stone wall.png",
		"textures/mossystone.png",
		"textures/colorstone.png",
		"textures/wood.png",
		"textures/grey stone wall.png",
		"textures/mossystone.png",
		"textures/colorstone.png",
		"textures/wood.png"

	};

	std::vector<std::string> vCeilSpritesFiles = {
		"textures/Metal_07.png",
		"textures/grey stone wall.png",
		"textures/mossystone.png",
		"textures/colorstone.png",
		"textures/wood.png",
		"textures/Metal_07.png",
		"textures/grey stone wall.png",
		"textures/mossystone.png",
		"textures/colorstone.png",
		"textures/wood.png",

	};

	std::vector<std::string> vRoofSpritesFiles = {

		"textures/Metal_07.png",
		"textures/Metal_09.png",
		"textures/mossystone.png",
		"textures/colorstone.png",
		"textures/wood.png",
		"textures/Metal_07.png",
		"textures/grey stone wall.png",
		"textures/mossystone.png",
		"textures/colorstone.png",
		"textures/wood.png",

	};

	std::vector<std::string> vFloorSpritesFiles = {
		"textures/wood.png",
		"textures/colorstone.png",
		"textures/mossystone.png",
		"textures/Metal_07.png",
		"textures/grey stone wall.png",
		"textures/mossystone.png",
		"textures/colorstone.png",
		"textures/wood.png",


	};

	std::vector<std::string> vObjectSpritesFiles = {
		"textures/r2.png",
		"textures/troopertest.png",
		"textures/tree3.png",
		"textures/tree3_4x.png",
	


	};


};

#endif // !SPRITE_LAYOUT_H

