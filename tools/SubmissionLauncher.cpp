#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

#include <filesystem>
#include <string>

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
	wchar_t launcherPath[MAX_PATH]{};
	if (GetModuleFileNameW(nullptr, launcherPath, MAX_PATH) == 0) {
		MessageBoxW(nullptr, L"Could not locate the launcher.", L"TatariBashiri", MB_OK | MB_ICONERROR);
		return 1;
	}

	const std::filesystem::path packageDirectory = std::filesystem::path(launcherPath).parent_path();
	const std::filesystem::path gamePath = packageDirectory / L"runtime" / L"TatariBashiriGame.exe";

	if (!std::filesystem::exists(gamePath)) {
		MessageBoxW(nullptr, L"The game was not found in the runtime folder.\nKeep the package contents in their original locations.", L"TatariBashiri", MB_OK | MB_ICONERROR);
		return 1;
	}

	const HINSTANCE result = ShellExecuteW(
		nullptr,
		L"open",
		gamePath.c_str(),
		nullptr,
		packageDirectory.c_str(),
		SW_SHOWNORMAL);

	if (reinterpret_cast<INT_PTR>(result) <= 32) {
		MessageBoxW(nullptr, L"Could not start the game.", L"TatariBashiri", MB_OK | MB_ICONERROR);
		return 1;
	}

	return 0;
}
