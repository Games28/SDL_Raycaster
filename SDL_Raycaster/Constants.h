#pragma once
#ifndef CONTANTS_H
#define CONTANTS_H
#include <iostream>
#include <string>
#include "SGE_Pixel.h"


#include <math.h>
#include <stdint.h>
#include <cfloat>       // needed for constant FLT_MAX in the DDA function





// Screen and pixel constants - keep the screen sizes constant and vary the resolution by adapting the pixel size
// to prevent accidentally defining too large a window
#define WINDOW_WIDTH   1200
#define WINDOW_HEIGHT    720
#define PIXEL_X       1
#define PIXEL_Y       1


#define STRETCHED_TEXTURING  false  // if true, multiple levels are stretched textured. If false: per 1x1x1 block
#define MULTIPLE_LEVELS      true
#define RENDER_CEILING       !MULTIPLE_LEVELS    // render ceilings only for single level world

#define MOUSE_CONTROL        false

// shading constants
#define RENDER_SHADED        true
#define OBJECT_INTENSITY       1.5f
#define MULTIPLIER_INTENSITY   5.0f
#define INTENSITY_SPEED        1.0f

#define SHADE_FACTOR_MIN  0.1f      // the shade factor is clamped between these two values
#define SHADE_FACTOR_MAX  1.0f

// colour constants
#define ROOF_COLOUR  olc::RED
#define TEXT_COLOUR  olc::YELLOW
#define SHADE_COLOUR olc::BLACK    // white for fog effect, black for night effect

#define SHADE_FACTOR_MIN  0.1f      // the shade factor is clamped between these two values
#define SHADE_FACTOR_MAX  1.0f
#define MAX_OBJ_SPRITES 4


#define NUM_TEXTURES 4

#define GRND_FLOOR '.'     // no block
#define FRST_FLOOR '#'     // block of height 1
#define SCND_FLOOR '@'     //                 2
#define THRD_FLOOR '*'     //                 3
#define FRTH_FLOOR '-'     //                 4
#define FFTH_FLOOR '+'     //                 5
#define SXTH_FLOOR '='     //                 6

#define FLOOR_1QRTR 'Q'    // block of height 1/4
#define FLOOR_HALVE 'H'    //                 2/4
#define FLOOR_3QRTR 'T'    //                 3/4


// constants for speed movements - all movements are modulated with fElapsedTime
#define SPEED_ROTATE      60.0f   //                            60 degrees per second
#define SPEED_MOVE         5.0f   // forward and backward    -   5 units per second
#define SPEED_STRAFE       5.0f   // left and right strafing -   5 units per second
#define SPEED_LOOKUP     200.0f   // looking up or down      - 200 pixels per second
#define SPEED_STRAFE_UP    1.0f   // flying or chroucing     -   1.0 block per second

// mini map constants
#define MINIMAP_TILE_SIZE     32
#define MINIMAP_SCALE_FACTOR   0.2   // should be 0.2

#define SIGNIFICANCE 3
#define SIG_POW10    1000

#define FACE_EAST    0
#define FACE_NORTH   1
#define FACE_WEST    2
#define FACE_SOUTH   3
#define FACE_TOP     4
#define FACE_BOTTOM  5
#define FACE_NR_OF   6

#define RADIUS_PLAYER   0.1f
#define RADIUS_ELF      0.2f

// ==============================/  convenience functions for angles  /==============================




typedef uint32_t color_t;

#endif // !CONTANTS_H
