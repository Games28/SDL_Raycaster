#pragma once
#ifndef GAME_H
#define GAME_H
#include "Constants.h"
#include "SGE_Core.h"
#include "SGE_Pixel.h"
#include <fstream>
#include "Player.h"
#include "RC_Map.h"
#include "RC_Object.h"
#include <algorithm>
#include "RC_MapLayout.h"
#include "RC_Misc.h"
#include "RC_Sprite_Layout.h"






class Game : public flc::SDL_GameEngine
{
public:
    Game() {
        // Name your application
        sAppName = "raycaster";
    }

    struct IntersectInfo {
        float fHitX,
            fHitY;
        int   nMapCoordX,
            nMapCoordY;
        float fDistFrnt,
            fDistBack;
        float fHeight;
        int   nLevel = -1;


        int bot_front = -1;
        int bot_back = -1;
        int top_front = -1;
        int top_back = -1;

    };

private:
    Player player;
    std::list<RC_Object> vListObjects;
    RC_Map cMap;
    MapLayout ml;
    Sprite_Layout sl;
    Blocks bk;

    std::string sMap;     // contains chars that define the type of block per map location
    float* fMap;     // contains floats that represent the height per block
    int nMapX = 48;
    int nMapY = 48;

    // max visible distance - use length of map diagonal to overlook whole map
    float fMaxDistance;

    // player: position and looking angle
    float fPlayerX = 2.5f;
    float fPlayerY = 2.5f;
    float fPlayerA_deg = 0.0f;      // looking angle is in degrees, 0.0f is EAST

    // player: height of eye point and field of view
    float fPlayerH = 0.5f;
    float fPlayerFoV_deg = 60.0f;   // in degrees !!

    // factor for looking up or down - initially 0.0f (in pixel space: float is for smooth movement)
    float fLookUp = 0.0f;
    float fDistToProjPlane;         // constant distance to projection plane - is calculated in OnUserCreate()

   //std::vector<flc::Sprite*> vWallSprites;
   //std::vector<flc::Sprite*> vCeilSprites;
   //std::vector<flc::Sprite*> vRoofSprites;
   //std::vector<flc::Sprite*> vFloorSprites;
   //std::vector<flc::Sprite*> vObjectSprites;

    //renderable wall sprites test
    std::vector<flc::Renderable> RendWallSprites;
    std::vector<flc::Renderable> RendCeilSprites;
    std::vector<flc::Renderable> RendFloorSprites;
    std::vector<flc::Renderable> RendRoofSprites;

    bool bMouseControl = MOUSE_CONTROL;     // toggle on mouse control (trigger key M)

    // var's and initial values for shading
    float fObjectIntensity = MULTIPLE_LEVELS ? OBJECT_INTENSITY : 0.2f;
    float fIntensityMultiplier = MULTIPLE_LEVELS ? MULTIPLIER_INTENSITY : 10.0f;

    // toggles for rendering
    bool bMinimap = false;    // toggle on mini map rendering (trigger key P)
    bool bMapRays = false;    //                              (trigger key O)
    bool bDebugInfo = false;    //                              (trigger key I)
    float* fDepthBuffer = nullptr;


