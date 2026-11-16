#pragma once
#include "ISkill.h"

class Player;

class SpeedUpSkill : public ISkill
{
public:
	SpeedUpSkill() = default;
	~SpeedUpSkill() override = default;

	std::string GetName() const override { return "SPEED UP"; }
	int GetCost() const override { return 100; } // スコア100消費

	bool Execute(Player* player, int& currentScore) override;
	void Update(float deltaTime, Player* player) override;

	bool IsActive() const override { return isActive_; }
	float GetRemainingTime() const { return durationTimer_; }

private:
	bool isActive_ = false;
	float durationTimer_ = 0.0f;
	const float kDuration_ = 6.0f;      // 効果時間（6秒）
	const float kSpeedMultiplier_ = 1.8f; // 1.8倍速
};