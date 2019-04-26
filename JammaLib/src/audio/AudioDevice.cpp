#include "AudioDevice.h"

using namespace audio;

AudioDevice::AudioDevice() :
	_inDeviceInfo(RtAudio::DeviceInfo()),
	_outDeviceInfo(RtAudio::DeviceInfo()),
	_stream(std::unique_ptr<RtAudio>())
{
}

AudioDevice::AudioDevice(RtAudio::DeviceInfo inDeviceInfo,
	RtAudio::DeviceInfo outDeviceInfo,
	std::unique_ptr<RtAudio> stream) :
	_inDeviceInfo(inDeviceInfo),
	_outDeviceInfo(outDeviceInfo),
	_stream(std::move(stream))
{
}

AudioDevice::~AudioDevice()
{
	if (!_stream)
		return;

	if (_stream->isStreamRunning())
		_stream->stopStream();

	if (_stream->isStreamOpen())
		_stream->closeStream();
}

void AudioDevice::SetDevice(std::unique_ptr<RtAudio> device)
{
	_stream = std::move(device);
}

void AudioDevice::Start()
{
	if (_stream)
		_stream->startStream();
}

RtAudio::DeviceInfo AudioDevice::GetInputStreamInfo()
{
	return _inDeviceInfo;
}

RtAudio::DeviceInfo AudioDevice::GetOutputStreamInfo()
{
	return _outDeviceInfo;
}

std::optional<std::unique_ptr<AudioDevice>> AudioDevice::Open(
	std::function<int(void*,void*,unsigned int,double,RtAudioStreamStatus,void*)> onAudio,
	std::function<void(RtAudioError::Type,const std::string&)> onError,
	void* AudioSink)
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

	unsigned int bufFrames = 512;

	auto deviceCount = rtAudio->getDeviceCount();
	auto inDeviceNum = rtAudio->getDefaultInputDevice();
	auto outDeviceNum = rtAudio->getDefaultOutputDevice();
	auto inDev = rtAudio->getDeviceInfo(inDeviceNum);
	auto outDev = rtAudio->getDeviceInfo(outDeviceNum);

	if ((inDev.inputChannels == 0) && (outDev.outputChannels == 0))
		return std::nullopt;

	RtAudio::StreamParameters inParams;
	inParams.deviceId = inDeviceNum;
	inParams.firstChannel = 0;
	inParams.nChannels = std::min(inDev.inputChannels, 2u);
	RtAudio::StreamParameters outParams;
	outParams.deviceId = outDeviceNum;
	outParams.firstChannel = 0;
	outParams.nChannels = std::min(outDev.outputChannels, 2u);

	RtAudio::StreamOptions streamOptions;
	streamOptions.numberOfBuffers = 4;
	streamOptions.flags = RTAUDIO_MINIMIZE_LATENCY;

	std::cout << "Opening audio stream" << std::endl;
	std::cout << "[Input Device] " << inParams.deviceId << " : " << inParams.nChannels << "ch" << std::endl;
	std::cout << "[Output Device] " << outParams.deviceId << " : " << outParams.nChannels << "ch" << std::endl;

	try
	{
		rtAudio->openStream(outParams.nChannels > 0 ? &outParams : nullptr,
			inParams.nChannels > 0 ? &inParams : nullptr,
			RTAUDIO_FLOAT32,
			44100,
			&bufFrames,
			*onAudio.target<RtAudioCallback>(),
			(void*)AudioSink,
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

	return std::make_unique<AudioDevice>(inDev, outDev, std::move(rtAudio));
}