#include "Player.h"

Player::Player()
{
	fPlayerX = 2.0f;
	fPlayerY = 2.0f;
	fPlayerA_deg = 0.0f;
	fPlayerH = 0.5f;

}

Player::~Player()
{
}

float Player::getPlayerX()
{
	return fPlayerX;
}

void Player::setPlayerX(float newX)
{
	fPlayerX = newX;
}

float Player::getPlayerY()
{
	return fPlayerY;
}

void Player::setPlayerY(float newY)
{
	fPlayerY = newY;
}

float Player::getPlayerAngle()
{
	return fPlayerA_deg;
}

void Player::setPlayerAngle(float angle)
{
	fPlayerA_deg = angle;
}

float Player::getPlayerH()
{
	return fPlayerH;
}
