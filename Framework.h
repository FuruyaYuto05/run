#pragma once
#include "Sound.h"
#include <memory>
class WinApp; class Input; class DirectXCommon; class SrvManager; class ImGuiManager; class SpriteCommon; class Object3dCommon;
class Framework {
public:
	Framework();
	virtual ~Framework();
	void Run();
	virtual void Initialize();
	virtual void Finalize();
	virtual void Update();
	virtual void Draw() = 0;
	bool IsEndRequest() const { return endRequest_; }
protected:
	std::unique_ptr<WinApp> winApp_; std::unique_ptr<Input> input_; std::unique_ptr<DirectXCommon> dxCommon_;
	std::unique_ptr<SrvManager> srvManager_; std::unique_ptr<ImGuiManager> imguiManager_; Sound* sound_ = nullptr;
	SpriteCommon* spriteCommon_ = nullptr; Object3dCommon* object3dCommon_ = nullptr;
	bool endRequest_ = false;
};
