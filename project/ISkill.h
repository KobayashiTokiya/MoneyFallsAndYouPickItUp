#pragma once
#include <string>

class Player; // 前方宣言

class ISkill
{
public:
	virtual ~ISkill() = default;

	// スキルの基本情報
	virtual std::string GetName() const = 0;
	virtual int GetCost() const = 0;

	// 発動可能かどうかの判定（デフォルトはスコアが足りているか）
	virtual bool CanUse(int currentScore) const
	{
		return currentScore >= GetCost();
	}

	// スキル発動処理（数字キーやマウスで選択された時に呼ばれる）
	virtual bool Execute(Player* player, int& currentScore) = 0;

	// ★ 毎フレームの更新処理
	// 時限スキルはこれをオーバーライドしてタイマー処理を書く。
	// 永続スキルは何も書かなくてOK（自動でこの空処理が呼ばれる）。
	virtual void Update(float deltaTime, Player* player) {}

	// 現在効果が有効かどうか
	virtual bool IsActive() const = 0;
};