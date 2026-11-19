#pragma once
#include <d3d12.h>
#include<wrl.h>

#include "DirectXCommon.h"
#include "ParticleManager.h"

class Camera;

//3Dオブジェクト
class Object3dCommon
{
public:
	static Object3dCommon* GetInstance();

	Object3dCommon(const Object3dCommon&) = delete;
	Object3dCommon& operator=(const Object3dCommon&) = delete;

	//メンバ関数
	//初期化
	void Initialize(DirectXCommon* dxCommon);
	//更新
	void Update();
	//描画
	void Draw();

	//共通描画設定
	void CommonDrawSettings();

	// ライト設定のファイル保存・読み込み
	void SaveLightSettings();
	void LoadLightSettings();
private:
	//ルートシグネチャの作成
	void CreateRootSignature();

	//グラフィックスパイプラインの生成
	void CreateGraphicsPipelineState();

	// ライトのリアルタイム調整用ImGui
	void DrawImGui();


public://getterとsetter
	DirectXCommon* GetDxCommon()const { return dxCommon_; }

	//カメラ
	void SetDefaultCamera(Camera* camera) { this->defaultCamera = camera; }
	Camera* GetDefaultCamera()const { return defaultCamera; }

	//ブレンドモード
	ID3D12PipelineState* GetPipelinestate(BlendMode mode) { return graphicsPipelineState[static_cast<int>(mode)].Get(); }
	D3D12_BLEND_DESC GetBlendDesc(BlendMode mode);

	// ★ ライトの取得・変更・保存・読み込みを public にする
	const Vector3& GetLightDirection() const { return lightDirection_; }
	const Vector4& GetLightColor() const { return lightColor_; }
	float GetLightIntensity() const { return lightIntensity_; }

	void SetLightDirection(const Vector3& dir) { lightDirection_ = dir; }
	void SetLightColor(const Vector4& color) { lightColor_ = color; }
	void SetLightIntensity(float intensity) { lightIntensity_ = intensity; }

private:
	Object3dCommon() = default;
	~Object3dCommon() = default;

	DirectXCommon* dxCommon_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineState[static_cast<int>(BlendMode::kCountOfBlendMode)];

	Camera* defaultCamera = nullptr;

	Vector3 lightDirection_ = { 0.3f, -0.8f, 0.5f };
	Vector4 lightColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	float lightIntensity_ = 1.0f;
	float saveMessageTimer_ = 0.0f; // 保存完了メッセージの表示時間
};

