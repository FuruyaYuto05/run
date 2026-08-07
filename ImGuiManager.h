#pragma once

#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#endif

class WinApp;
class DirectXCommon;
class SrvManager;

class ImGuiManager
{
public:
	void Initialize(WinApp* winApp, DirectXCommon* dxCommon, SrvManager* srvManager);
	void Finalize();
	void Begin();
	void End();
	void Draw();

private:
	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;
};
