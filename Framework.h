#pragma once

#include "Sound.h"
#include <memory>

class WinApp;
class Input;
class DirectXCommon;
class SrvManager;
class ImGuiManager;
class SpriteCommon;
class Object3dCommon;

class Framework {
public:
    Framework();
    virtual ~Framework();

    void Run();
    bool IsEndRequest() const { return endRequest_; }

protected:
    virtual void OnInitialize() = 0;
    virtual void OnFinalize() = 0;
    virtual void OnUpdate() = 0;
    virtual void OnDraw() = 0;

    Input* GetInput() const { return input_.get(); }
    DirectXCommon* GetDirectXCommon() const { return dxCommon_.get(); }
    SrvManager* GetSrvManager() const { return srvManager_.get(); }
    ImGuiManager* GetImGuiManager() const { return imguiManager_.get(); }
    Object3dCommon* GetObject3dCommon() const { return object3dCommon_; }

private:
    void InitializeEngine();
    void FinalizeEngine();
    void UpdateEngine();

    std::unique_ptr<WinApp> winApp_;
    std::unique_ptr<Input> input_;
    std::unique_ptr<DirectXCommon> dxCommon_;
    std::unique_ptr<SrvManager> srvManager_;
    std::unique_ptr<ImGuiManager> imguiManager_;

    Sound* sound_ = nullptr;
    SpriteCommon* spriteCommon_ = nullptr;
    Object3dCommon* object3dCommon_ = nullptr;
    bool endRequest_ = false;
};
