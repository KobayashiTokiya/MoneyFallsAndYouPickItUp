#include "SpeedUpSkill.h"
#include "Player.h"

bool SpeedUpSkill::Execute(Player* player, int& currentScore)
{
	if (!player || !CanUse(currentScore) || isActive_)
	{
		return false;
	}

	currentScore -= GetCost();
	durationTimer_ = kDuration_;
	isActive_ = true;

	// プレイヤーの移動速度を 1.8倍 にアップ
	player->SetSpeedMultiplier(kSpeedMultiplier_);
	return true;
}

void SpeedUpSkill::Update(float deltaTime, Player* player)
{
	if (!isActive_) return;

	durationTimer_ -= deltaTime;
	if (durationTimer_ <= 0.0f)
	{
		durationTimer_ = 0.0f;
		isActive_ = false;

		// 効果終了時にプレイヤーの移動速度を通常（1.0倍）に戻す
		if (player)
		{
			player->SetSpeedMultiplier(1.0f);
		}
	}
}