    std::vector<flc::vf2d> vRayList;    // needed for ray rendering in minimap
  

public:
    bool OnUserCreate() override {

        bool bSuccess = true;

        bk.InitBlockTypes();
       
        

        // create and fill the map - NOTE: string arguments in AddLayer() must match x and y dimensions in InitMap()!
        // the map itself is defined in a separate file
        cMap.InitMap(ml.glbMapX, ml.glbMapY);
        for (int i = 0; i < (int)ml.vMap_level.size(); i++) {
            cMap.AddLayer(ml.vMap_level[i], bk);
        }

        // max ray length for DDA is diagonal length of the map
        fMaxDistance = cMap.DiagonalLength();

        // initialize sine and cosine lookup arrays - these are meant for performance improvement
        init_lu_sin_array();
        init_lu_cos_array();

        // Work out distance to projection plane. This is a constant float value, depending on the width of the projection plane and the field of view.
        fDistToProjPlane = ((ScreenWidth() / 2.0f) / lu_sin(fPlayerFoV_deg / 2.0f)) * lu_cos(fPlayerFoV_deg / 2.0f);

       //load all sprites
        //auto load_sprite_file = [=](const std::string& sFileName) {
        //    flc ::Sprite* tmp = new flc::Sprite(sFileName);
        //    if (tmp->width == 0 || tmp->height == 0) {
        //        std::cout << "ERROR: OnUserCreate() --> can't load file: " << sFileName << std::endl;
        //        delete tmp;
        //        tmp = nullptr;
        //    }
        //    return tmp;
        //    };
        //
        //auto load_sprites_from_files = [=](std::vector<std::string>& vFileNames, std::vector<flc::Sprite*>& vSpritePtrs) {
        //    bool bNoErrors = true;
        //    for (auto& s : vFileNames) {
        //        flc::Sprite* tmpPtr = load_sprite_file(s);
        //        bNoErrors &= (tmpPtr != nullptr);
        //        vSpritePtrs.push_back(tmpPtr);
        //    }
        //    return bNoErrors;
        //    };
        //
        //// load all sprites into their associated container
        //bSuccess &= load_sprites_from_files(sl.vWallSpritesFiles, vWallSprites);
        //bSuccess &= load_sprites_from_files(sl.vCeilSpritesFiles, vCeilSprites);
        //bSuccess &= load_sprites_from_files(sl.vRoofSpritesFiles, vRoofSprites);
        //bSuccess &= load_sprites_from_files(sl.vFloorSpritesFiles, vFloorSprites);
        //bSuccess &= load_sprites_from_files(sl.vObjectSpritesFiles, vObjectSprites);
        
       
        //test loading renderable sprites
        //wall
        for (int i = 0; i < sl.vWallSpritesFiles.size(); i++)
        {
            flc::Renderable tmp;
            tmp.Load(sl.vWallSpritesFiles[i]);
            RendWallSprites.push_back(tmp);
        }

        //ceiling
        for (int i = 0; i < sl.vCeilSpritesFiles.size(); i++)
        {
            flc::Renderable tmp;
            tmp.Load(sl.vCeilSpritesFiles[i]);
            RendCeilSprites.push_back(tmp);
        }

        //floor
        for (int i = 0; i < sl.vFloorSpritesFiles.size(); i++)
        {
            flc::Renderable tmp;
            tmp.Load(sl.vFloorSpritesFiles[i]);
            RendFloorSprites.push_back(tmp);
        }

        //roof
        for (int i = 0; i < sl.vRoofSpritesFiles.size(); i++)
        {
            flc::Renderable tmp;
            tmp.Load(sl.vRoofSpritesFiles[i]);
            RendRoofSprites.push_back(tmp);
        }


        // Initialize depth buffer
        fDepthBuffer = new float[ScreenWidth() * ScreenHeight()];

       


        // aux map to keep track of placed objects
        //std::string sObjMap;
        //for (int y = 0; y < cMap.Height(); y++) {
        //    for (int x = 0; x < cMap.Width(); x++) {
        //        sObjMap.append(" ");
        //    }
        //}
        //// only place objects where there's nothing in the direct (8 connected) neighbourhood
        //auto space_for_object = [=](int x, int y) -> bool {
        //    int xMin = std::max(0, x - 1);
        //    int yMin = std::max(0, y - 1);
        //    int xMax = std::min(cMap.Width() - 1, x + 1);
        //    int yMax = std::min(cMap.Height() - 1, y + 1);
        //    bool bOccupied = false;
        //    for (int r = yMin; r <= yMax && !bOccupied; r++) {
        //        for (int c = xMin; c <= xMax && !bOccupied; c++) {
        //            bOccupied = cMap.CellHeight(c, r) != 0.0f || sObjMap[r * cMap.Width() + c] != ' ';
        //        }
        //    }
        //    return !bOccupied;
        //    };
        //
        //// populate object list with randomly chosen, scaled and placed objects (switched off for current development phase)
        //for (int i = 0; i < 0; i++) {
        //    int nRandX, nRandY;
        //    bool bFoundEmpty = false;
        //    bool bMakeDynamic = false;
        //    do {
        //        nRandX = rand() % cMap.Width();
        //        nRandY = rand() % cMap.Height();
        //
        //        bFoundEmpty = space_for_object(nRandX, nRandY);
        //    } while (!bFoundEmpty);
        //
        //    // make sure that at least ... objects are dynamic
        //    int nRandObj = (i < 5) ? 0 : rand() % vObjectSprites.size();
        //    int nRandSize;
        //    switch (nRandObj) {
        //    case 0:
        //        // this is an elf girl, make dynamic
        //        bMakeDynamic = true;
        //        nRandSize = rand() % 5 + 5;
        //        break;
        //    case 1: case 2:
        //        // these are stationary objects of fixed size
        //        bMakeDynamic = false;
        //        nRandSize = 10;
        //        break;
        //    case 3: case 4: case 5: case 6:
        //        // these are bushes
        //        bMakeDynamic = false;
        //        nRandSize = rand() % 10 + 5;
        //        break;
        //    default:
        //        // trees
        //        bMakeDynamic = false;
        //        nRandSize = rand() % 20 + 10;
        //    }
        //
        //    // convenience lambda's for random range integers and floats
        //    auto int_rand_between = [=](int nLow, int nHgh) {
        //        return (rand() % (nHgh - nLow + 1)) + nLow;
        //        };
        //    auto float_rand_between = [=](float fLow, float fHgh) {
        //        int nLow = int(1000.0f * fLow);
        //        int nHgh = int(1000.0f * fHgh);
        //        return float(int_rand_between(nLow, nHgh)) / 1000.0f;
        //        };
        //
        //    RC_Object tmpObj(float(nRandX) + 0.5f, float(nRandY) + 0.5f, float(nRandSize / 10.0f), -1.0f, 0.0f,0.4f, vObjectSprites[nRandObj]);
        //    if (bMakeDynamic) {
        //        tmpObj.bStatic = false;
        //        tmpObj.SetVX(float_rand_between(-5.0f, 5.0f));
        //        tmpObj.SetVY(float_rand_between(-5.0f, 5.0f));
        //
        //    }
        //    else {
        //        tmpObj.bStatic = true;
        //        tmpObj.SetVX(0.0f);
        //        tmpObj.SetVY(0.0f);
        //    }
        //    vListObjects.push_back(tmpObj);
        //    // make a mark that an object is placed at this tile
        //    sObjMap[nRandY * cMap.Width() + nRandX] = 'X';
        //}


      

        return bSuccess;
    }


    void PrintHitPoint(IntersectInfo& p, bool bVerbose) {
        std::cout << "hit (world): ( " << p.fHitX << ", " << p.fHitY << " ) ";
        std::cout << "hit (tile): ( " << p.nMapCoordX << ", " << p.nMapCoordY << " ) ";
        std::cout << "dist.: " << p.fDistFrnt << " ";
        std::cout << "lvl: " << p.nLevel << " hght: " << p.fHeight << " ";
        if (bVerbose) {
            std::cout << "bot frnt: " << p.bot_front << " bot back: " << p.bot_back << " ";
            std::cout << "top frnt: " << p.top_front << " top back: " << p.top_back << " ";
        }
        std::cout << std::endl;
    }

    void PrintHitList(std::vector<IntersectInfo>& vHitList, bool bVerbose = false) {
        for (int i = 0; i < (int)vHitList.size(); i++) {
            std::cout << "Elt: " << i << " = ";
            PrintHitPoint(vHitList[i], bVerbose);
        }
        std::cout << std::endl;
    }


