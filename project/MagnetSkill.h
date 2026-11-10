#pragma once
#include "ISkill.h"

class Player;

class MagnetSkill : public ISkill
{
public:
	MagnetSkill() = default;
	~MagnetSkill() override = default;

	std::string GetName() const override { return "MAGNET"; }
	int GetCost() const override { return 150; } // スコア150消費

	bool Execute(Player* player, int& currentScore) override;
	void Update(float deltaTime, Player* player) override;

	// コインを引き寄せるメイン処理（SkillManagerやSceneから呼ばれる）
	void ApplyMagnet(Player* player, class Coin* coin);

	bool IsActive() const override { return isActive_; }
	float GetRemainingTime() const { return durationTimer_; }

private:
	bool isActive_ = false;
	float durationTimer_ = 0.0f;
	const float kDuration_ = 4.0f;    // 効果時間（4秒）
	const float kMagnetRange_ = 15.0f; // 吸引範囲
	const float kMagnetSpeed_ = 0.8f;  // 吸引スピード
};