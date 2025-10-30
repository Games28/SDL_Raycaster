#include "Raycast.h"

Raycast::Raycast()
{
    pWallSprite = new flc::Sprite("colorstone.png");
    pFloorSprite = new flc::Sprite("wood.png");
    pCeilSprite = new flc::Sprite("mossystone.png");
}

Raycast::~Raycast()
{
}

bool Raycast::GetDistanceToWall(Player& player, Map& map, float fRayAngle, float& fHitX, float& fHitY, float& fDistIfFound, int& nMapCoordX, int& nMapCoordY)
{
    float fFromX = player.getPlayerX();
    float fFromY = player.getPlayerY();
    // Calculate the "to point" using it's angle and fMaxDistance
    float fToX = player.getPlayerX() + map.getMaxDistance() * cos(fRayAngle * PI / 180.0f);
    float fToY = player.getPlayerY() + map.getMaxDistance() * sin(fRayAngle * PI / 180.0f);
    // work out the direction vector (fDX, fDY) and normalize it
    float fDX = fToX - fFromX;
    float fDY = fToY - fFromY;
    float fRayLen = sqrt(fDX * fDX + fDY * fDY);
    fDX /= fRayLen;
    fDY /= fRayLen;

    // calculate the scaling factors for the ray increments per unit in x resp y direction
    // this calculation takes division by 0.0f into account
    float fSX = (fDX == 0.0f) ? FLT_MAX : sqrt(1.0f + (fDY / fDX) * (fDY / fDX));
    float fSY = (fDY == 0.0f) ? FLT_MAX : sqrt(1.0f + (fDX / fDY) * (fDX / fDY));

    float fLengthPartialRayX = 0.0f;
    float fLengthPartialRayY = 0.0f;
    // work out if line is going right or left resp. down or up
    int nGridStepX = (fDX > 0.0f) ? +1 : -1;
    int nGridStepY = (fDY > 0.0f) ? +1 : -1;
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

    // did analysis get out of map boundaries?
    bool bOutOfBounds = (nCurX < 0 || nCurX >= map.getMapX() ||
        nCurY < 0 || nCurY >= map.getMapY());
    // was a hit with a wall cell found?
    bool bHitFound = bOutOfBounds ? false : map.getMapIndex(nCurX,nCurY) != '.';
    // did analysis reach the destination cell?
    bool bDestCellFound = (nCurX == int(fToX) && nCurY == int(fToY));

    fDistIfFound = 0.0f;
    while (!bOutOfBounds && !bHitFound && !bDestCellFound && fDistIfFound < map.getMaxDistance()) {

        // advance to next map cell, depending on length of partial ray's
        if (fLengthPartialRayX < fLengthPartialRayY) {
            // move in x direction
            nCurX += nGridStepX;
            fDistIfFound = fLengthPartialRayX;
            fLengthPartialRayX += fSX;
        }
        else {
            // move in y direction
            nCurY += nGridStepY;
            fDistIfFound = fLengthPartialRayY;
            fLengthPartialRayY += fSY;
        }

        bOutOfBounds = (nCurX < 0 || nCurX >= map.getMapX() ||
            nCurY < 0 || nCurY >= map.getMapY());
        bHitFound = bOutOfBounds ? false : map.getMapIndex(nCurX, nCurY) != '.';
        bDestCellFound = (nCurX == int(fToX) && nCurY == int(fToY));
    }

    if (bHitFound) {
        // return the correct values - note that fDistIfFound already has the correct value
        fHitX = fFromX + fDistIfFound * fDX;
        fHitY = fFromY + fDistIfFound * fDY;
        nMapCoordX = nCurX;
        nMapCoordY = nCurY;
    }
    else {
        // make sure ref. variables don't suggest meaningful values
        fHitX = -1.0f;
        fHitY = -1.0f;
        nMapCoordX = -1;
        nMapCoordY = -1;
        fDistIfFound = 0.0f;
    }

    return bHitFound;
}

