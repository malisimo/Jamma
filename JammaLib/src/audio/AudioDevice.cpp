#include "AudioDevice.h"

AudioDevice::AudioDevice()
{
}

AudioDevice::~AudioDevice()
{
}

void AudioDevice::SetDevice(std::unique_ptr<RtAudio> device)
{
	_current = std::move(device);
}

RtAudio::DeviceInfo AudioDevice::Current()
{
	return _current->getDeviceInfo(0);
}

std::optional<std::unique_ptr<RtAudio>> AudioDevice::Start(
	std::function<int(void*,void*,unsigned int,double,RtAudioStreamStatus,void*)> onAudio,
	std::function<void(RtAudioError::Type,const std::string&)> onError,
	Audible* audioReceiver)
{
	std::unique_ptr<RtAudio> rtAudio;

	try
	{
		rtAudio = std::make_unique<RtAudio>(RtAudio::WINDOWS_DS);
	}
	catch (RtAudioError& err)
	{
		std::cout << "Error instantiating DirectSound API: " << err.getMessage() << std::endl;
		return std::nullopt;
	}

	auto inDeviceNum = rtAudio->getDefaultInputDevice();
	auto outDeviceNum = rtAudio->getDefaultOutputDevice();

	RtAudio::StreamParameters inParams;
	inParams.deviceId = inDeviceNum;
	inParams.firstChannel = 0;
	inParams.nChannels = 2;
	RtAudio::StreamParameters outParams;
	outParams.deviceId = outDeviceNum;
	outParams.firstChannel = 0;
	outParams.nChannels = 2;

	RtAudio::StreamOptions streamOptions;
	streamOptions.numberOfBuffers = 1;
	streamOptions.flags = RTAUDIO_MINIMIZE_LATENCY;
	
	unsigned int bufFrames = 1;

	auto deviceCount = rtAudio->getDeviceCount();
	auto inDev = rtAudio->getDeviceInfo(inDeviceNum);
	auto outDev = rtAudio->getDeviceInfo(outDeviceNum);

	std::cout << "Opening audio stream" << std::endl;
	std::cout << "[Input Device] " << inParams.deviceId << " : " << inParams.nChannels << "ch" << std::endl;
	std::cout << "[Output Device] " << outParams.deviceId << " : " << outParams.nChannels << "ch" << std::endl;

	try
	{
		rtAudio->openStream(&outParams,
			&inParams,
			RTAUDIO_FLOAT32,
			44100,
			&bufFrames,
			*onAudio.target<RtAudioCallback>(),
			(void*)audioReceiver,
			&streamOptions,
			nullptr);
			//*onError.target<RtAudioErrorCallback>());
	}
	catch (RtAudioError& err)
	{
		std::cout << "Error opening audio stream: " << err.getMessage() << std::endl;
		return std::nullopt;
	}
	
	if (!rtAudio->isStreamOpen())
		return std::nullopt;

	if (!rtAudio->isStreamRunning())
		return std::nullopt;

	return rtAudio;
}