    bool GetDistancesToWallsPerLevel(int level, float fRayAngle, std::vector<IntersectInfo>& vHitList) {

        // counter for nr of hit points found
        int nHitPointsFound = 0;

        // The player's position is the "from point"
        float fFromX = fPlayerX;
        float fFromY = fPlayerY;
        // Calculate the "to point" using the player's angle and fMaxDistance
        float fToX = fPlayerX + fMaxDistance * lu_cos(fRayAngle);
        float fToY = fPlayerY + fMaxDistance * lu_sin(fRayAngle);
        // work out normalized direction vector (fDX, fDY)
        float fDX = fToX - fFromX;
        float fDY = fToY - fFromY;
        float fRayLen = sqrt(fDX * fDX + fDY * fDY);
        fDX /= fRayLen;
        fDY /= fRayLen;
        // calculate the scaling factors for the ray increments per unit in x resp y direction
        // this calculation takes division by 0.0f into account
        float fSX = (fDX == 0.0f) ? FLT_MAX : sqrt(1.0f + (fDY / fDX) * (fDY / fDX));
        float fSY = (fDY == 0.0f) ? FLT_MAX : sqrt(1.0f + (fDX / fDY) * (fDX / fDY));
        // work out if line is going right or left resp. down or up
        int nGridStepX = (fDX > 0.0f) ? +1 : -1;
        int nGridStepY = (fDY > 0.0f) ? +1 : -1;

        // init loop variables
        float fLengthPartialRayX = 0.0f;
        float fLengthPartialRayY = 0.0f;

        int nCurX = int(fFromX);
        int nCurY = int(fFromY);

        // work out the first intersections with the grid
        if (nGridStepX < 0) { // ray is going left - get scaled difference between start point and left cell border
            fLengthPartialRayX = (fFromX - float(nCurX)) * fSX;
        }
        else {              // ray is going right - get scaled difference between right cell border and start point
            fLengthPartialRayX = (float(nCurX + 1.0f) - fFromX) * fSX;
        }
        if (nGridStepY < 0) { // ray is going up - get scaled difference between start point and top cell border
            fLengthPartialRayY = (fFromY - float(nCurY)) * fSY;
        }
        else {              // ray is going down - get scaled difference between bottom cell border and start point
            fLengthPartialRayY = (float(nCurY + 1.0f) - fFromY) * fSY;
        }

        // check whether analysis got out of map boundaries
        bool bOutOfBounds = !cMap.IsInBounds(nCurX, nCurY);
        // did analysis reach the destination cell?
        bool bDestCellFound = (nCurX == int(fToX) && nCurY == int(fToY));

        float fDistIfFound = 0.0f;  // accumulates distance of analysed piece of ray
        float fCurHeight = 0.0f;  // to check on differences in height

        while (!bOutOfBounds && !bDestCellFound && fDistIfFound < fMaxDistance) {

            // advance to next map cell, depending on length of partial ray's
            if (fLengthPartialRayX < fLengthPartialRayY) {
                // continue analysis in x direction
                nCurX += nGridStepX;
                fDistIfFound = fLengthPartialRayX;
                fLengthPartialRayX += fSX;
            }
            else {
                // continue analysis in y direction
                nCurY += nGridStepY;
                fDistIfFound = fLengthPartialRayY;
                fLengthPartialRayY += fSY;
            }

            bOutOfBounds = !cMap.IsInBounds(nCurX, nCurY);
            if (bOutOfBounds) {
                bDestCellFound = false;

                // If out of bounds, finalize the list with one additional intersection with the map boundary and height 0.
                // (only if the list is not empty!) This additional intersection record is necessary for proper rendering at map boundaries.
                if (fCurHeight != 0.0f && nHitPointsFound > 0) {

                    fCurHeight = 0.0f;  // since we're out of bounds
                    // put the collision info in a new IntersectInfo node and push it up the hit list
                    IntersectInfo sInfo;
                    sInfo.fDistFrnt = fDistIfFound;
                    sInfo.fHitX = fFromX + fDistIfFound * fDX;
                    sInfo.fHitY = fFromY + fDistIfFound * fDY;
                    sInfo.nMapCoordX = nCurX;
                    sInfo.nMapCoordY = nCurY;
                    sInfo.fHeight = fCurHeight;
                    sInfo.nLevel = level;
                    vHitList.push_back(sInfo);
                }
            }
            else {
                // check if there's a difference in height found
                bool bHitFound = (cMap.CellHeightAt(nCurX, nCurY, level) != fCurHeight);
                bDestCellFound = (nCurX == int(fToX) && nCurY == int(fToY));

                if (bHitFound) {
                    nHitPointsFound += 1;

                    // reset current height to new value
                    fCurHeight = cMap.CellHeightAt(nCurX, nCurY, level);
                    // put the collision info in a new IntersectInfo node and push it up the hit list
                    IntersectInfo sInfo;
                    sInfo.fDistFrnt = fDistIfFound;
                    sInfo.fHitX = fFromX + fDistIfFound * fDX;
                    sInfo.fHitY = fFromY + fDistIfFound * fDY;
                    sInfo.nMapCoordX = nCurX;
                    sInfo.nMapCoordY = nCurY;
                    sInfo.fHeight = fCurHeight;
                    sInfo.nLevel = level;
                    vHitList.push_back(sInfo);
                }
            }
        }
        // return whether any hitpoints were found on this level
        return (nHitPointsFound > 0);
    }

    // Returns the projected bottom and top (i.e. the y screen coordinates for them) of a wall block.
    // The wall is at fCorrectedDistToWall from eye point, nHorHight is the height of the horizon, nLevelHeight
    // is the level for this block and fWallHeight is the height of the wall (in blocks) according to the map
    void CalculateWallBottomAndTop2(float fCorrectedDistToWall, int nHorHeight, int nLevelHeight, float fWallHeight, int& nWallTop, int& nWallBottom) {
        // calculate slice height for a *unit height* wall
        int nSliceHeight = int((1.0f / fCorrectedDistToWall) * fDistToProjPlane);
        nWallTop = nHorHeight - (nSliceHeight * (1.0f - fPlayerH)) - (nLevelHeight + fWallHeight - 1.0f) * nSliceHeight;
        nWallBottom = nWallTop + nSliceHeight * fWallHeight;
    }

    // ==============================/   Mini map rendering prototypes   /==============================

    void RenderMapGrid();        // function to render the mini map on the screen
    void RenderMapPlayer();      // function to render the player in the mini map on the screen
    void RenderMapRays();        // function to render the rays in the mini map on the screen
    void RenderMapObjects();     // function to render all the objects in the mini map on the screen
    void RenderDebugInfo();      // function to render debug info in a separate hud on the screen

    bool GetMouseSteering(float& fHorPerc, float& fVerPerc);	    // experimental function for mouse control
    flc::Pixel ShadePixel(const flc::Pixel& p, float fDistance);	// Shade the pixel p using fDistance as a factor in the shade formula

    // Variant on Draw() that takes fDepth and the depth buffer into account.
    // Pixel col is only drawn if fDepth is less than the depth buffer at that screen location (in which case the depth buffer is updated)
    void DrawDepth(float fDepth, int x, int y, flc::Pixel col) {
        // prevent out of bounds drawing
        if (x >= 0 && x < ScreenWidth() &&
            y >= 0 && y < ScreenHeight()) {

            if (fDepth <= fDepthBuffer[y * ScreenWidth() + x]) {
                fDepthBuffer[y * ScreenWidth() + x] = fDepth;
                Draw(x, y, col);
            }
        }
    }

