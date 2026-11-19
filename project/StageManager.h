#pragma once
#include <vector>
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

class StageManager
{
public:
	//一つの関門のルール(制限時間・目標スコア)
	struct QuotaRule
	{
		float timeLimit;
		int targetScore;
	};

	void Initialize();
	void Update(int currentScore);
	void DrawUI();

	bool IsAllClear() const { return isAllClear_; }
	bool IsGameOver() const { return isGameOver_; }

	//Get
	float GetTimer()const { return timer_; }
	int GetCurrentStageNumber()const { return currentStageIndex_ + 1; }
	int GetCurrentTargetScore()const
	{
		if (currentStageIndex_ < static_cast<int>(stages_.size()))
		{
			return stages_[currentStageIndex_].targetScore;
		}
		return stages_.back().targetScore;
	}

private:
	std::vector<QuotaRule> stages_ =
	{
		{ 30.0f,1000 },//第一関門：30秒内に1000点以上
		{ 30.0f,3000 },//第二関門：30秒内に3000点以上
		{ 30.0f,5000 },//第三関門：30秒内に5000点以上
	};

	int currentStageIndex_ = 0;
	float timer_ = 0.0f;
	bool isAllClear_ = false;
	bool isGameOver_ = false;
};