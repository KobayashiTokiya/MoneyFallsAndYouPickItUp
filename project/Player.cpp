#include "Player.h"
#include "Input.h"
#include "ModelManager.h"
#include "Model.h"
#include "Object3d.h"
#include "camera.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Initialize(const std::string& modelFilePath)
{
	position_ = { 0.0f, 0.0f, 0.0f };
	scale_ = { 3.0f, 2.5f, 3.0f };
	targetScale_ = { 3.0f, 1.0f, 3.0f };
	speed_ = 0.5f;

	// Object3d の生成と初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize();
	object3d_->SetModel(modelFilePath);

	uint32_t uvCheckerTexIndex = TextureManager::GetInstance()->GetSrvIndex("Resource/player.png");
	object3d_->SetTextureIndex(uvCheckerTexIndex);

	//初期スケールを反映
	object3d_->SetScale(scale_);
}

void Player::Update()
{
	Vector3 move = { 0.0f, 0.0f, 0.0f };

	if (Input::GetInstance()->PushKey(DIK_W)) { move.z += speed_; }
	if (Input::GetInstance()->PushKey(DIK_A)) { move.x -= speed_; }
	if (Input::GetInstance()->PushKey(DIK_S)) { move.z -= speed_; }
	if (Input::GetInstance()->PushKey(DIK_D)) { move.x += speed_; }

	// BottomUp モードの時だけ Z移動（WSキー）を反転する
	if (camera_ && camera_->GetMode() == Camera::Mode::BottomUp) { move.z *= -1.0f; }

	// 移動量を座標に加算
	position_ += move;

	// ジャンプ処理
	if (Input::GetInstance()->PushKey(DIK_SPACE) && !isJumping_)
	{
		velocityY_ = jumpInitialSpeed_;
		isJumping_ = true;
	}

	if (isJumping_)
	{
		velocityY_ -= gravity_;

		position_.y += velocityY_;
		if (position_.y <= 0.0f)
		{
			position_.y = 0.0f;
			velocityY_ = 0.0f;
			isJumping_ = false;
		}
	}

	//限界値（最小 0.2倍 〜 最大 5.0倍）を設定して大きくなりすぎ・小さくなりすぎを防ぐ
	const float kMinScale = 0.2f;
	const float kMaxScale = 5.0f;
	targetScale_.x = std::clamp(targetScale_.x, kMinScale, kMaxScale);
	targetScale_.z = std::clamp(targetScale_.z, kMinScale, kMaxScale);

	// 線形補間 (Lerp) で滑らかに目標値へ近づける
	scale_.x += (targetScale_.x - scale_.x) * lerpSpeed_;
	scale_.z += (targetScale_.z - scale_.z) * lerpSpeed_;

	if (object3d_)
	{
		object3d_->SetTranslate(position_);
		object3d_->SetScale(scale_);
		object3d_->Update();
	}
}

void Player::Draw()
{
	if (object3d_)
	{
		object3d_->Draw();
	}
}

void Player::CountDownUpdate()
{
	if (object3d_)
	{
		object3d_->SetTranslate(position_);
		object3d_->SetScale(scale_);
		object3d_->Update();
	}
};