    bool OnUserUpdate(float fElapsedTime) override {

        // step 1 - user input
        // ===================

        // For all movements and rotation you can speed up by keeping SHIFT pressed
        // or speed down by keeping CTRL pressed. This also affects shading/lighting
        float fSpeedUp = 1.0f;
        if (GetKey(flc::SHIFT).bHeld) fSpeedUp = 3.0f;
        if (GetKey(flc::CTRL).bHeld) fSpeedUp = 0.2f;

      

        // reset look up value and player height on pressing 'R'
        if (GetKey(flc::R).bReleased) { fPlayerH = 0.5f; fLookUp = 0.0f; }

        // toggles for HUDs
        if (GetKey(flc::I).bPressed) bDebugInfo = !bDebugInfo;
        if (GetKey(flc::P).bPressed) bMinimap = !bMinimap;
        if (GetKey(flc::O).bPressed) bMapRays = !bMapRays;
  

        // Rotate - collision detection not necessary. Keep fPlayerA_deg between 0 and 360 degrees
        if (GetKey(flc::D).bHeld) { fPlayerA_deg += SPEED_ROTATE * fSpeedUp * fElapsedTime; if (fPlayerA_deg >= 360.0f) fPlayerA_deg -= 360.0f; }
        if (GetKey(flc::A).bHeld) { fPlayerA_deg -= SPEED_ROTATE * fSpeedUp * fElapsedTime; if (fPlayerA_deg < 0.0f) fPlayerA_deg += 360.0f; }

        // variables used for collision detection - work out the new location in a seperate coordinate pair, and only alter
        // the players coordinate if there's no collision
        float fNewX = fPlayerX;
        float fNewY = fPlayerY;

        // walking forward, backward and strafing left, right
        if (GetKey(flc::W).bHeld) { fNewX += lu_cos(fPlayerA_deg) * SPEED_MOVE * fSpeedUp * fElapsedTime; fNewY += lu_sin(fPlayerA_deg) * SPEED_MOVE * fSpeedUp * fElapsedTime; }   // walk forward
        if (GetKey(flc::S).bHeld) { fNewX -= lu_cos(fPlayerA_deg) * SPEED_MOVE * fSpeedUp * fElapsedTime; fNewY -= lu_sin(fPlayerA_deg) * SPEED_MOVE * fSpeedUp * fElapsedTime; }   // walk backwards

        if (GetKey(flc::Q).bHeld) { fNewX += lu_sin(fPlayerA_deg) * SPEED_STRAFE * fSpeedUp * fElapsedTime; fNewY -= lu_cos(fPlayerA_deg) * SPEED_STRAFE * fSpeedUp * fElapsedTime; }   // strafe left
        if (GetKey(flc::E).bHeld) { fNewX -= lu_sin(fPlayerA_deg) * SPEED_STRAFE * fSpeedUp * fElapsedTime; fNewY += lu_cos(fPlayerA_deg) * SPEED_STRAFE * fSpeedUp * fElapsedTime; }   // strafe right
        // collision detection - check if out of bounds or inside non-empty tile
        // only update position if no collision
        if (cMap.IsInBounds(fNewX, fNewY) &&
            // collision detection criterion - is players height > height of map at player's level?
            // Note that int( fPlayerH ) denotes player's level, and (fPlayerH - int( fPlayerH )) denotes
            // player's height within that level
            (fPlayerH > float(cMap.NrOfLayers()) ||
                cMap.CellHeightAt(int(fNewX), int(fNewY), int(fPlayerH)) < (fPlayerH - int(fPlayerH)))) {
            fPlayerX = fNewX;
            fPlayerY = fNewY;
        }

        // looking up or down - collision detection not necessary
        // NOTE - there's no clamping to extreme values (yet)
        if (GetKey(flc::UP).bHeld) { fLookUp += SPEED_LOOKUP * fSpeedUp * fElapsedTime; }
        if (GetKey(flc::DOWN).bHeld) { fLookUp -= SPEED_LOOKUP * fSpeedUp * fElapsedTime; }

        // Mouse control
        if (GetKey(flc::M).bReleased) { bMouseControl = !bMouseControl; }  // toggle on or off
        float fRotFactor, fTiltFactor;
        if (bMouseControl && GetMouseSteering(fRotFactor, fTiltFactor)) {
            fPlayerA_deg += SPEED_ROTATE * fRotFactor * fSpeedUp * fElapsedTime;
            fLookUp -= SPEED_LOOKUP * fTiltFactor * fSpeedUp * fElapsedTime;
        }

        // flying or crouching
        // NOTE - for multi level rendering there's only clamping to keep fPlayerH > 0.0f, there's no upper limit.

        // cache current height of horizon, so that you can compensate for changes in it via the look up value
        float fCacheHorHeight = float(ScreenHeight() * fPlayerH) + fLookUp;
        if (MULTIPLE_LEVELS) {
            // if the player height is adapted, keep horizon height stable by compensating with look up value
            if (GetKey(flc::PGUP).bHeld) {
                float fNewHeight = fPlayerH + SPEED_STRAFE_UP * fSpeedUp * fElapsedTime;
                // do CD on the height map
                if (
                    fNewHeight > float(cMap.NrOfLayers()) ||
                    cMap.CellHeightAt(int(fPlayerX), int(fPlayerY), int(fNewHeight)) < (fNewHeight - int(fNewHeight))
                    ) {
                    fPlayerH = fNewHeight;
                    fLookUp = fCacheHorHeight - float(ScreenHeight() * fPlayerH);
                }
            }
            if (GetKey(flc::PGDN).bHeld) {
                float fNewHeight = fPlayerH - SPEED_STRAFE_UP * fSpeedUp * fElapsedTime;
                // prevent negative height, and do CD on the height map
                if (
                    fNewHeight > 0.0f && (
                        fNewHeight > float(cMap.NrOfLayers()) ||
                        cMap.CellHeightAt(int(fPlayerX), int(fPlayerY), int(fNewHeight)) < (fNewHeight - int(fNewHeight))
                        )
                    ) {
                    fPlayerH = fNewHeight;
                    fLookUp = fCacheHorHeight - float(ScreenHeight() * fPlayerH);
                }
            }
        }
        else {
            if (GetKey(flc::PGUP).bHeld) {
                float fNewHeight = fPlayerH + SPEED_STRAFE_UP * fSpeedUp * fElapsedTime;
                if (fNewHeight < 1.0f) {
                    fPlayerH = fNewHeight;
                    // compensate look up value so that horizon remains stable
                    fLookUp = fCacheHorHeight - float(ScreenHeight() * fPlayerH);
                }
            }
            if (GetKey(flc::PGDN).bHeld) {
                float fNewHeight = fPlayerH - SPEED_STRAFE_UP * fSpeedUp * fElapsedTime;
                if (fNewHeight > 0.0f) {
                    fPlayerH = fNewHeight;
                    // compensate look up value so that horizon remains stable
                    fLookUp = fCacheHorHeight - float(ScreenHeight() * fPlayerH);
                }
            }
        }

        // alter object intensity and multiplier
        if (GetKey(flc::INS).bHeld) fObjectIntensity += INTENSITY_SPEED * fSpeedUp * fElapsedTime;
        if (GetKey(flc::DEL).bHeld) fObjectIntensity -= INTENSITY_SPEED * fSpeedUp * fElapsedTime;
        if (GetKey(flc::HOME).bHeld) fIntensityMultiplier += INTENSITY_SPEED * fSpeedUp * fElapsedTime;
        if (GetKey(flc::END).bHeld) fIntensityMultiplier -= INTENSITY_SPEED * fSpeedUp * fElapsedTime;

        // step 2 - game logic
        // ===================

        // update all objects
        for (auto& elt : vListObjects) {
            elt.Update(cMap, fElapsedTime);
        }

        // step 3 - render
        // ===============

        // BACK GROUND SCENE RENDERING
        // ===========================

        // typically, the horizon height is halfway the screen height. However, you have to offset with look up value,
        // and the viewpoint of the player is variable too (since flying and crouching)
        int nHorizonHeight = ScreenHeight() * fPlayerH + (int)fLookUp;
        float fAngleStep = fPlayerFoV_deg / float(ScreenWidth());

        // iterate over all screen slices, processing the screen in columns
        for (int x = 0; x < ScreenWidth(); x++) {
            float fViewAngle = float(x - (ScreenWidth() / 2)) * fAngleStep;
            float fCurAngle = fPlayerA_deg + fViewAngle;

            float fX_hit, fY_hit;        // to hold exact (float) hit location (world space)
            int   nX_hit, nY_hit;        // to hold coords of tile that was hit (tile space)

            int   nWallTop, nWallTop2;   // to store the top and bottom y coord of the wall per column (screen space)
            int   nWallBot, nWallBot2;   // the ...2 variant represents the back of the current block

            // This lambda performs much of the sampling proces of horizontal surfaces. It can be used for floors, roofs and ceilings etc.
            // fProjDistance is the distance from the player to the hit point on the surface.
            auto generic_sampling = [=](float fProjDistance, flc::Sprite* cTexturePtr) -> flc::Pixel {
                // calculate the world coordinates from the distance and the view angle + player angle
                float fProjX = fPlayerX + fProjDistance * lu_cos(fCurAngle);
                float fProjY = fPlayerY + fProjDistance * lu_sin(fCurAngle);
                // calculate the sample coordinates for that world coordinate, by subtracting the
                // integer part and only keeping the fractional part. Wrap around if the result < 0 or > 1
                float fSampleX = fProjX - int(fProjX); if (fSampleX < 0.0f) fSampleX += 1.0f; if (fSampleX >= 1.0f) fSampleX -= 1.0f;
                float fSampleY = fProjY - int(fProjY); if (fSampleY < 0.0f) fSampleY += 1.0f; if (fSampleY >= 1.0f) fSampleY -= 1.0f;
                // having both sample coordinates, use the texture pointer to get the sample, and shade and return it
                return ShadePixel(cTexturePtr->Sample(fSampleX, fSampleY), fProjDistance);
                };


            // This lambda performs much of the sampling proces of horizontal surfaces. It can be used for floors, roofs and ceilings etc.
          // fProjDistance is the distance from the player to the hit point on the surface.
            auto generic_sampling_new = [=](float fProjDistance, int nLevel, int nFaceID) -> flc::Pixel {
                // calculate the world coordinates from the distance and the view angle + player angle
                float fProjX = fPlayerX + fProjDistance * lu_cos(fCurAngle);
                float fProjY = fPlayerY + fProjDistance * lu_sin(fCurAngle);
                // calculate the sample coordinates for that world coordinate, by subtracting the
                // integer part and only keeping the fractional part. Wrap around if the result < 0 or > 1
                float fSampleX = fProjX - int(fProjX); if (fSampleX < 0.0f) fSampleX += 1.0f; if (fSampleX >= 1.0f) fSampleX -= 1.0f;
                float fSampleY = fProjY - int(fProjY); if (fSampleY < 0.0f) fSampleY += 1.0f; if (fSampleY >= 1.0f) fSampleY -= 1.0f;

                // select the sprite to render the ceiling depending on the block that was hit
                int nTileX = std::clamp(int(fProjX), 0, cMap.Width() - 1);
                int nTileY = std::clamp(int(fProjY), 0, cMap.Height() - 1);
                char cBlockType = cMap.CellValueAt(nTileX, nTileY, nLevel);
                BlockType& sBlockHit = bk.GetBlock(cBlockType);
                int nSpriteIx = bk.GetBlockFaceIx(sBlockHit, nFaceID);
                flc::Sprite* cTexturePtr = nullptr;
                switch (nFaceID) {
                case FACE_TOP: cTexturePtr = RendRoofSprites[nSpriteIx].Sprite(); break;
                case FACE_BOTTOM: cTexturePtr = RendCeilSprites[nSpriteIx].Sprite(); break;
                default: std::cout << "ERROR: generic_sampling_new() --> invalid face id: " << nFaceID << std::endl;
                }

                // having the sample coordinates and the texture pointer, get the sample, and shade and return it
                return ShadePixel(cTexturePtr->Sample(fSampleX, fSampleY), fProjDistance);
                };


            // this lambda returns a sample of the floor through the pixel at screen coord (px, py)
            auto get_floor_sample = [=](int px, int py) -> flc::Pixel {
                // work out the distance to the location on the floor you are looking at through this pixel
                float fFloorProjDistance = ((fPlayerH / float(py - nHorizonHeight)) * fDistToProjPlane) / lu_cos(fViewAngle);
                // call the generic sampler to work out the rest
                return generic_sampling(fFloorProjDistance, RendFloorSprites[0].Decal()->m_sprite);
                };

            // this lambda returns a sample of the roof through the pixel at screen coord (px, py)
            // NOTE: fHeightWithinLevel denotes the height of the hit point on the roof. This is typically the height of the block + its level
            auto get_roof_sample = [=](int px, int py, int nLevel, float fHeightWithinLevel, float& fRoofProjDistance) -> flc::Pixel {
                // work out the distance to the location on the roof you are looking at through this pixel
                // (the pixel is given since you know the x and y to draw to)
                fRoofProjDistance = (((fPlayerH - (float(nLevel) + fHeightWithinLevel)) / float(py - nHorizonHeight)) * fDistToProjPlane) / lu_cos(fViewAngle);
                // call the generic sampler to work out the rest
                return generic_sampling_new(fRoofProjDistance, nLevel, FACE_TOP);
                };

            // this lambda returns a sample of the ceiling through the pixel at screen coord (px, py)
            // NOTE: fHeightWithinLevel denotes the height of the hit point on the ceiling. This is typically the level of the block, WITHOUT its height!
            auto get_ceil_sample = [=](int px, int py, int nLevel, float fHeightWithinLevel, float& fCeilProjDistance) -> flc::Pixel {
                // work out the distance to the location on the ceiling you are looking at through this pixel
                // (the pixel is given since you know the x and y screen coordinate to draw to)
                fCeilProjDistance = (((float(nLevel) - fPlayerH) / float(nHorizonHeight - py)) * fDistToProjPlane) / lu_cos(fViewAngle);
                // call the generic sampler to work out the rest
                return generic_sampling_new(fCeilProjDistance, nLevel, FACE_BOTTOM);
                };

            // prepare the rendering for this slice by calculating the list of intersections in this ray's direction
            // for each level, get the list of hitpoints in that level, work out front and back distances and projections
            // on screen, and add to the global vHitPointList
            std::vector<IntersectInfo> vHitPointList;
            for (int k = 0; k < cMap.NrOfLayers(); k++) {

                std::vector<IntersectInfo> vCurLevelList;
                GetDistancesToWallsPerLevel(k, fCurAngle, vCurLevelList);

                for (int i = 0; i < (int)vCurLevelList.size(); i++) {
                    // make correction for the fish eye effect
                    vCurLevelList[i].fDistFrnt *= lu_cos(fViewAngle);
                    // calculate values for the on screen projections top_front and top_bottom
                    CalculateWallBottomAndTop2(
                        vCurLevelList[i].fDistFrnt,
                        nHorizonHeight,
                        vCurLevelList[i].nLevel,
                        vCurLevelList[i].fHeight,
                        vCurLevelList[i].top_front,
                        vCurLevelList[i].bot_front
                    );
                }
                // Extend the hit list with projected ceiling info for the back of the wall
                for (int i = 0; i < (int)vCurLevelList.size(); i++) {
                    if (i == (int)vCurLevelList.size() - 1) {
                        // last element, has no successor
                        vCurLevelList[i].fDistBack = vCurLevelList[i].fDistFrnt;
                        vCurLevelList[i].top_back = vCurLevelList[i].top_front;
                        vCurLevelList[i].bot_back = vCurLevelList[i].bot_front;
                    }
                    else {
                        // calculate values for the on screen projections top_front and top_bottom
                        vCurLevelList[i].fDistBack = vCurLevelList[i + 1].fDistFrnt;
                        CalculateWallBottomAndTop2(
                            vCurLevelList[i].fDistBack,
                            nHorizonHeight,
                            vCurLevelList[i].nLevel,
                            vCurLevelList[i].fHeight,
                            vCurLevelList[i].top_back,
                            vCurLevelList[i].bot_back
                        );
                    }
                }
                // add the hit points for this level list to the combined hit point list
                vHitPointList.insert(vHitPointList.end(), vCurLevelList.begin(), vCurLevelList.end());
            }

            // populate ray list for rendering mini map
            if (bMinimap && !vHitPointList.empty()) {
                flc::vf2d curHitPoint = { vHitPointList[0].fHitX, vHitPointList[0].fHitY };
                vRayList.push_back(curHitPoint);
            }

            // remove all hit points with height 0.0f - they were necessary for calculating the back face projection
            // of blocks, but that part is done now
            vHitPointList.erase(
                std::remove_if(
                    vHitPointList.begin(),
                    vHitPointList.end(),
                    [](IntersectInfo& a) {
                        return a.fHeight == 0.0f;
                    }
                ),
                vHitPointList.end()
            );

            // sort hit points from far away to close by (painters algorithm)
            std::sort(
                vHitPointList.begin(),
                vHitPointList.end(),
                [](IntersectInfo& a, IntersectInfo& b) {
                    return (a.fDistFrnt > b.fDistFrnt) ||
                        (a.fDistFrnt == b.fDistFrnt && a.nLevel < b.nLevel);
                }
            );

            // start rendering this slice by putting sky and floor in it
            for (int y = ScreenHeight() - 1; y >= 0; y--) {
                // reset depth buffer for this slice
                fDepthBuffer[y * ScreenWidth() + x] = fMaxDistance;
                // draw floor and horizon
                if (y < nHorizonHeight) {
                    flc::Pixel skySample = flc::CYAN;
                    DrawDepth(fMaxDistance, x, y, skySample);
                }
                else {
                    flc::Pixel floorSample = get_floor_sample(x, y);   // shading is done in get_floor_sample()
                    DrawDepth(fMaxDistance, x, y, floorSample);
                }
            }

            // now render all hit points back to front
            for (auto& hitRec : vHitPointList) {

                float fBlockElevation = 1.0f;
                int   nBlockLevel = 0;
                float fFrntDistance = 0.0f;     // distance var also used for wall shading
                float fBackDistance = 0.0f;

                // For the distance calculations we needed also points where the height returns to 0.0f (the
                // back faces of the block). For the rendering we must skip these "hit points"
                if (hitRec.fHeight > 0.0f) {
                    // load the info from next hit point
                    fX_hit = hitRec.fHitX;
                    fY_hit = hitRec.fHitY;
                    nX_hit = hitRec.nMapCoordX;
                    nY_hit = hitRec.nMapCoordY;
                    fBlockElevation = hitRec.fHeight;
                    nBlockLevel = hitRec.nLevel;
                    fFrntDistance = hitRec.fDistFrnt;
                    fBackDistance = hitRec.fDistBack;
                    // make sure the screen y coordinate is within screen boundaries
                    nWallTop = std::clamp(hitRec.top_front, 0, ScreenHeight() - 1);
                    nWallTop2 = std::clamp(hitRec.top_back, 0, ScreenHeight() - 1);
                    nWallBot = std::clamp(hitRec.bot_front, 0, ScreenHeight() - 1);
                    nWallBot2 = std::clamp(hitRec.bot_back, 0, ScreenHeight() - 1);

                    // render roof segment if it's visible
                   // render roof segment if it's visible
                    for (int y = nWallTop2; y < nWallTop; y++) {
                        // the distance to this point is calculated and passed from get_roof_sample
                        float fRenderDistance;
                        flc::Pixel roofSample = get_roof_sample(x, y, nBlockLevel, fBlockElevation, fRenderDistance);   // shading is done in get_roof_sample()
                        DrawDepth(fRenderDistance, x, y, roofSample);
                    }
                    // render wall segment - make sure that computational expensive calculations are only done once
                    int nSide = -1;
                    float fSampleX;
                    for (int y = nWallTop; y <= nWallBot; y++) {

                        if (nSide == -1) {
                            // the x sample coordinate takes more work to figure out. You need to check what side of the
                            // block was hit
                            float fBlockMidX = (float)nX_hit + 0.5f;   // location of middle of the cell that was hit
                            float fBlockMidY = (float)nY_hit + 0.5f;
                            // determine in what quadrant the hit location is wrt the block mid point
                            float fTestAngle = atan2f((fY_hit - fBlockMidY), (fX_hit - fBlockMidX));

                            // I tested several supposedly faster approximations for atan2f(), but the results are really not significant
                            // The major bottleneck here is that this analysis is done for each separate pixel in the slice:
                            // for now I solved this using a flag (bSampleX_known)

                            //   * possible improvement 1: determine the ranges within a slice so that you don't have to repeat the atan2f() call for each pixel
                            //   * possible improvement 2: (after 1) render these slice parts as scaled decals
                            if (-0.75f * PI <= fTestAngle && fTestAngle < -0.25f * PI) { fSampleX = fX_hit - (float)nX_hit; nSide = FACE_SOUTH; } // south side
                            if (-0.25f * PI <= fTestAngle && fTestAngle < 0.25f * PI) { fSampleX = fY_hit - (float)nY_hit; nSide = FACE_EAST; } // east  side
                            if (0.25f * PI <= fTestAngle && fTestAngle < 0.75f * PI) { fSampleX = fX_hit - (float)nX_hit; nSide = FACE_NORTH; } // north side
                            if (-0.75f * PI > fTestAngle || fTestAngle >= 0.75f * PI) { fSampleX = fY_hit - (float)nY_hit; nSide = FACE_WEST; } // west  side
                        }

                        float fSampleY;
                        // the y sample coordinate depends only on the pixel y coord on the screen
                        // in relation to the vertical space the wall is taking up
                        fSampleY = fBlockElevation * float(y - hitRec.top_front) / float(hitRec.bot_front - hitRec.top_front);

                        // select the sprite to render the wall depending on the block that was hit
                        char cBlockType = cMap.CellValueAt(nX_hit, nY_hit, nBlockLevel);
                        BlockType& sBlockHit = bk.GetBlock(cBlockType);
                        int nWallSpriteIx = bk.GetBlockFaceIx(sBlockHit, nSide);
                        flc::Sprite* pSpritePtr = RendWallSprites[nWallSpriteIx].Decal()->m_sprite;
                       
                        

                        // having the sample coordinates and the sprite pointer, get the sample, shade it and draw the pixel
                        flc::Pixel wallSample = ShadePixel(pSpritePtr->Sample(fSampleX, fSampleY), fFrntDistance);
                        DrawDepth(fFrntDistance, x, y, wallSample);
                    }
                    // render ceiling segment if it's visible
                    for (int y = nWallBot + 1; y <= nWallBot2; y++) {
                        float fRenderDistance;
                        flc::Pixel ceilSample = get_ceil_sample(x, y, nBlockLevel, fBlockElevation, fRenderDistance);   // shading is done in get_ceil_sample()
                        DrawDepth(fRenderDistance, x, y, ceilSample);
                    }
                }
            }
           
        }

        // OBJECT RENDERING
        // ================

        // display all objects after the background rendering and before displaying the minimap or debugging output
        float fPlayerFoV_rad = deg2rad(fPlayerFoV_deg);
        // split the rendering into two phase so that it can be sorted on distance (painters algo) before rendering

        // phase 1 - just determine distance (and angle cause of convenience)
        for (auto& object : vListObjects) {
            // can object be seen?
            float fVecX = object.GetX() - fPlayerX;
            float fVecY = object.GetY() - fPlayerY;
            object.SetDistToPlayer(sqrtf(fVecX * fVecX + fVecY * fVecY));
            // calculate angle between vector from player to object, and players looking direction
            // to determine if object is in players field of view
            float fEyeX = lu_cos(fPlayerA_deg);
            float fEyeY = lu_sin(fPlayerA_deg);
            float fObjA = atan2f(fVecY, fVecX) - atan2f(fEyeY, fEyeX);
            // "bodge" angle into range [ -PI, PI ]
            if (fObjA < -PI) fObjA += 2.0f * PI;
            if (fObjA > PI) fObjA -= 2.0f * PI;
            object.SetAngleToPlayer(fObjA);

            // experiment
            object.SetDistToPlayer(object.GetDistToPlayer() * lu_cos(fObjA));
        }

        // sort for painters algo
        vListObjects.sort(
            [=](RC_Object& a, RC_Object& b) {
                return a.GetDistToPlayer() > b.GetDistToPlayer();
            }
        );

        // phase 2: render it from large to smaller distances
        for (auto& object : vListObjects) {
            // can object be seen?
            float fObjDist = object.GetDistToPlayer();
            float fObjA = object.GetAngle();
            // determine whether object is in field of view (a bit larger to prevent objects being not rendered at
            // screen boundaries)
            bool bInFOV = fabs(fObjA) < fPlayerFoV_rad / 1.2f;

            // render object only when within Field of View, and within visible distance.
            // the check on proximity is to prevent asymptotic errors when distance to player becomes very small
            if (bInFOV && fObjDist >= 0.3f && fObjDist < fMaxDistance) {

                // determine the difference between standard player height (i.e. 0.5f = standing on the floor)
                // and current player height
                float fCompensatePlayerHeight = fPlayerH - 0.5f;
                // get the projected (halve) slice height of this object
                float fObjHlveSliceHeight = float(ScreenHeight() / fObjDist);
                float fObjHlveSliceHeightScld = float((ScreenHeight() * object.GetScale()) / fObjDist);

                // work out where objects floor and ceiling are (in screen space)
                // due to scaling factor, differentiated a normalized (scale = 1.0f) ceiling and a scaled variant
                float fObjCeilingNormalized = float(nHorizonHeight) - fObjHlveSliceHeight;
                float fObjCeilingScaled = float(nHorizonHeight) - fObjHlveSliceHeightScld;
                // and adapt all the scaling into the ceiling value
                float fScalingDifference = fObjCeilingNormalized - fObjCeilingScaled;
                float fObjCeiling = fObjCeilingNormalized - 2 * fScalingDifference;
                float fObjFloor = float(nHorizonHeight) + fObjHlveSliceHeight;

                // compensate object projection heights for elevation of the player
                fObjCeiling += fCompensatePlayerHeight * fObjHlveSliceHeight * 2.0f;
                fObjFloor += fCompensatePlayerHeight * fObjHlveSliceHeight * 2.0f;

                // get height, aspect ratio and width
                float fObjHeight = fObjFloor - fObjCeiling;
                float fObjAR = float(object.GetSprite()->height) / float(object.GetSprite()->width);
                float fObjWidth = fObjHeight / fObjAR;
                // work out where the object is across the screen width
                float fMidOfObj = (0.5f * (fObjA / (fPlayerFoV_rad / 2.0f)) + 0.5f) * float(ScreenWidth());

                // render the sprite
                for (float fx = 0.0f; fx < fObjWidth; fx++) {
                    // get distance across the screen to render
                    int nObjColumn = int(fMidOfObj + fx - (fObjWidth / 2.0f));
                    // only render this column if it's on the screen
                    if (nObjColumn >= 0 && nObjColumn < ScreenWidth()) {
                        for (float fy = 0.0f; fy < fObjHeight; fy++) {
                            // calculate sample coordinates as a percentage of object width and height
                            float fSampleX = fx / fObjWidth;
                            float fSampleY = fy / fObjHeight;
                            // sample the pixel and draw it
                            flc::Pixel objSample = ShadePixel(object.GetSprite()->Sample(fSampleX, fSampleY), fObjDist);
                            if (objSample != flc::BLACK) {
                                DrawDepth(fObjDist, nObjColumn, fObjCeiling + fy, objSample);
                            }
                        }
                    }
                }
            }
        }

      

        if (bMinimap) {
            RenderMapGrid();
            if (bMapRays) {
                RenderMapRays();
            }
            RenderMapPlayer();
            RenderMapObjects();

            vRayList.clear();
        }

        if (bDebugInfo) {
            RenderDebugInfo();
        }

        return true;
    }

