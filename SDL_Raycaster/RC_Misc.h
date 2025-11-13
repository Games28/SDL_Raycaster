#pragma once
#ifndef RC_MISC_H
#define RC_MISC_H
#include <cmath>
#define PI 3.1415926535f

float deg2rad(float fAngleDeg) { return fAngleDeg * PI / 180.0f; }
float rad2deg(float fAngleRad) { return fAngleRad / PI * 180.0f; }
float deg_mod2pi(float fAngleDeg) {
    while (fAngleDeg < 0.0f) fAngleDeg += 360.0f;
    while (fAngleDeg >= 360.0f) fAngleDeg -= 360.0f;
    return fAngleDeg;
}
float rad_mod2pi(float fAngleRad) {
    while (fAngleRad < 0.0f) fAngleRad += 2.0f * PI;
    while (fAngleRad >= 2.0f * PI) fAngleRad -= 2.0f * PI;
    return fAngleRad;
}

// ==============================/  lookup sine and cosine functions  /==============================

#define SIGNIFICANCE 3      // float angle is rounded at three decimal points
#define SIG_POW10    1000

float lu_sin_array[360 * SIG_POW10];
float lu_cos_array[360 * SIG_POW10];

void init_lu_sin_array() {
    for (int i = 0; i < 360; i++) {
        for (int j = 0; j < SIG_POW10; j++) {
            int nIndex = i * SIG_POW10 + j;
            float fArg_deg = float(nIndex) / float(SIG_POW10);
            lu_sin_array[nIndex] = sinf(deg2rad(fArg_deg));
        }
    }
}

void init_lu_cos_array() {
    for (int i = 0; i < 360; i++) {
        for (int j = 0; j < SIG_POW10; j++) {
            int nIndex = i * SIG_POW10 + j;
            float fArg_deg = float(nIndex) / float(SIG_POW10);
            lu_cos_array[nIndex] = cosf(deg2rad(fArg_deg));
        }
    }
}

float lu_sin(float fDegreeAngle) {
    fDegreeAngle = deg_mod2pi(fDegreeAngle);
    int nWholeNr = int(fDegreeAngle);
    int nRemainder = int((fDegreeAngle - nWholeNr) * float(SIG_POW10));
    int nIndex = nWholeNr * SIG_POW10 + nRemainder;
    return lu_sin_array[nIndex];
}

float lu_cos(float fDegreeAngle) {
    fDegreeAngle = deg_mod2pi(fDegreeAngle);
    int nWholeNr = int(fDegreeAngle);
    int nRemainder = int((fDegreeAngle - nWholeNr) * float(SIG_POW10));
    int nIndex = nWholeNr * SIG_POW10 + nRemainder;
    return lu_cos_array[nIndex];
}


#endif // !RC_MISC_H
