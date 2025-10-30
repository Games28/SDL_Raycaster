/* main.cpp - testprogram for the SDL2-based Game Engine (SGE) v.20221204
 * ======================================================================
 *
 * The SGE was developed by Joseph21 and is heavily inspired bij the Pixel Game Engine (PGE) by Javidx9
 * (see: https://github.com/OneLoneCoder/olcPixelGameEngine). It's interface is deliberately kept very
 * close to that of the PGE, so that programs can be ported from the one to the other quite easily.
 *
 * License
 * -------
 * This code is completely free to use, change, rewrite or get inspiration from. At the same time, there's
 * no warranty that this code is free of bugs. If you use (any part of) this code, you accept each and any
 * risk or consequence thereof.
 *
 * Although there is no obligation to mention or shout out to the creator, I wouldn't mind if you did :)
 *
 * Have fun with it!
 *
 * Joseph21
 * december 4, 2022
 */

#include "Game.h"
#include "Constants.h"

// Override base class with your custom functionality


int main( int argc, char *argv[] ) {
    Game demo;
    if (demo.Construct(WINDOW_WIDTH, WINDOW_HEIGHT, 1, 1))
        demo.Start();
    return 0;
}
