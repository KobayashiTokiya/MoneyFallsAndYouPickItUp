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

#include <cmath>
#include <cstdlib>

void Clear::Initialize()
{
	score_ = ScoreManager::GetScore();

	// 1. テクスチャのロード ＆ インデックス／ハンドルの取得
	TextureManager::GetInstance()->LoadTexture("Resource/white.png");
	TextureManager::GetInstance()->LoadTexture("Resource/coin(-500).png");
	TextureManager::GetInstance()->LoadTexture("Resource/white.dds");
	TextureManager::GetInstance()->LoadTexture("Resource/floor.png");
	TextureManager::GetInstance()->LoadTexture("Resource/UI/reset.png");
	TextureManager::GetInstance()->LoadTexture("Resource/UI/enter(title).png");


	//スプライトで使用
	backGroundHandle_ = TextureManager::GetInstance()->GetSrvHandleGPU("Resource/white.png");
	enterHandle_ = TextureManager::GetInstance()->GetSrvHandleGPU("Resource/UI/enter(title).png");

	//モデルで使用
	uint32_t whiteTexIndex = TextureManager::GetInstance()->GetSrvIndex("Resource/white.png");
	uint32_t envTexIndex = TextureManager::GetInstance()->GetSrvIndex("Resource/white.dds");
	uint32_t floorTexture = TextureManager::GetInstance()->GetSrvIndex("Resource/floor.png");
	uint32_t coinTexture = TextureManager::GetInstance()->GetSrvIndex("Resource/coin(-500).png");

	//数字テクスチャのロード（0〜9）
	for (int i = 0; i < 10; ++i)
	{
		std::string path = "Resource/number/" + std::to_string(i) + ".png";
		TextureManager::GetInstance()->LoadTexture(path);
		numberTexHandles_[i] = TextureManager::GetInstance()->GetSrvHandleGPU(path);
	}

	//マイナス記号のロード
	TextureManager::GetInstance()->LoadTexture("Resource/number/minus.png");
	minusTexHandle_ = TextureManager::GetInstance()->GetSrvHandleGPU("Resource/number/minus.png");

	minusSprite_ = new Sprite();
	minusSprite_->Initialize(SpriteCommon::GetInstance(), "Resource/number/minus.png");
	minusSprite_->SetSize({ 48.0f, 96.0f }); // クリア画面用に少し大きめ（48x96）に設定

	// スコア数字スプライトの生成（画面中央・大きめの表示）
	// 表示位置：画面上部〜中央寄りに大きめに配置（例: 画面中央 640 を中心に並べる）
	Vector2 basePos = { 680.0f, 350.0f }; // 一番右（1の位）の位置
	float digitWidth = 50.0f;             // 桁間隔

	for (int i = 0; i < kMaxScoreDigits; ++i)
	{
		scoreSprites_[i] = new Sprite();
		scoreSprites_[i]->Initialize(SpriteCommon::GetInstance(), "Resource/number/0.png");
		scoreSprites_[i]->SetSize({ 48.0f, 96.0f });
		scoreSprites_[i]->SetPosition({ basePos.x - (i * digitWidth), basePos.y });
	}

	//カメラの生成と位置・注視点の設定
	camera_ = new Camera();
	camera_->SetMode(Camera::Mode::Normal);
	camera_->SetTranslate({ 0.0f, 4.0f, -15.0f }); // 後ろに引く
	camera_->TargetUpdate({ 0.0f, 3.0f, 0.0f });   // 空中と地面の中間を注視
	Object3dCommon::GetInstance()->SetDefaultCamera(camera_);

	// 3Dモデルのロード・生成・テクスチャ設定
	ModelManager::GetInstance()->LoadModel("clear.obj");
	ModelManager::GetInstance()->LoadModel("Coin.obj");
	ModelManager::GetInstance()->LoadModel("plane.obj");

	groundModel_ = new Object3d();
	groundModel_->Initialize();
	groundModel_->SetModel("plane.obj");
	groundModel_->SetTextureIndex(floorTexture);

	clearModel_ = new Object3d();
	clearModel_->Initialize();
	clearModel_->SetModel("clear.obj");
	clearModel_->SetTextureIndex(whiteTexIndex);       // ★ テクスチャインデックスをセット
	clearModel_->SetEnvironmentTexture(envTexIndex);  // ★ 環境マップをセット

	// 座標・回転・スケールの調整（出ない場合はスケールを 5.0f や 10.0f に大きくしてみてください）
	clearModel_->SetTranslate({ 0.0f,5.0f,0.0f });
	clearModel_->SetRotate({ 0.0f, 0.0f, 0.0f });
	clearModel_->SetScale({ 2.0f, 2.0f, 2.0f });

	//背景スプライトの生成
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

	// フェードイン（明転）開始
	FadeManager::GetInstance()->StartFadeIn(0.5f);
}

