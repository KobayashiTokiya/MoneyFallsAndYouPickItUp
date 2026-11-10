#pragma once
#include "ISkill.h"

class Player;

class SmallCoinSkill : public ISkill
{
public:
	SmallCoinSkill() = default;
	~SmallCoinSkill() override = default;

	std::string GetName() const override { return "SMALL COIN"; }
	int GetCost() const override { return 150; } // スコア150消費

	bool Execute(Player* player, int& currentScore) override;
	void Update(float deltaTime, Player* player) override;

	bool IsActive() const override { return isActive_; }
	float GetRemainingTime() const { return durationTimer_; }

private:
	bool isActive_ = false;
	float durationTimer_ = 0.0f;
	const float kDuration_ = 8.0f; // 8秒間小型化
};