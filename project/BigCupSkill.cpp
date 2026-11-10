#include "BigCupSkill.h"
#include "Player.h"

bool BigCupSkill::Execute(Player* player, int& currentScore)
{
	// 安全チェック（Player が nullptr でないか確認）
	if (!player || !CanUse(currentScore) || isActive_)
	{
		return false;
	}

	currentScore -= GetCost();

	//プレイヤーの大きさを 5.0f に設定
	player->SetTargetScale(5.0f);

	durationTimer_ = 7.0f;
	isActive_ = true;
	return true;
}

void BigCupSkill::Update(float deltaTime, Player* player)
{
	if (!isActive_) return;

	durationTimer_ -= deltaTime;
	if (durationTimer_ <= 0.0f)
	{
		durationTimer_ = 0.0f;
		isActive_ = false;

		if (player)
		{
			// 元の大きさに戻す
			player->SetTargetScale(3.0f);
		}
	}
}