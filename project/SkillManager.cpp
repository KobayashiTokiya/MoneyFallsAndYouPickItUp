#include "SkillManager.h"
#include "Input.h"
#include "Player.h"
#include "Coin.h" // ★ コインの操作に必要なヘッダー

#ifdef USE_IMGUI
#include "imgui.h"
#endif

// スキル
#include "BigCupSkill.h"
#include "MagnetSkill.h"
#include "ScoreBoostSkill.h"
#include "SpeedUpSkill.h"
#include "CoinRainSkill.h"
#include "SmallCoinSkill.h"

void SkillManager::Initialize()
{
	skills_.clear();
	skills_.push_back(std::make_unique<BigCupSkill>());     // [1] 巨大化
	skills_.push_back(std::make_unique<MagnetSkill>());     // [2] 磁石吸引
	skills_.push_back(std::make_unique<ScoreBoostSkill>()); // [3] スコア2倍
	skills_.push_back(std::make_unique<SpeedUpSkill>());    // [4] 移動速度アップ
	skills_.push_back(std::make_unique<CoinRainSkill>());   // [5] コインの雨
	skills_.push_back(std::make_unique<SmallCoinSkill>());  // [6] コイン小型化
}

void SkillManager::Update(float deltaTime, Player* player, int& currentScore)
{
	// Eキー長押し中は時間をスロー化
	isMenuOpen_ = Input::GetInstance()->PushKey(DIK_E);

	if (isMenuOpen_)
	{
		// [1] 巨大化
		if (Input::GetInstance()->TriggerKey(DIK_1) && skills_.size() >= 1) { skills_[0]->Execute(player, currentScore); }
		// [2] 磁石吸引
		if (Input::GetInstance()->TriggerKey(DIK_2) && skills_.size() >= 2) { skills_[1]->Execute(player, currentScore); }
		// [3] キーでスコアブースト発動
		if (Input::GetInstance()->TriggerKey(DIK_3) && skills_.size() >= 3) { skills_[2]->Execute(player, currentScore); }
		// [4] キーで Speed Up 発動
		if (Input::GetInstance()->TriggerKey(DIK_4) && skills_.size() >= 4) { skills_[3]->Execute(player, currentScore); }
		// [5] キーで Coin Rain 発動
		if (Input::GetInstance()->TriggerKey(DIK_5) && skills_.size() >= 5) { skills_[4]->Execute(player, currentScore); }
		// [6] キーで Small Coin 発動
		if (Input::GetInstance()->TriggerKey(DIK_6) && skills_.size() >= 6) { skills_[5]->Execute(player, currentScore); }
	}

	// 登録されている全スキルの効果時間タイマー更新
	for (auto& skill : skills_)
	{
		skill->Update(deltaTime, player);
	}
}

// 登録されている MagnetSkill を探してコインをひきつける
void SkillManager::ApplyMagnetEffect(Player* player, Coin* coin)
{
	for (auto& skill : skills_)
	{
		// MagnetSkill かどうかをチェックして呼び出す
		MagnetSkill* magnet = dynamic_cast<MagnetSkill*>(skill.get());
		if (magnet)
		{
			magnet->ApplyMagnet(player, coin);
		}
	}
}

// ScoreBoostSkill の発動状態をチェックして倍率を返す
int SkillManager::GetScoreMultiplier() const
{
	for (const auto& skill : skills_)
	{
		auto boost = dynamic_cast<const ScoreBoostSkill*>(skill.get());
		if (boost && boost->IsActive())
		{
			return boost->GetMultiplier(); // 2倍
		}
	}
	return 1; // 通常時は1倍
}

// CoinRainSkill のトリガーを取り出す
bool SkillManager::PopCoinRainTrigger()
{
	for (auto& skill : skills_)
	{
		auto rain = dynamic_cast<CoinRainSkill*>(skill.get());
		if (rain)
		{
			return rain->PopTrigger();
		}
	}
	return false;
}

// コイン小型化スキルが有効か判定
bool SkillManager::IsSmallCoinActive() const
{
	for (const auto& skill : skills_)
	{
		auto smallCoin = dynamic_cast<const SmallCoinSkill*>(skill.get());
		if (smallCoin && smallCoin->IsActive())
		{
			return true;
		}
	}
	return false;
}

void SkillManager::DrawImGui(int currentScore)
{
#ifdef USE_IMGUI
	// Eキー押下時：GTA5風の選択オーバーレイ表示
	if (isMenuOpen_)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 250), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(320, 160), ImGuiCond_Once);
		ImGui::Begin("=== SKILL MENU (SLOW MOTION) ===", nullptr, ImGuiWindowFlags_NoResize);

		ImGui::Text("Hold [E] & Press Number Key");
		ImGui::Separator();

		for (size_t i = 0; i < skills_.size(); ++i)
		{
			auto& skill = skills_[i];
			bool canUse = skill->CanUse(currentScore) && !skill->IsActive();

			if (canUse)
			{
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "[%d] %s (Cost: %d)",
					static_cast<int>(i + 1), skill->GetName().c_str(), skill->GetCost());
			}
			else
			{
				ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), "[%d] %s (Cost: %d) %s",
					static_cast<int>(i + 1), skill->GetName().c_str(), skill->GetCost(),
					skill->IsActive() ? "[ACTIVE]" : "[NEED SCORE]");
			}
		}

		ImGui::End();
	}

	// 発動中のスキル情報表示
	for (auto& skill : skills_)
	{
		if (skill->IsActive())
		{
			ImGui::Begin("Active Skill Status");
			ImGui::Text("%s ACTIVE!", skill->GetName().c_str());
			ImGui::End();
		}
	}
#endif
}