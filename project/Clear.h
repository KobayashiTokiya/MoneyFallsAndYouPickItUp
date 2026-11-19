#pragma once
#pragma once
#include <d3d12.h>
#include <vector>
#include "BaseScene.h"
#include "Vector.h"

// 前方宣言
class Sprite;
class Object3d;
class Camera;

class Clear : public BaseScene
{
public:
	Clear() = default;
	~Clear() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;

private:
	struct FallingCoin
	{
		Object3d* model = nullptr;
		float fallSpeed = 0.0f;
		float rotateSpeed = 0.0f;
	};


	Sprite* backGroundSprite_ = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE backGroundHandle_{};

	Sprite* enterSprite_ = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE enterHandle_{};

	Object3d* clearModel_ = nullptr;
	Camera* camera_ = nullptr;

	Object3d* groundModel_ = nullptr;

	Object3d* playerModel_ = nullptr;

	//左右のタワーコイン
	std::vector<Object3d*> coinTowers_;

	// 上から降ってくるコイン
	std::vector<FallingCoin> fallingCoins_;

	// 花火パーティクル用
	float fireworkTimer_ = 0.0f;
	D3D12_GPU_DESCRIPTOR_HANDLE ringTexHandle_{};
	D3D12_GPU_DESCRIPTOR_HANDLE cylinderTexHandle_{};
	D3D12_GPU_DESCRIPTOR_HANDLE sphereTexHandle_{};
	D3D12_GPU_DESCRIPTOR_HANDLE lightningTexHandle_{};

	// スコア表示用
	int score_ = 0;
	static const int kMaxScoreDigits = 6;
	Sprite* scoreSprites_[kMaxScoreDigits] = {};
	D3D12_GPU_DESCRIPTOR_HANDLE numberTexHandles_[10] = {};

	Sprite* minusSprite_ = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE minusTexHandle_{};

	// ImGui調整用パラメータ
	Vector3 playerTranslate_ = { 0.0f, 0.0f, 0.0f };
	Vector3 playerRotate_ = { 0.0f, 0.0f, 0.0f };
	Vector3 playerScale_ = { 2.0f, 2.0f, 2.0f };

	Vector3 clearTranslate_ = { 0.0f, 5.0f, 0.0f };
	Vector3 clearScale_ = { 2.0f, 2.0f, 2.0f };

	Vector3 cameraTranslate_ = { 0.0f, 0.0f, 0.0f };
	Vector3 cameraTarget_ = { 0.0f, 0.0f, -2.0f };

#ifdef USE_IMGUI
	Object3d* debugLightMarker_ = nullptr;
#endif
};

