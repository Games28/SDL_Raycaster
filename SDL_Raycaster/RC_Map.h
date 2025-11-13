#pragma once
#ifndef RC_MAP_H
#define RC_MAP_H
#include <string>
#include <vector>
#include <iostream>
#include "RC_Block.h"

class RC_Map {

public:
    RC_Map() {
        InitMap(0, 0);
       
    }
    
   
    // First initialize the map calling this method ...
    void InitMap(int nSizeX, int nSizeY) {
        nMapX = nSizeX;
        nMapY = nSizeY;
    }

    // ... then add at least 1 layer to it using this method
    void AddLayer(const std::string& sUserMap, Blocks& b) {
        if (nMapX * nMapY != (int)sUserMap.length()) {
            std::cout << "ERROR: InitMap() -->  mismatch between map dimensions and length of map string" << std::endl;
        }
        // temporary character map to add current layer
        std::string sMap = sUserMap;

        // Initialise fMap as a 2d array of floats, having the same size as sMap, and containing the height per cell.
        // NOTE - if MULTIPLE_LEVELS is false, the fMap will contain no values > 1
        float* fMap = new float[nMapX * nMapY];
        for (int y = 0; y < nMapY; y++) {
            for (int x = 0; x < nMapX; x++) {
                BlockType& refBlock = b.GetBlock(sMap[y * nMapX + x]);
                fMap[y * nMapX + x] = refBlock.fHeight;
            }
        }
        sMaps.push_back(sMap);
        fMaps.push_back(fMap);
    }

    // method to clean up the object before it gets out of scope
    void FinalizeMap() {
        for (auto& elt : fMaps) {
            delete elt;
        }
        sMaps.clear();
        fMaps.clear();
    }

    // getters for map width and height
    int Width() { return nMapX; }
    int Height() { return nMapY; }

    // returns whether (x, y) is within map boundaries
    bool IsInBounds(float x, float y) {
        return (x >= 0 && x < nMapX && y >= 0 && y < nMapY);
    }

    // getter for (cumulated) cell height at coordinates (x, y)
    // Note - there's no intuitive meaning for this method in maps with holes
    float CellHeight(int x, int y) {
        float result = -1.0f;
        if (!IsInBounds(x, y)) {
            std::cout << "ERROR: CellHeight() --> map indices out of bounds: (" << x << ", " << y << ")" << std::endl;
        }
        else {
            result = 0.0f;
            for (int i = 0; i < (int)fMaps.size(); i++) {
                result += fMaps[i][y * nMapX + x];
            }
        }
        return result;
    }

    // getter for obtaining height value of the cell at level, coordinates (x, y)
    float CellHeightAt(int x, int y, int level) {
        float result = -1.0f;
        if (!IsInBounds(x, y)) {
            std::cout << "ERROR: CellHeightAt() --> map indices out of bounds: (" << x << ", " << y << ")" << std::endl;
        }
        else if (level >= (int)fMaps.size()) {
            std::cout << "ERROR: CellHeightAt() --> level argument out of range: " << level << std::endl;
        }
        else {
            result = fMaps[level][y * nMapX + x];
        }
        return result;
    }

    // getter for obtaining the character value of the cell at level, coordinates (x, y)
    char CellValueAt(int x, int y, int level) {
        char result = ' ';
        if (!IsInBounds(x, y)) {
            std::cout << "ERROR: CellValueAt() --> map indices out of bounds: (" << x << ", " << y << ")" << std::endl;
        }
        else if (level >= (int)fMaps.size()) {
            std::cout << "ERROR: CellValueAt() --> level argument out of range: " << level << std::endl;
        }
        else {
            result = sMaps[level][y * nMapX + x];
        }
        return result;
    }

    // returns the diagonal length of the map - useful for setting max distance value
    float DiagonalLength() {
        return sqrt(nMapX * nMapX + nMapY * nMapY);
    }

    // returns current number of layers in this map object
    int NrOfLayers() {
        return (int)fMaps.size();
    }

    // collision detection on the map:
    // Note that int( fH ) denotes level to check, and (fH - int( fH )) denotes height to check within that level
    // and fR is the radius of the object (considered as a pillar shape)
    bool Collides(float fX, float fY, float fH, float fR, float fVX, float fVY) {

        float fOffsetX;
        float fOffsetY;
        if (fVX == 0.0f) { fOffsetX = 0.0f; }
        else { fOffsetX = (fVX < 0.0f ? -fR : fR); }
        if (fVY == 0.0f) { fOffsetY = 0.0f; }
        else { fOffsetY = (fVY < 0.0f ? -fR : fR); }
        return (
            !IsInBounds(fX + fOffsetX, fY + fOffsetY) ||
            (fH - fR) < 0.0f || (
                fH <= NrOfLayers() &&
                CellHeightAt(int(fX + fOffsetX), int(fY + fOffsetY), int(fH)) >= (fH - int(fH))
                )
            );
    }

private:
    std::vector<std::string> sMaps;    // contains chars that define the type of block per map location
    std::vector<float*>     fMaps;    // contains floats that represent the height per block
    
    int nMapX;                // dimensions for the map
    int nMapY;
};


#endif // !RC_MAP_H


