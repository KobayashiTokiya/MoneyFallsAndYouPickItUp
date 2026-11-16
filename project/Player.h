#pragma once
#include <string>
#include <memory>
#include "Vector.h"

class Object3d;
class Camera;

class Player
{
public:
	Player();
	~Player();

	void Initialize(const std::string& modelFilePath);
	void Update(float timeScale = 1.0f);
	void Draw();

	//プレイヤーをカウントダウンの時に表示するためのもの
	void CountDownUpdate();

	const Vector3& GetPosition() const { return position_; }

	void SaveCurrentScale() { savedScale_ = targetScale_; }

	// キープしたサイズを適用する
	void LoadSavedScale() { targetScale_ = savedScale_; }

	// 保存されているサイズを取得
	const Vector3& GetSavedScale() const { return savedScale_; }

	//スケール
	void SetScale(const Vector3& scale) { scale_ = scale; }
	void SetScale(float scale) { scale_ = { scale, scale, scale }; } // 一律変更用
	const Vector3& GetScale() const { return scale_; }
	void SetTargetScale(const Vector3& target) { targetScale_ = target; }
	void SetTargetScale(float target) { targetScale_ = { target, target, target }; }

	//スケールに連動した当たり判定半径の取得
	float GetRadius() const { return baseRadius_ * scale_.x; }
	const Vector3& GetTargetScale() const { return targetScale_; }
	void SetCamera(Camera* camera) { camera_ = camera; }

	//移動速度の倍率をセットする
	void SetSpeedMultiplier(float mult) { speedMultiplier_ = mult; }
	float GetSpeedMultiplier() const { return speedMultiplier_; }
private:
	Vector3 position_;
	Vector3 scale_{ 1.0f, 1.5f, 1.0f }; // ★ 大きさ（デフォルト1.0）
	Vector3 targetScale_{ 1.0f, 1.5f, 1.0f }; // ★ 目標の大きさ
	Vector3 savedScale_{ 1.0f, 1.0f, 1.0f }; // ★ 一時保存用の変数
	float speed_;
	float baseRadius_ = 1.2f;
	
	float lerpSpeed_ = 0.1f; // ★ 変化のスピード（0.05f〜0.2f 程度で調整）
	float velocityY_ = 0.0f;
	const float jumpInitialSpeed_ = 0.5f;
	const float gravity_ = 0.02f;
	bool isJumping_ = false;

	std::unique_ptr<Object3d> object3d_ = nullptr;
	Camera* camera_ = nullptr;

	//速度倍率（通常は 1.0f）
	float speedMultiplier_ = 1.0f;
};

