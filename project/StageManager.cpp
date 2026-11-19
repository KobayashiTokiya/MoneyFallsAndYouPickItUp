#include "StageManager.h"

void StageManager::Initialize()
{
	currentStageIndex_ = 0;
	isAllClear_ = false;
	isGameOver_ = false;
	if (!stages_.empty())
	{
		timer_ = stages_[0].timeLimit;
	}
}

void StageManager::Update(int currentScore)
{
	if (isAllClear_||isGameOver_)
	{
		return;
	}

	timer_ -= 1.0f / 60.0f;

	if (timer_<=0)
	{
		//現在のスコアが目標スコア以上かどうかを判定
		if(currentScore>=stages_[currentStageIndex_].targetScore)
		{ 
			currentStageIndex_++;
			if (currentStageIndex_>=static_cast<int>(stages_.size()))
			{
				timer_ = 0.0f;
				isAllClear_ = true;
			}
			else
			{
				//次の関門の制限時間をセット
				timer_ = stages_[																								currentStageIndex_].timeLimit;
			}
		}
		else
		{
			timer_ = 0.0f;
			isGameOver_ = true;
		}	
	}

}

void StageManager::DrawUI()
{
#ifdef USE_IMGUI
	ImGui::Begin("Stage Manager");
	ImGui::Text("Current Stage: %d", GetCurrentStageNumber());
	ImGui::Text("Timer: %.2f", GetTimer());
	ImGui::Text("Current Score: %d", GetCurrentTargetScore());
	ImGui::End();
#endif // USE_IMGUI

}