#include "Clear.h"
#include "Input.h"
#include "SceneManager.h"
#include "FadeManager.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "ModelManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "Camera.h"
#include "ScoreManager.h"
#include "ParticleManager.h"
#ifdef USE_IMGUI
#include "ImGuiManager.h"
#endif

#include <cmath>
#include <cstdlib>

void Clear::Initialize()
{	score_ = ScoreManager::GetScore();

	// 1. テクスチャのロード ＆ インデックス／ハンドルの取得
	TextureManager::GetInstance()->LoadTexture("Resource/white.png");
	TextureManager::GetInstance()->LoadTexture("Resource/coin(500).png");
	TextureManager::GetInstance()->LoadTexture("Resource/white.dds");
	TextureManager::GetInstance()->LoadTexture("Resource/floor.png");
	TextureManager::GetInstance()->LoadTexture("Resource/UI/reset.png");
	TextureManager::GetInstance()->LoadTexture("Resource/UI/enter(title).png");
	TextureManager::GetInstance()->LoadTexture("Resource/player.png");
	TextureManager::GetInstance()->LoadTexture("Resource/circle.png");
	TextureManager::GetInstance()->LoadTexture("Resource/gradationLine.png");
	TextureManager::GetInstance()->LoadTexture("Resource/lightning.png");

	// スプライトで使用
	backGroundHandle_ = TextureManager::GetInstance()->GetSrvHandleGPU("Resource/white.png");
	enterHandle_ = TextureManager::GetInstance()->GetSrvHandleGPU("Resource/UI/enter(title).png");

	// パーティクル用ハンドル
	ringTexHandle_ = TextureManager::GetInstance()->GetSrvHandleGPU("Resource/circle.png");
	cylinderTexHandle_ = TextureManager::GetInstance()->GetSrvHandleGPU("Resource/gradationLine.png");
	sphereTexHandle_ = TextureManager::GetInstance()->GetSrvHandleGPU("Resource/white.png");
	lightningTexHandle_ = TextureManager::GetInstance()->GetSrvHandleGPU("Resource/white.png");

	// モデルで使用
	uint32_t whiteTexIndex = TextureManager::GetInstance()->GetSrvIndex("Resource/white.png");
	uint32_t envTexIndex = TextureManager::GetInstance()->GetSrvIndex("Resource/white.dds");
	uint32_t floorTexture = TextureManager::GetInstance()->GetSrvIndex("Resource/floor.png");
	uint32_t coinTexture = TextureManager::GetInstance()->GetSrvIndex("Resource/coin(500).png");
	uint32_t playerTexture = TextureManager::GetInstance()->GetSrvIndex("Resource/player.png");

	// 数字テクスチャのロード（0〜9）
	for (int i = 0; i < 10; ++i)
	{
		std::string path = "Resource/number/" + std::to_string(i) + ".png";
		TextureManager::GetInstance()->LoadTexture(path);
		numberTexHandles_[i] = TextureManager::GetInstance()->GetSrvHandleGPU(path);
	}

	// マイナス記号のロード
	TextureManager::GetInstance()->LoadTexture("Resource/number/minus.png");
	minusTexHandle_ = TextureManager::GetInstance()->GetSrvHandleGPU("Resource/number/minus.png");

	minusSprite_ = new Sprite();
	minusSprite_->Initialize(SpriteCommon::GetInstance(), "Resource/number/minus.png");
	minusSprite_->SetSize({ 48.0f, 96.0f });

	for (int i = 0; i < kMaxScoreDigits; ++i)
	{
		scoreSprites_[i] = new Sprite();
		scoreSprites_[i]->Initialize(SpriteCommon::GetInstance(), "Resource/number/0.png");
		scoreSprites_[i]->SetSize({ 48.0f, 96.0f });
	}

	// カメラの生成（今のまま維持）
	camera_ = new Camera();
	camera_->SetMode(Camera::Mode::Normal);
	camera_->SetTranslate({ 0.0f, 4.0f, -15.0f });
	camera_->TargetUpdate({ 0.0f, 3.0f, 0.0f });
	Object3dCommon::GetInstance()->SetDefaultCamera(camera_);

	// 3Dモデルのロード・生成・テクスチャ設定
	ModelManager::GetInstance()->LoadModel("clear.obj");
	ModelManager::GetInstance()->LoadModel("Coin.obj");
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("player.obj");

	groundModel_ = new Object3d();
	groundModel_->Initialize();
	groundModel_->SetModel("plane.obj");
	groundModel_->SetTextureIndex(floorTexture);

	playerModel_ = new Object3d();
	playerModel_->Initialize();
	playerModel_->SetModel("player.obj");
	playerModel_->SetTextureIndex(playerTexture);

	clearModel_ = new Object3d();
	clearModel_->Initialize();
	clearModel_->SetModel("clear.obj");
	clearModel_->SetTextureIndex(whiteTexIndex);
	clearModel_->SetEnvironmentTexture(envTexIndex);

	clearModel_->SetTranslate({ 0.0f, 5.0f, 0.0f });
	clearModel_->SetRotate({ 0.0f, 0.0f, 0.0f });
	clearModel_->SetScale({ 2.0f, 2.0f, 2.0f });

	// プレイヤーの現在位置を基準にする
	Vector3 playerPos = playerModel_->GetTranslate();

	// 4. プレイヤーの左右にコインタワーを生成（平積み）
	const float towerOffsetX[] = { -4.5f, -6.8f, 4.5f, 6.8f };
	const float towerOffsetZ[] = { 0.5f, 2.2f, 0.5f, 2.2f };
	const int towerHeights[] = { 10, 15, 10, 15 }; // 平積みになるので少し枚数を増やす

	for (int t = 0; t < 4; ++t)
	{
		for (int h = 0; h < towerHeights[t]; ++h)
		{
			Object3d* coin = new Object3d();
			coin->Initialize();
			coin->SetModel("Coin.obj");
			coin->SetTextureIndex(coinTexture);

			// 積み重ねたときの自然な微ズレ（完全に真っ直ぐにしたい場合は 0.0f にする）
			float jitterX = static_cast<float>(rand() % 11 - 5) * 0.015f;
			float jitterZ = static_cast<float>(rand() % 11 - 5) * 0.015f;

			coin->SetTranslate({
				playerPos.x + towerOffsetX[t] + jitterX,
				playerPos.y - 0.35f + (h * 0.22f),
				playerPos.z + towerOffsetZ[t] + jitterZ
				});
			// X回転を 0.0f にして水平に寝かせる（Y回転だけ変えて模様の向きを散らす）
			coin->SetRotate({ 0.0f, static_cast<float>(rand() % 628) * 0.01f, 0.0f });
			coin->SetScale({ 0.85f, 0.85f, 0.85f });
			coinTowers_.push_back(coin);
		}
	}

	// 5. プレイヤーの後ろ全体（横幅いっぱい）で降ってくるコイン（35枚）
	for (int i = 0; i < 35; ++i)
	{
		FallingCoin fc;
		fc.model = new Object3d();
		fc.model->Initialize();
		fc.model->SetModel("Coin.obj");
		fc.model->SetTextureIndex(coinTexture);

		float x = playerPos.x + static_cast<float>(rand() % 220 - 110) * 0.1f; // 画面端まで広く（-11.0 〜 +11.0）
		float y = playerPos.y + static_cast<float>(rand() % 110) * 0.1f;        // 高さ（0.0 〜 +11.0）
		float z = playerPos.z + 3.0f + static_cast<float>(rand() % 70) * 0.1f;  // プレイヤーの後ろ（+3.0 〜 +10.0）

		fc.model->SetTranslate({ x, y, z });
		fc.model->SetRotate({
			static_cast<float>(rand() % 628) * 0.01f,
			static_cast<float>(rand() % 628) * 0.01f,
			static_cast<float>(rand() % 628) * 0.01f
			});
		fc.model->SetScale({ 0.45f, 0.45f, 0.45f });
		fc.fallSpeed = 0.03f + static_cast<float>(rand() % 4) * 0.01f;
		fc.rotateSpeed = 0.05f + static_cast<float>(rand() % 5) * 0.02f;

		fallingCoins_.push_back(fc);
	}

	// 背景スプライトの生成
	backGroundSprite_ = new Sprite();
	backGroundSprite_->Initialize(SpriteCommon::GetInstance(), "Resource/white.png");
	backGroundSprite_->SetSize({ 1280.0f, 720.0f });
	backGroundSprite_->SetPosition({ 0.0f, 0.0f });
	backGroundSprite_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	enterSprite_ = new Sprite();
	enterSprite_->Initialize(SpriteCommon::GetInstance(), "Resource/UI/enter(title).png");
	enterSprite_->SetSize({ 640.0f, 128.0f });
	enterSprite_->SetPosition({ 640.0f, 592.0f });
	enterSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	// 開始直後に左右の空に花火を1発ずつ打ち上げておく
	ParticleManager::GetInstance()->EmitFirework({ playerPos.x - 5.0f, playerPos.y + 3.5f, playerPos.z + 5.0f });
	ParticleManager::GetInstance()->EmitFirework({ playerPos.x + 5.0f, playerPos.y + 4.0f, playerPos.z + 5.0f });

#ifdef USE_IMGUI
	debugLightMarker_ = new Object3d();
	debugLightMarker_->Initialize();
	debugLightMarker_->SetModel("sphere.obj");
	debugLightMarker_->SetTextureIndex(whiteTexIndex);
	debugLightMarker_->SetScale({ 0.5f, 0.5f, 0.5f });
#endif


	// フェードイン（明転）開始
	FadeManager::GetInstance()->StartFadeIn(0.5f);
}

