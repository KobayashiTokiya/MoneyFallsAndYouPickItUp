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
	Sprite* backGroundSprite_ = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE backGroundHandle_{};

	Sprite* enterSprite_ = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE enterHandle_{};

	Object3d* clearModel_ = nullptr;
	Camera* camera_ = nullptr;

	Object3d* groundModel_ = nullptr;

	// スコア表示用
	int score_ = 0;
	static const int kMaxScoreDigits = 6;
	Sprite* scoreSprites_[kMaxScoreDigits] = {};
	D3D12_GPU_DESCRIPTOR_HANDLE numberTexHandles_[10] = {};

	Sprite* minusSprite_ = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE minusTexHandle_{};
};