    bool OnUserDestroy() override
    {
        //test loading renderable sprites
       //wall
        for (int i = 0; i < RendWallSprites.size(); i++)
        {
            RendWallSprites[i].Clean();
        }

        //ceiling
        for (int i = 0; i < RendCeilSprites.size(); i++)
        {
            RendCeilSprites[i].Clean();
        }

        //floor
        for (int i = 0; i < RendFloorSprites.size(); i++)
        {
            RendFloorSprites[i].Clean();
        }

        //roof
        for (int i = 0; i < RendRoofSprites.size(); i++)
        {
            RendRoofSprites[i].Clean();
        }

        return true;
    }
};




// ==============================/   put bloat behind main()  /==============================

// ==============================/   Mini map rendering stuff   /==============================

// function to render the mini map on the screen
void Game::RenderMapGrid() {
    // fill background for minimap
    float fMMFactor = MINIMAP_SCALE_FACTOR * MINIMAP_TILE_SIZE;
    FillRect(0, 0, cMap.Width() * fMMFactor, cMap.Height() * fMMFactor, flc::BLUE);
    // draw each tile
    for (int y = 0; y < cMap.Height(); y++) {
        for (int x = 0; x < cMap.Width(); x++) {
            // colour different for different heights
            flc::Pixel p;
            bool bBorderFlag = true;
            if (cMap.CellHeight(x, y) == 0.0f) {
                p = flc::BLUE;   // don't visibly render
                bBorderFlag = false;
            }
            else if (cMap.CellHeight(x, y) < 1.0f) {
                p = flc::Pixel(cMap.CellHeight(x, y), 0.0f, 0.0f);    // height < 1.0f = shades of red
            }
            else {
                float fColFactor = std::min(cMap.CellHeight(x, y) / 4.0f + 0.5f, 1.0f);    // heights > 1.0f = shades of blue
                p = flc::Pixel(0.0f, 0.0f, fColFactor);
            }
            // render this tile
            FillRect(x * fMMFactor + 1, y * fMMFactor + 1, fMMFactor - 1, fMMFactor - 1, p);
            if (bBorderFlag) {
                p = flc::WHITE;
                DrawRect(x * fMMFactor, y * fMMFactor, fMMFactor, fMMFactor, p);
            }
        }
    }
}

