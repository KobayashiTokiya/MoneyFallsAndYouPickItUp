#include "SmallCoinSkill.h"

bool SmallCoinSkill::Execute(Player* player, int& currentScore)
{
	if (!CanUse(currentScore) || isActive_)
	{
		return false;
	}

	currentScore -= GetCost();
	durationTimer_ = kDuration_;
	isActive_ = true;
	return true;
}

void SmallCoinSkill::Update(float deltaTime, Player* player)
{
	if (!isActive_) return;

	durationTimer_ -= deltaTime;
	if (durationTimer_ <= 0.0f)
	{
		durationTimer_ = 0.0f;
		isActive_ = false;
	}
}