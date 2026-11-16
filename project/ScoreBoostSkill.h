#pragma once
#include "ISkill.h"

class ScoreBoostSkill : public ISkill
{
public:
	ScoreBoostSkill() = default;
	~ScoreBoostSkill() override = default;

	std::string GetName() const override { return "BOOST"; }
	int GetCost() const override { return 200; } // スコア200消費

	bool Execute(Player* player, int& currentScore) override;
	void Update(float deltaTime, Player* player) override;

	bool IsActive() const override { return isActive_; }
	float GetRemainingTime() const { return durationTimer_; }

	// 発動中なら2倍、通常時は1倍を返す
	int GetMultiplier() const { return isActive_ ? 2 : 1; }

private:
	bool isActive_ = false;
	float durationTimer_ = 0.0f;
	const float kDuration_ = 10.0f; // 効果時間（10秒）
};