#pragma once

#include <iostream>
#include <string>
#include <any>
#include <memory>
#include <optional>
#include <functional>
#include "../base/Audible.h"
#include "rtaudio/RtAudio.h"

class AudioDevice
{
public:
	AudioDevice();
	~AudioDevice();

public:
	void SetDevice(std::unique_ptr<RtAudio> device);
	RtAudio::DeviceInfo Current();

private:
	std::unique_ptr<RtAudio> _current;

public:
	static std::optional<std::unique_ptr<RtAudio>> Start(
		std::function<int(void*, void*, unsigned int, double, RtAudioStreamStatus, void*)> onAudio,
		std::function<void(RtAudioError::Type, const std::string&)> onError,
		Audible* audioReceiver);
};

