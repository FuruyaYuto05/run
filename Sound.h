#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <xaudio2.h>
#include <wrl.h>

struct SoundData {
	WAVEFORMATEX wfex{};
	std::vector<BYTE> buffer;
};

class Sound {
public:
	static Sound* GetInstance();
	void Initialize();
	void Finalize();

	SoundData LoadFile(const std::string& filename);
	void PlayWave(const SoundData& soundData);
	void Unload(SoundData* soundData);

private:
	struct PlayingVoice {
		IXAudio2SourceVoice* sourceVoice = nullptr;
		const BYTE* audioData = nullptr;
	};

	Sound() = default;
	Sound(const Sound&) = delete;
	Sound& operator=(const Sound&) = delete;
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;
	std::vector<PlayingVoice> sourceVoices_;
};
