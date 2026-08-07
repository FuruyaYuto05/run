#include "ImGuiManager.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "SrvManager.h"

#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

namespace {
	void AllocateImGuiSrvDescriptor(
		ImGui_ImplDX12_InitInfo* initInfo,
		D3D12_CPU_DESCRIPTOR_HANDLE* cpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE* gpuHandle)
	{
		auto* srvManager = static_cast<SrvManager*>(initInfo->UserData);
		const uint32_t srvIndex = srvManager->Allocate();
		*cpuHandle = srvManager->GetCPUDescriptorHandle(srvIndex);
		*gpuHandle = srvManager->GetGPUDescriptorHandle(srvIndex);
	}

	void FreeImGuiSrvDescriptor(
		ImGui_ImplDX12_InitInfo*,
		D3D12_CPU_DESCRIPTOR_HANDLE,
		D3D12_GPU_DESCRIPTOR_HANDLE)
	{
		// SrvManager は現在、確保したSRV番号を再利用しない設計です。
	}
}
#endif

void ImGuiManager::Initialize(
	[[maybe_unused]] WinApp* winApp,
	[[maybe_unused]] DirectXCommon* dxCommon,
	[[maybe_unused]] SrvManager* srvManager)
{
#ifdef USE_IMGUI
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(winApp->GetHwnd());

	ImGui_ImplDX12_InitInfo initInfo{};
	initInfo.Device = dxCommon->GetDevice();
	initInfo.CommandQueue = dxCommon->GetCommandQueue();
	initInfo.NumFramesInFlight = static_cast<int>(dxCommon->GetSwapChainResourcesNum());
	initInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	initInfo.DSVFormat = DXGI_FORMAT_UNKNOWN;
	initInfo.UserData = srvManager;
	initInfo.SrvDescriptorHeap = srvManager->GetDescriptorHeap();
	initInfo.SrvDescriptorAllocFn = AllocateImGuiSrvDescriptor;
	initInfo.SrvDescriptorFreeFn = FreeImGuiSrvDescriptor;

	ImGui_ImplDX12_Init(&initInfo);
#endif
}

void ImGuiManager::Finalize()
{
#ifdef USE_IMGUI
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif
}

void ImGuiManager::Begin()
{
#ifdef USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
}

void ImGuiManager::End()
{
#ifdef USE_IMGUI
	ImGui::Render();
#endif
}

void ImGuiManager::Draw()
{
#ifdef USE_IMGUI
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvManager_->GetDescriptorHeap() };
	commandList->SetDescriptorHeaps(1, descriptorHeaps);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif
}
