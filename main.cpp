#include <Windows.h>

#include "Framework.h"
#include "Game.h"
#include <memory>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	auto game = std::make_unique<Game>();
	game->Run();
	return 0;
}
