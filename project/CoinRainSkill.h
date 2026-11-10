#pragma once
#include "ISkill.h"

class Player;

class CoinRainSkill : public ISkill
{
public:
	CoinRainSkill() = default;
	~CoinRainSkill() override = default;

	std::string GetName() const override { return "COIN RAIN"; }
	int GetCost() const override { return 250; } // スコア250消費

	bool Execute(Player* player, int& currentScore) override;
	void Update(float deltaTime, Player* player) override;

	bool IsActive() const override { return isActive_; }

	// コイン生成リクエストを取り出す関数
	bool PopTrigger()
	{
		bool res = isTriggered_;
		isTriggered_ = false;
		return res;
	}

private:
	bool isActive_ = false;
	bool isTriggered_ = false;
	float durationTimer_ = 0.0f;
};