void Raycast::Render(Player& player, Map& map, flc::SDL_GameEngine& sge)
{
    int nHalfScreenWidth = WINDOW_WIDTH / 2;
    float fAngleStep = fPlayerFoV_deg / float(WINDOW_WIDTH);

    // iterate over all screen slices, processing the screen in columns
    for (int x = 0; x < WINDOW_WIDTH; x++) {
        float fViewAngle = float(x - nHalfScreenWidth) * fAngleStep;
        float fCurAngle = player.getPlayerAngle() + fViewAngle;

        float fRawDistToWall, fCorrectDistToWall;
        float fX_hit, fY_hit;   // to hold exact (float) hit location
        int   nX_hit, nY_hit;   // to hold coords of tile that was hit

        int nWallCeil, nWallFloor;   // to store the top and bottom y coord of the wall per column

        float fLighting = 1.0f;
        if (GetDistanceToWall(player, map,fCurAngle, fX_hit, fY_hit, fRawDistToWall, nX_hit, nY_hit)) {
            // a wall was hit - set bottom and top value depending on the distance found

            // little lambda expression to compare float values
            auto my_float_equal = [=](float a, float b) -> bool {
                float fEpsilon = 0.00001f;
                return abs(a - b) < fEpsilon;
                };

            // determine shading factor depending on which face was hit
           if (my_float_equal(fX_hit, float(nX_hit))) { fLighting = 1.0f; }
           else    // west  face was hit
               if (my_float_equal(fY_hit, float(nY_hit))) { fLighting = 0.8f; }
               else    // north
                   if (my_float_equal(fX_hit, float(nX_hit + 1))) { fLighting = 0.6f; }
                   else    // east
                       if (my_float_equal(fY_hit, float(nY_hit + 1))) { fLighting = 0.4f; }
                       else {  // south
                           std::cout << "ERROR: OnUserUpdate() --> this situation should not occur" << std::endl;
                           fLighting = 0.2f;
                       }

            // make correction for the fish eye effect
            fCorrectDistToWall = fRawDistToWall * cos(fViewAngle * PI / 180.0f);
            int nSliceHeight = int((1.0f / fCorrectDistToWall) * map.getDistToProjPlane());
            nWallCeil = (WINDOW_HEIGHT / 2.0f) - (nSliceHeight / 2.0f);
            nWallFloor = (WINDOW_HEIGHT / 2.0f) + (nSliceHeight / 2.0f);
        }
        else {
            // no wall was hit - set bottom and top value for wall at the horizon
            nWallCeil = WINDOW_HEIGHT / 2;
            nWallFloor = WINDOW_HEIGHT / 2;
        }

        // fill column with pixels
        for (int y = 0; y < WINDOW_HEIGHT; y++) {
            if (y < nWallCeil) {
                sge.Draw(x, y, COL_CEIL);                    // render ceiling
            }
            else if (y > nWallFloor) {
                sge.Draw(x, y, COL_FLOOR);                   // render floor
            }
            else {
                float fSampleY = float(y - nWallCeil) / float(nWallFloor - nWallCeil);
                float fSampleX;
                float fBlockMidX = (float)nX_hit + 0.5f;   // location of middle of the cell that was hit
                float fBlockMidY = (float)nY_hit + 0.5f;
                // determine in what quadrant the hit location is wrt the block mid point
                float fTestAngle = atan2f((fY_hit - fBlockMidY), (fX_hit - fBlockMidX));
                if (-0.75f * PI <= fTestAngle && fTestAngle < -0.25f * PI) fSampleX = fX_hit - (float)nX_hit;  // south side
                if (-0.25f * PI <= fTestAngle && fTestAngle < 0.25f * PI) fSampleX = fY_hit - (float)nY_hit;  // east  side
                if (0.25f * PI <= fTestAngle && fTestAngle < 0.75f * PI) fSampleX = fX_hit - (float)nX_hit;  // north side
                if (-0.75f * PI > fTestAngle || fTestAngle >= 0.75f * PI) fSampleX = fY_hit - (float)nY_hit;  // west  side

                flc::Pixel auxSample = pWallSprite->Sample(fSampleX, fSampleY);

                sge.Draw(x, y, auxSample);        // render wall
            }
        }
    }
}
