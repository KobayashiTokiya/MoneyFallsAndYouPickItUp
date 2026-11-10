#include "Coin.h"
#include "TextureManager.h"

void Coin::Initialize(const std::string& modelFilePath, const Vector3& position, const Vector3& scale, int score)
{
	position_ = position;
	scale_ = scale;
	score_ = score;
	isDead_ = false;

	float minSpeed = -0.03f;
	float maxSpeed = -0.12f;
	float randomY = minSpeed + (static_cast<float>(rand()) / RAND_MAX) * (maxSpeed - minSpeed);

	// 初期速度（真下にゆっくり落ちる）
	velocity_ = { 0.0f, randomY, 0.0f };

	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize();
	object3d_->SetModel(modelFilePath);

	// デフォルトのテクスチャを割り当て
	uint32_t uvCheckerTexIndex = TextureManager::GetInstance()->GetSrvIndex("Resource/uvChecker.png");
	uint32_t envTexIndex = TextureManager::GetInstance()->GetSrvIndex("Resource/white.dds");

	object3d_->SetTextureIndex(uvCheckerTexIndex);
	object3d_->SetEnvironmentTexture(envTexIndex);

	object3d_->SetScale(scale_);

	//影用オブジェクト
	shadowObject3d_ = std::make_unique<Object3d>();
	shadowObject3d_->Initialize();
	shadowObject3d_->SetModel("plane.obj");

	uint32_t shadowTexIndex = TextureManager::GetInstance()->GetSrvIndex("Resource/CoinShadow.png");
	shadowObject3d_->SetTextureIndex(shadowTexIndex);

	// ランダムな回転速度を設定 (X, Y, Z軸それぞれに違う回転を与える)
	float rx = (static_cast<float>(rand() % 100) / 100.0f - 0.5f) * 0.1f;
	float ry = (static_cast<float>(rand() % 100) / 100.0f - 0.5f) * 0.1f;
	float rz = (static_cast<float>(rand() % 100) / 100.0f - 0.5f) * 0.1f;
	rotationSpeed_ = { rx, ry, rz };

}

void Coin::Update(float timeScale)
{
	// 1. 重力を加算（timeScale を掛けてスローに対応）
	velocity_.y -= 0.001f * timeScale;

	// 2. 速度を位置に反映（全方向の移動に timeScale を掛ける）
	position_.x += velocity_.x * timeScale;
	position_.y += velocity_.y * timeScale;
	position_.z += velocity_.z * timeScale;

	// 3. 回転処理（スロー中は回転もゆっくりにする）
	rotate_.x += rotationSpeed_.x * timeScale;
	rotate_.y += rotationSpeed_.y * timeScale;
	rotate_.z += rotationSpeed_.z * timeScale;

	// 4. 慣性抵抗（弾かれた後の横移動の減速）
	velocity_.x *= 0.95f;
	velocity_.z *= 0.95f;

	// 5. 画面外（落下）判定
	if (position_.y < -10.0f)
	{
		isDead_ = true;
	}

	// 6. 3Dオブジェクトの行列更新
	if (object3d_)
	{
		object3d_->SetTranslate(position_);
		object3d_->SetRotate(rotate_);

		// スキル倍率（tempScaleMult_）を掛けて見た目のサイズを反映
		Vector3 currentScale = { scale_.x * tempScaleMult_, scale_.y * tempScaleMult_, scale_.z * tempScaleMult_ };
		object3d_->SetScale(currentScale);
		object3d_->Update();
	}

	// 7. 影の位置・スケール更新
	if (shadowObject3d_)
	{
		Vector3 shadowPos = { position_.x, -0.98f, position_.z };
		Vector3 shadowRotate = { 1.57f, 0.0f, 0.0f };

		// 高さに応じて影の大きさを変える
		float heightFactor = (position_.y - (-0.98f)) / 20.0f;
		heightFactor = std::clamp(heightFactor, 0.0f, 1.0f);
		float scaleMultiplier = (1.2f - heightFactor * 0.7f);

		Vector3 shadowScale = { scale_.x * scaleMultiplier, scale_.z * scaleMultiplier, 1.0f };

		shadowObject3d_->SetTranslate(shadowPos);
		shadowObject3d_->SetRotate(shadowRotate);
		shadowObject3d_->SetScale(shadowScale);
		shadowObject3d_->Update();
	}
}

// 壁（フチ）に当たった時に外部（GamePlayScene）から呼ばれる処理
void Coin::OnBounce(const Vector3& bounceDir)
{
	// 1. 外側へ強く弾き飛ばす（0.05f -> 0.25f に強化）
	velocity_.x = bounceDir.x * 0.15f;
	velocity_.z = bounceDir.z * 0.15f;

	// 2. 上方向に跳ね上げる力を与えて「コップのフチで跳ね返った感」を出す
	velocity_.y = 0.1f;

	// 3. インパクト時の回転のブレも大きくする
	rotationSpeed_.x =static_cast<float>(rand() % 50-25)/100.0f;
	rotationSpeed_.z =static_cast<float>(rand() % 50-25)/100.0f;
}

void Coin::Draw()
{
	// 影を先に描画
	if (shadowObject3d_ && position_.y > 0)
	{
		shadowObject3d_->Draw();
	}

	if (object3d_ && !isDead_)
	{
		object3d_->Draw();
	}
}