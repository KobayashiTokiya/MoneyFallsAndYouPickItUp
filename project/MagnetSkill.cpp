#include "MagnetSkill.h"
#include "Player.h"
#include "Coin.h"
#include <cmath>

bool MagnetSkill::Execute(Player* player, int& currentScore)
{
	if (!player || !CanUse(currentScore) || isActive_)
	{
		return false;
	}

	currentScore -= GetCost();
	durationTimer_ = kDuration_;
	isActive_ = true;
	return true;
}

void MagnetSkill::Update(float deltaTime, Player* player)
{
	if (!isActive_) return;

	durationTimer_ -= deltaTime;
	if (durationTimer_ <= 0.0f)
	{
		durationTimer_ = 0.0f;
		isActive_ = false;
	}
}

// 範囲内かつ「カップ入口より上」のコインだけを入口へ引っ張る処理
void MagnetSkill::ApplyMagnet(Player* player, Coin* coin)
{
	if (!isActive_ || !player || !coin || coin->IsDead()) return;

	Vector3 pPos = player->GetPosition();
	Vector3 cPos = coin->GetPosition();

	// 1. カップ入口の高さを計算（プレイヤーの位置 + カップの高さ1.5f）
	//    ※ BigCup等でスケールが変わる場合はスケールも考慮
	float cupHeight = 1.5f * player->GetScale().y;
	float entranceY = pPos.y + cupHeight;

	// ★ 2. コインがカップの入口より下にある場合は吸い寄せない
	if (cPos.y < entranceY)
	{
		return;
	}

	// 3. 吸引目標を「カップの入口の中心」に設定
	Vector3 targetPos = { pPos.x, entranceY, pPos.z };

	// 4. ターゲットまでのベクトルと距離を計算
	Vector3 diff = { targetPos.x - cPos.x, targetPos.y - cPos.y, targetPos.z - cPos.z };
	float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

	// 5. 範囲内ならターゲットに向かって引き寄せる
	if (distance > 0.001f && distance <= kMagnetRange_)
	{
		Vector3 dir = { diff.x / distance, diff.y / distance, diff.z / distance };

		Vector3 newPos = cPos;
		newPos.x += dir.x * kMagnetSpeed_;
		newPos.y += dir.y * kMagnetSpeed_;
		newPos.z += dir.z * kMagnetSpeed_;

		coin->SetPosition(newPos);
	}
}