// function to render the player in the mini map on the screen
void Game::RenderMapPlayer() {
    float fMMFactor = MINIMAP_TILE_SIZE * MINIMAP_SCALE_FACTOR;
    flc::Pixel p = flc::YELLOW;
    float px = fPlayerX * fMMFactor;
    float py = fPlayerY * fMMFactor;
    float pr = 0.6f * fMMFactor;
    FillCircle(px, py, pr, p);
    float dx = lu_cos(fPlayerA_deg);
    float dy = lu_sin(fPlayerA_deg);
    float pdx = dx * 2.0f * fMMFactor;
    float pdy = dy * 2.0f * fMMFactor;
    DrawLine(px, py, px + pdx, py + pdy, p);
}

// function to render the rays in the mini map on the screen
void Game::RenderMapRays() {
    float fMMFactor = MINIMAP_TILE_SIZE * MINIMAP_SCALE_FACTOR;
    for (auto& elt : vRayList) {
        DrawLine(
            fPlayerX * fMMFactor,
            fPlayerY * fMMFactor,
            elt.x * fMMFactor,
            elt.y * fMMFactor,
            flc::GREEN
        );
    }
}

// function to render all the objects in the mini map on the screen
void Game::RenderMapObjects() {
    float fMMFactor = MINIMAP_TILE_SIZE * MINIMAP_SCALE_FACTOR;
    flc::Pixel p = flc::RED;
    for (auto& elt : vListObjects) {
        float px = elt.GetX() * fMMFactor;
        float py = elt.GetY() * fMMFactor;
        float pr = 0.4f * fMMFactor;
        FillCircle(px, py, pr, p);
    }
}

