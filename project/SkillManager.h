#pragma once
#include <vector>
#include <memory>
#include "ISkill.h"

class Player;
class Coin; // ★ 追加

class SkillManager
{
public:
	SkillManager() = default;
	~SkillManager() = default;

	void Initialize();
	void Update(float deltaTime, Player* player, int& currentScore); // ★ Player* ポインタに修正
	void DrawImGui(int currentScore);

	// マグネット効果の適用関数
	void ApplyMagnetEffect(Player* player, Coin* coin);

	// 現在のスコア倍率を取得（ブースト中なら2倍、通常時は1倍）
	int GetScoreMultiplier() const;

	// コインの雨が発動したか判定する
	bool PopCoinRainTrigger();

	// コイン小型化スキルが有効か判定
	bool IsSmallCoinActive() const;

	// 時間スロー用パラメータ
	bool IsTimeSlowed() const { return isMenuOpen_; }
	float GetTimeScale() const { return isMenuOpen_ ? 0.1f : 1.0f; }
private:
	std::vector<std::unique_ptr<ISkill>> skills_;
	bool isMenuOpen_ = false;
};