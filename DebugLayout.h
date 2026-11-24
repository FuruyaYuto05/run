#pragma once

// デバッグ画面の見た目を一箇所で決めるための小さなレイアウト部品です。
// 新しい調整ウィンドウは BeginPanel() を呼ぶだけで、ゲーム画面の外周に置けます。
#ifdef USE_IMGUI
#include <algorithm>
#include <imgui.h>

namespace DebugLayout {

enum class Panel {
	Toolbar,
	Left,
	Right,
	Bottom,
};

inline constexpr ImGuiWindowFlags kFixedPanelFlags =
	ImGuiWindowFlags_NoMove |
	ImGuiWindowFlags_NoResize |
	ImGuiWindowFlags_NoCollapse |
	ImGuiWindowFlags_NoBringToFrontOnFocus;

struct Rect {
	float x = 0.0f;
	float y = 0.0f;
	float width = 0.0f;
	float height = 0.0f;
};

inline constexpr float kToolbarHeight = 48.0f;
inline constexpr float kLeftWidth = 300.0f;
inline constexpr float kRightWidth = 340.0f;
inline constexpr float kBottomHeight = 210.0f;

inline Rect GetGameViewRect(const ImVec2& display) {
	return {
		kLeftWidth,
		kToolbarHeight,
		(std::max)(160.0f, display.x - kLeftWidth - kRightWidth),
		(std::max)(120.0f, display.y - kToolbarHeight - kBottomHeight),
	};
}

inline void PreparePanel(Panel panel) {
	const ImVec2 display = ImGui::GetIO().DisplaySize;

	ImVec2 position{};
	ImVec2 size{};
	switch (panel) {
	case Panel::Toolbar:
		position = { 0.0f, 0.0f };
		size = { display.x, kToolbarHeight };
		break;
	case Panel::Left:
		position = { 0.0f, kToolbarHeight };
		size = { kLeftWidth, (std::max)(120.0f, display.y - kToolbarHeight - kBottomHeight) };
		break;
	case Panel::Right:
		position = { (std::max)(0.0f, display.x - kRightWidth), kToolbarHeight };
		size = { kRightWidth, (std::max)(120.0f, display.y - kToolbarHeight - kBottomHeight) };
		break;
	case Panel::Bottom:
		position = { 0.0f, (std::max)(kToolbarHeight, display.y - kBottomHeight) };
		size = { (std::max)(240.0f, display.x), kBottomHeight };
		break;
	}

	// パネルの下にゲーム画面を透かさず、中央のビューポートだけを見せる。
	ImGui::SetNextWindowBgAlpha(1.0f);
	ImGui::SetNextWindowPos(position, ImGuiCond_Always);
	ImGui::SetNextWindowSize(size, ImGuiCond_Always);
}

inline bool BeginPanel(const char* title, Panel panel) {
	PreparePanel(panel);
	return ImGui::Begin(title, nullptr, kFixedPanelFlags);
}

} // namespace DebugLayout
#endif