// function to render debug info in a separate hud on the screen
void Game::RenderDebugInfo() {
    int nStartX = ScreenWidth() - 200;
    int nStartY = 10;
    // render background pane for debug info
    FillRect(nStartX, nStartY, 195, 85, flc::BLUE);
    // output player and rendering values for debugging
    DrawString(nStartX + 5, nStartY + 5, "fPlayerX = " + std::to_string(fPlayerX), flc::BLACK);
    DrawString(nStartX + 5, nStartY + 15, "fPlayerY = " + std::to_string(fPlayerY), flc::BLACK);
    DrawString(nStartX + 5, nStartY + 25, "fPlayerA = " + std::to_string(fPlayerA_deg), flc::BLACK);
    DrawString(nStartX + 5, nStartY + 35, "fPlayerH = " + std::to_string(fPlayerH), flc::BLACK);
    DrawString(nStartX + 5, nStartY + 45, "fLookUp  = " + std::to_string(fLookUp), flc::BLACK);
    DrawString(nStartX + 5, nStartY + 65, "Intensity  = " + std::to_string(fObjectIntensity), flc::BLACK);
    DrawString(nStartX + 5, nStartY + 75, "Multiplier = " + std::to_string(fIntensityMultiplier), flc::BLACK);
}

// experimental function for mouse control
bool Game::GetMouseSteering(float& fHorPerc, float& fVerPerc) {
    // grab mouse coordinates
    int nMouseX = GetMouseX();
    int nMouseY = GetMouseY();

    // express mouse coords in -1.0f, 1.0f range
    float fRangeX = (nMouseX - (ScreenWidth() / 2)) / float(ScreenWidth() / 2);
    float fRangeY = (nMouseY - (ScreenHeight() / 2)) / float(ScreenHeight() / 2);

    // the screen width / height is mapped onto [ -1.0, +1.0 ] range
    // the range [ -0.2f, +0.2f ] is the stable (inactive) zone
    fHorPerc = 0.0f;
    fVerPerc = 0.0f;
    // if outside the stable zone, map to [ -1.0f, +1.0f ] again
    if (fRangeX < -0.2f) fHorPerc = (fRangeX + 0.2f) * (1.0f / 0.8f);
    if (fRangeX > 0.2f) fHorPerc = (fRangeX - 0.2f) * (1.0f / 0.8f);
    if (fRangeY < -0.2f) fVerPerc = (fRangeY + 0.2f) * (1.0f / 0.8f);
    if (fRangeY > 0.2f) fVerPerc = (fRangeY - 0.2f) * (1.0f / 0.8f);

    return (fHorPerc != 0.0f || fVerPerc != 0.0f);
}

// Shade the pixel p using fDistance as a factor in the shade formula
flc::Pixel Game::ShadePixel(const flc::Pixel& p, float fDistance) {
    if (RENDER_SHADED) {
        float fShadeFactor = std::max(SHADE_FACTOR_MIN, std::min(SHADE_FACTOR_MAX, fObjectIntensity * (fIntensityMultiplier / fDistance)));
        return p * fShadeFactor;
    }
    else
        return p;
}





#endif // !GAME_H