void Clear::Update()
{
	FadeManager::GetInstance()->Update();

	if (backGroundSprite_)
	{
		backGroundSprite_->Update();
	}

	if (enterSprite_)
	{
		enterSprite_->Update();
	}

	// 地面や背景の変換行列を更新して静止画を作る
	Vector3 floorTranslate = { 0.0f, -0.5f, 0.0f };
	Vector3 floorRotate = { 1.57f, 0.0f, 0.0f };
	Vector3 floorScale = { 50.0f, 50.0f, 0.2f };
	groundModel_->SetTranslate(floorTranslate);
	groundModel_->SetRotate(floorRotate);
	groundModel_->SetScale(floorScale);

	groundModel_->Update();

	if (clearModel_)
	{
		clearModel_->Update();
	}

	// カメラとモデルの更新
	if (camera_)
	{
		Vector3 cameraPos = { 0.0f,0.0f,0.0f };
		Vector3 targetPos = { 0.0f,0.0f,-2.0f };
		camera_->SetTranslate(cameraPos);
		camera_->TargetUpdate(targetPos); // 原点を注視し続ける
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



	// 2. 深度バッファをクリアして3Dモデルを描画可能にする
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DirectXCommon::GetInstance()->GetDSVCPUDescriptorHandle(0);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// 3. 3Dモデルを描画
	Object3dCommon::GetInstance()->CommonDrawSettings();
	groundModel_->Draw();

	if (clearModel_)
	{
		clearModel_->Draw();
	}


	// 1. 背景スプライトを描画
	SpriteCommon::GetInstance()->CommonDrawSettings();
	enterSprite_->Draw(commandList, enterHandle_);
	
	// ★ スコア描画（6桁対応）
	bool isNegative = (score_ < 0);
	int tempScoreDraw = std::abs(score_);

	Vector2 basePos = { 680.0f, 350.0f }; // 一番右（1の位）の位置
	float digitWidth = 50.0f;             // 桁間隔

	int activeDigits = 1;
	int checkVal = tempScoreDraw;
	while (checkVal >= 10)
	{
		checkVal /= 10;
		activeDigits++;
	}

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
			if (isNegative)
			{
				scoreSprites_[i]->SetColor({ 1.0f, 0.3f, 0.3f, 1.0f });
			}
			else
			{
				scoreSprites_[i]->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
			}

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



	// 4. 最前面にフェードを描画
	FadeManager::GetInstance()->Draw();

	// 描画後処理
	DirectXCommon::GetInstance()->PostDraw();
}

void Clear::Finalize()
{
	if (backGroundSprite_)
	{
		delete backGroundSprite_;
		backGroundSprite_ = nullptr;
	}

	if (enterSprite_)
	{
		delete enterSprite_;
		enterSprite_ = nullptr;
	}

	// スコア用スプライトの解放
	for (int i = 0; i < kMaxScoreDigits; ++i)
	{
		if (scoreSprites_[i])
		{
			delete scoreSprites_[i];
			scoreSprites_[i] = nullptr;
		}
	}

	if (minusSprite_)
	{
		delete minusSprite_;
		minusSprite_ = nullptr;
	}

	delete groundModel_;
	groundModel_ = nullptr;
	if (clearModel_)
	{
		delete clearModel_;
		clearModel_ = nullptr;
	}
	if (camera_)
	{
		delete camera_;
		camera_ = nullptr;
	}
}