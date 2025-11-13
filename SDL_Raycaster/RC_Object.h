#pragma once
#ifndef RC_OBJECT_H
#define RC_OBJECT_H
#include "SGE_Core.h"
#include "RC_Misc.h"

class RC_Object {

public:
    RC_Object() {}
    RC_Object(float fX, float fY, float fS, float fD, float fA, float fR, flc ::Sprite* pS) {
        x = fX;
        y = fY;
        scale = fS;
        fDistToPlayer = fD;
        fAngleToPlayer = fA;
        sprite = pS;
        vx = 0.0f;
        vy = 0.0f;
        fObjRadius = fR;
        UpdateObjAngle();
        UpdateObjSpeed();
    }

    void SetX(float fX) { x = fX; }
    void SetY(float fY) { y = fY; }

    float GetX() { return x; }
    float GetY() { return y; }

    void SetPos(float fX, float fY) {
        x = fX;
        y = fY;
    }

    void SetScale(float fS) { scale = fS; }
    void SetDistToPlayer(float fD) { fDistToPlayer = fD; }
    void SetAngleToPlayer(float fA) { fAngleToPlayer = fA; }
    void setObjectRadius(float fR) { fObjRadius = fR; }

    float GetScale() { return scale; }
    float GetDistToPlayer() { return fDistToPlayer; }
    float GetAngleToPlayer() { return fAngleToPlayer; }
    float getObjectRadius() { return fObjRadius; }

    void SetSprite(flc::Sprite* pS) { sprite = pS; }
    flc::Sprite* GetSprite() { return sprite; }

    void SetVX(float fVX) { vx = fVX; UpdateObjAngle(); UpdateObjSpeed(); }
    void SetVY(float fVY) { vy = fVY; UpdateObjAngle(); UpdateObjSpeed(); }

    float GetVX() { return vx; }
    float GetVY() { return vy; }
    float GetAngle() { return fObjAngle; }   // in radians!!
    float GetSpeed() { return fObjSpeed; }

    void Update(RC_Map& cMap, float fElapsedTime) {
        if (!bStatic) {
            float newX = x + vx * fElapsedTime;
            float newY = y + vy * fElapsedTime;
            if (!cMap.Collides(newX, y, scale, fObjRadius, vx, vy)) {
                x = newX;
            }
            else {
                vx = -vx;
                UpdateObjAngle();
                UpdateObjSpeed();
            }
            if (!cMap.Collides(x, newY, scale, fObjRadius, vx, vy)) {
                y = newY;
            }
            else {
                vy = -vy;
                UpdateObjAngle();
                UpdateObjSpeed();
            }
        }
    }

    void Print() {
        std::cout << "object @ pos: (" << x << ", " << y << "), ";
        std::cout << "vel: (" << vx << ", " << vy << "), ";
        std::cout << (bStatic ? "STATIONARY " : "DYNAMIC ");
        std::cout << std::endl;
    }

public:
    bool bStatic = true;

private:
    void UpdateObjAngle() { fObjAngle = rad_mod2pi(atan2f(vy, vx)); }
    void UpdateObjSpeed() { fObjSpeed = sqrt(vx * vx + vy * vy); }

private:
    float x, y;             // position in the map
    float scale;            // 1.0f is 100%

    float vx, vy;           // velocity
    float fObjAngle;
    float fObjSpeed;
    float fObjRadius;

    float fDistToPlayer,
        fAngleToPlayer;  // w.r.t. player
    flc::Sprite* sprite;
};

#endif // !RC_OBJECT_H
