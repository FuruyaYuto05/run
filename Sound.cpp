#include "Sound.h"

#include <cassert>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include "StringUtility.h"

#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

Sound* Sound::GetInstance() {
	static Sound instance;
	return &instance;
}

void Sound::Initialize() {
	HRESULT result = XAudio2Create(xAudio2_.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(result));
}

void Sound::Finalize() {
	for (PlayingVoice& playingVoice : sourceVoices_) {
		if (playingVoice.sourceVoice) {
			playingVoice.sourceVoice->Stop(0);
			playingVoice.sourceVoice->DestroyVoice();
		}
	}
	sourceVoices_.clear();

	if (masterVoice_) {
		masterVoice_->DestroyVoice();
		masterVoice_ = nullptr;
	}
	xAudio2_.Reset();
}

SoundData Sound::LoadFile(const std::string& filename) {
	const std::wstring filePathW = StringUtility::ConvertString(filename);

	Microsoft::WRL::ComPtr<IMFSourceReader> reader;
	HRESULT result = MFCreateSourceReaderFromURL(filePathW.c_str(), nullptr, reader.GetAddressOf());
	assert(SUCCEEDED(result));

	// 出力を非圧縮PCMに指定する
	Microsoft::WRL::ComPtr<IMFMediaType> pcmType;
	result = MFCreateMediaType(pcmType.GetAddressOf());
	assert(SUCCEEDED(result));
	result = pcmType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	assert(SUCCEEDED(result));
	result = pcmType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	assert(SUCCEEDED(result));
	result = reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pcmType.Get());
	assert(SUCCEEDED(result));

	// XAudio2で使うWaveフォーマットを取得する
	Microsoft::WRL::ComPtr<IMFMediaType> outputType;
	result = reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, outputType.GetAddressOf());
	assert(SUCCEEDED(result));

	WAVEFORMATEX* waveFormat = nullptr;
	result = MFCreateWaveFormatExFromMFMediaType(outputType.Get(), &waveFormat, nullptr);
	assert(SUCCEEDED(result));

	SoundData soundData{};
	soundData.wfex = *waveFormat;
	CoTaskMemFree(waveFormat);

	// デコードされたPCMデータを最後まで読み込み、バッファへ連結する
	while (true) {
		Microsoft::WRL::ComPtr<IMFSample> sample;
		DWORD streamIndex = 0;
		DWORD flags = 0;
		LONGLONG timestamp = 0;

		result = reader->ReadSample(
			MF_SOURCE_READER_FIRST_AUDIO_STREAM,
			0,
			&streamIndex,
			&flags,
			&timestamp,
			sample.GetAddressOf());
		assert(SUCCEEDED(result));

		if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
			break;
		}

		if (sample) {
			Microsoft::WRL::ComPtr<IMFMediaBuffer> buffer;
			result = sample->ConvertToContiguousBuffer(buffer.GetAddressOf());
			assert(SUCCEEDED(result));

			BYTE* pData = nullptr;
			DWORD maxLength = 0;
			DWORD currentLength = 0;
			result = buffer->Lock(&pData, &maxLength, &currentLength);
			assert(SUCCEEDED(result));

			soundData.buffer.insert(soundData.buffer.end(), pData, pData + currentLength);
			buffer->Unlock();
		}
	}

	return soundData;
}

void Sound::PlayWave(const SoundData& soundData) {
	IXAudio2SourceVoice* sourceVoice = nullptr;
	HRESULT result = xAudio2_->CreateSourceVoice(&sourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = soundData.buffer.data();
	buffer.AudioBytes = static_cast<UINT32>(soundData.buffer.size());
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	result = sourceVoice->SubmitSourceBuffer(&buffer);
	assert(SUCCEEDED(result));

	result = sourceVoice->Start();
	assert(SUCCEEDED(result));

	// 音声データを解放するまで、対応するソースボイスを管理する
	sourceVoices_.push_back({ sourceVoice, soundData.buffer.data() });
}

void Sound::Unload(SoundData* soundData) {
	const BYTE* audioData = soundData->buffer.data();
	for (auto it = sourceVoices_.begin(); it != sourceVoices_.end();) {
		if (it->audioData == audioData) {
			it->sourceVoice->Stop(0);
			it->sourceVoice->DestroyVoice();
			it = sourceVoices_.erase(it);
		} else {
			++it;
		}
	}

	soundData->buffer.clear();
	soundData->wfex = {};
}
