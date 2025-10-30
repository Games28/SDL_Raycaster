#pragma once
#ifndef CONTANTS_H
#define CONTANTS_H
#include <iostream>
#include <string>
#include "SGE_Pixel.h"


#include <math.h>
#include <stdint.h>

#define PI 3.1415926535f

#define WINDOW_WIDTH   1400
#define WINDOW_HEIGHT    800
#define PIXEL_X       1
#define PIXEL_Y       1

#define COL_CEIL  flc::GREY
#define COL_FLOOR  flc::DARK_GREEN
#define COL_WALL  flc::BLUE
#define COL_TEXT  flc::DARK_YELLOW

#define SPEED_ROTATE 60.0f
#define SPEED_MOVE 5.0f
#define SPEED_STRAFE 5.0f;

#define NUM_TEXTURES 4

const float fPlayerFoV_deg = 60.0f;


typedef uint32_t color_t;

#endif // !CONTANTS_H
