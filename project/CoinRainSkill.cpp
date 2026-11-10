#include "CoinRainSkill.h"

bool CoinRainSkill::Execute(Player* player, int& currentScore)
{
	if (!CanUse(currentScore) || isActive_)
	{
		return false;
	}

	currentScore -= GetCost();
	isTriggered_ = true;
	isActive_ = true;
	durationTimer_ = 0.5f; // 発動判定用の短いタイマー
	return true;
}

void CoinRainSkill::Update(float deltaTime, Player* player)
{
	if (!isActive_) return;

	durationTimer_ -= deltaTime;
	if (durationTimer_ <= 0.0f)
	{
		durationTimer_ = 0.0f;
		isActive_ = false;
	}
}