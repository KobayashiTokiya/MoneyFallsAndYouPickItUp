#pragma once
#include "ISkill.h"

class Player;

class BigCupSkill : public ISkill
{
public:
	BigCupSkill() = default;
	~BigCupSkill() override = default;

	std::string GetName() const override { return "BIG CUP"; }
	int GetCost() const override { return 100; }

	bool Execute(Player* player, int& currentScore) override;
	void Update(float deltaTime, Player* player) override;

	bool IsActive() const override { return isActive_; }

private:
	bool isActive_ = false;
	float durationTimer_ = 0.0f;
};