void Clear::Update()
{
	FadeManager::GetInstance()->Update();

#ifdef USE_IMGUI
	ImGuiManager::GetInstance()->Begin();
	ImGuiManager::GetInstance()->UpdateClearUI(
		playerTranslate_, playerRotate_, playerScale_,
		clearTranslate_, clearScale_,
		cameraTranslate_, cameraTarget_
	);

	if (debugLightMarker_)
	{
		Vector3 dir = Object3dCommon::GetInstance()->GetLightDirection();
		// 光が飛んでいく向き(dir)の逆側(-dir)に距離 8.0f だけ離して「光源の位置」として見せる
		Vector3 markerPos = {
			-dir.x * 8.0f,
			-dir.y * 8.0f,
			-dir.z * 8.0f
		};
		debugLightMarker_->SetTranslate(markerPos);
		debugLightMarker_->Update();
	}
#endif

	if (backGroundSprite_)
	{
		backGroundSprite_->Update();
	}

	if (enterSprite_)
	{
		enterSprite_->Update();
	}

	// 地面の更新
	Vector3 floorTranslate = { 0.0f, -0.5f, 0.0f };
	Vector3 floorRotate = { 1.57f, 0.0f, 0.0f };
	Vector3 floorScale = { 50.0f, 50.0f, 0.2f };
	groundModel_->SetTranslate(floorTranslate);
	groundModel_->SetRotate(floorRotate);
	groundModel_->SetScale(floorScale);
	groundModel_->Update();

	if (clearModel_)
	{
		clearModel_->SetTranslate(clearTranslate_);
		clearModel_->SetScale(clearScale_);
		clearModel_->Update();
	}

	if (playerModel_)
	{
		playerModel_->SetTranslate(playerTranslate_);
		playerModel_->SetRotate(playerRotate_);
		playerModel_->SetScale(playerScale_);
		playerModel_->Update();
	}

	// 左右のコインタワーの行列更新
	for (auto* coin : coinTowers_)
	{
		coin->Update();
	}

	Vector3 playerPos = playerModel_ ? playerModel_->GetTranslate() : Vector3{ 0.0f, 0.0f, 0.0f };

	// 上から降ってくるコインの更新
	for (auto& fc : fallingCoins_)
	{
		Vector3 pos = fc.model->GetTranslate();
		Vector3 rot = fc.model->GetRotate();

		pos.y -= fc.fallSpeed;
		rot.x += fc.rotateSpeed;
		rot.y += fc.rotateSpeed * 0.7f;
		rot.z += fc.rotateSpeed * 0.4f;

		// 地面の下まで落ちたら上空に再配置
		if (pos.y < playerPos.y - 1.0f)
		{
			pos.y = playerPos.y + 7.0f + static_cast<float>(rand() % 40) * 0.1f;
			pos.x = playerPos.x + static_cast<float>(rand() % 220 - 110) * 0.1f;
			pos.z = playerPos.z + 3.0f + static_cast<float>(rand() % 70) * 0.1f;
		}

		fc.model->SetTranslate(pos);
		fc.model->SetRotate(rot);
		fc.model->Update();
	}

	// 花火パーティクルの定期打ち上げ（左右の空きスペースの上空に交互に出す）
	fireworkTimer_ += 1.0f / 60.0f;
	if (fireworkTimer_ >= 0.7f)
	{
		fireworkTimer_ = 0.0f;
		static bool isLeft = true;
		float side = isLeft ? -1.0f : 1.0f;
		isLeft = !isLeft;

		// 中央のリザルト文字と被らない左右（±3.5 〜 ±7.5）に打ち上げる
		float fx = playerPos.x + side * (3.5f + static_cast<float>(rand() % 40) * 0.1f);
		float fy = playerPos.y + 2.5f + static_cast<float>(rand() % 25) * 0.1f;
		float fz = playerPos.z + 5.0f;
		ParticleManager::GetInstance()->EmitFirework({ fx, fy, fz });
	}
	ParticleManager::GetInstance()->Update(camera_);

	// カメラ更新（今のまま維持）
	if (camera_)
	{
		camera_->SetTranslate(cameraTranslate_);
		camera_->TargetUpdate(cameraTarget_);
	}

	// 1. ENTERキーで暗転（フェードアウト）開始
	if (Input::GetInstance()->TriggerKey(DIK_RETURN) && !FadeManager::GetInstance()->IsFading())
	{
		FadeManager::GetInstance()->StartFadeOut(0.5f);
	}

	// 2. 暗転が完了してからタイトルへ遷移
	if (FadeManager::GetInstance()->GetStatus() == FadeManager::Status::kFadeOut &&
		FadeManager::GetInstance()->IsFadeFinished())
	{
#ifdef USE_IMGUI
		ImGuiManager::GetInstance()->End();
#endif
		SceneManager::GetInstance()->ChangeScene("TITLE");
		return;
	}
}

