#pragma once
#ifndef PLAYER_H
#define PLAYER_H

class Player
{
public:
	Player();
	~Player();

	float getPlayerX();
	void setPlayerX(float newX);
	float getPlayerY();
	void setPlayerY(float newY);
	float getPlayerAngle();
	void setPlayerAngle(float angle);
	float getPlayerH();
	void setPlayerH(float set);
	float getLookUp();
	void setLookUp(float set);
	
	
private:
	float fPlayerX;
	float fPlayerY;
	float fPlayerA_deg;
	float fPlayerH;
	float fLookUp;
	
	
};


#endif // !1


