#pragma once

#include <iostream>
#include <string>
#include <any>
#include <algorithm>
#include <memory>
#include <optional>
#include <functional>
#include "../base/Audible.h"
#include "rtaudio/RtAudio.h"

namespace audio
{
	class AudioDevice
	{
	public:
		AudioDevice();
		AudioDevice(RtAudio::DeviceInfo inDeviceInfo,
			RtAudio::DeviceInfo outDeviceInfo,
			std::unique_ptr<RtAudio> stream);
		~AudioDevice();

	public:
		void SetDevice(std::unique_ptr<RtAudio> device);
		void Start();
		RtAudio::DeviceInfo GetInputStreamInfo();
		RtAudio::DeviceInfo GetOutputStreamInfo();

	private:
		RtAudio::DeviceInfo _inDeviceInfo;
		RtAudio::DeviceInfo _outDeviceInfo;
		std::unique_ptr<RtAudio> _stream;

	public:
		static std::optional<std::unique_ptr<AudioDevice>> Open(
			std::function<int(void*, void*, unsigned int, double, RtAudioStreamStatus, void*)> onAudio,
			std::function<void(RtAudioError::Type, const std::string&)> onError,
			void* audioReceiver);
	};
}