void Clear::Draw()
{
	// 描画前処理
	DirectXCommon::GetInstance()->PreDraw();
	SrvManager::GetInstance()->PreDraw();

	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	// 深度バッファをクリアして3Dモデルを描画可能にする
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DirectXCommon::GetInstance()->GetDSVCPUDescriptorHandle(0);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// 3Dモデルを描画
	Object3dCommon::GetInstance()->CommonDrawSettings();
	groundModel_->Draw();

	if (clearModel_) clearModel_->Draw();
	if (playerModel_) playerModel_->Draw();

	for (auto* coin : coinTowers_)
	{
		coin->Draw();
	}

	for (auto& fc : fallingCoins_)
	{
		fc.model->Draw();
	}

#ifdef USE_IMGUI
	if (debugLightMarker_)
	{
		debugLightMarker_->Draw();
	}
#endif

	// 花火パーティクルの描画
	ParticleManager::GetInstance()->Draw(
		camera_,
		ringTexHandle_,
		cylinderTexHandle_,
		sphereTexHandle_,
		lightningTexHandle_
	);

	// スプライトの描画
	SpriteCommon::GetInstance()->CommonDrawSettings();
	enterSprite_->Draw(commandList, enterHandle_);

	// スコア描画（プレイヤーの足元・画面下部中央に自動中央揃え）
	bool isNegative = (score_ < 0);
	int tempScoreDraw = std::abs(score_);
	float digitWidth = 50.0f;

	int activeDigits = 1;
	int checkVal = tempScoreDraw;
	while (checkVal >= 10)
	{
		checkVal /= 10;
		activeDigits++;
	}

	float totalWidth = activeDigits * digitWidth;
	Vector2 basePos = { 640.0f + (totalWidth * 0.5f) - digitWidth, 530.0f };

	for (int i = 0; i < kMaxScoreDigits; ++i)
	{
		int digit = tempScoreDraw % 10;
		tempScoreDraw /= 10;

		if (i >= activeDigits)
		{
			continue;
		}

		if (scoreSprites_[i])
		{
			scoreSprites_[i]->SetPosition({ basePos.x - (i * digitWidth), basePos.y });
			scoreSprites_[i]->SetColor(isNegative ? Vector4{ 1.0f, 0.3f, 0.3f, 1.0f } : Vector4{ 1.0f, 1.0f, 1.0f, 1.0f });
			scoreSprites_[i]->Update();
			scoreSprites_[i]->Draw(commandList, numberTexHandles_[digit]);
		}
	}

	// マイナス記号の描画
	if (isNegative && minusSprite_)
	{
		minusSprite_->SetPosition({ basePos.x - (activeDigits * digitWidth), basePos.y });
		minusSprite_->SetColor({ 1.0f, 0.3f, 0.3f, 1.0f });
		minusSprite_->Update();
		minusSprite_->Draw(commandList, minusTexHandle_);
	}

	// 最前面にフェードを描画
	FadeManager::GetInstance()->Draw();

#ifdef USE_IMGUI
	ImGuiManager::GetInstance()->End();
	ImGuiManager::GetInstance()->Draw();
#endif

	// 描画後処理
	DirectXCommon::GetInstance()->PostDraw();
}

void Clear::Finalize()
{
	if (backGroundSprite_) { delete backGroundSprite_; backGroundSprite_ = nullptr; }
	if (enterSprite_) { delete enterSprite_; enterSprite_ = nullptr; }

	for (int i = 0; i < kMaxScoreDigits; ++i)
	{
		if (scoreSprites_[i]) { delete scoreSprites_[i]; scoreSprites_[i] = nullptr; }
	}
	if (minusSprite_) { delete minusSprite_; minusSprite_ = nullptr; }

	delete groundModel_; groundModel_ = nullptr;
	if (clearModel_) { delete clearModel_; clearModel_ = nullptr; }
	delete playerModel_; playerModel_ = nullptr;

	for (auto* coin : coinTowers_) { delete coin; }
	coinTowers_.clear();

	for (auto& fc : fallingCoins_) { delete fc.model; }
	fallingCoins_.clear();

	if (camera_) { delete camera_; camera_ = nullptr; }

#ifdef USE_IMGUI
	delete debugLightMarker_;
	debugLightMarker_ = nullptr;
#endif
}