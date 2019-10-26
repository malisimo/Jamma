///////////////////////////////////////////////////////////
//
// Author 2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#include "UserConfig.h"

using namespace io;

std::optional<UserConfig> UserConfig::FromJson(Json::JsonPart json)
{
	if (json.KeyValues.find("name") == json.KeyValues.end())
		return std::nullopt;

	if (json.KeyValues["name"].index() != 4)
		return std::nullopt;

	UserConfig cfg;
	cfg.Name = std::get<std::string>(json.KeyValues["name"]);

	auto gotAudio = false;
	auto iter = json.KeyValues.find("audio");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["audio"].index() == 6)
		{
			auto audioJson = std::get<Json::JsonPart>(json.KeyValues["audio"]);
			auto audioOpt = AudioSettings::FromJson(audioJson);

			if (audioOpt.has_value())
			{
				cfg.Audio = audioOpt.value();
				gotAudio = true;
			}
		}
	}

	if (!gotAudio)
		return std::nullopt;

	iter = json.KeyValues.find("loop");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["loop"].index() == 6)
		{
			auto loopJson = std::get<Json::JsonPart>(json.KeyValues["loop"]);
			auto loopOpt = LoopSettings::FromJson(loopJson);

			if (loopOpt.has_value())
				cfg.Loop = loopOpt.value();
		}
	}

	iter = json.KeyValues.find("trigger");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["trigger"].index() == 6)
		{
			auto trigJson = std::get<Json::JsonPart>(json.KeyValues["trigger"]);
			auto trigOpt = TriggerSettings::FromJson(trigJson);

			if (trigOpt.has_value())
				cfg.Trigger = trigOpt.value();
		}
	}

	return cfg;
}

std::optional<UserConfig::AudioSettings> UserConfig::AudioSettings::FromJson(Json::JsonPart json)
{
	std::string name;
	unsigned int bufSize = 0;
	unsigned int latency = 0;
	unsigned int numChannelsIn = 0;
	unsigned int numChannelsOut = 0;

	auto iter = json.KeyValues.find("name");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["name"].index() == 4)
			name = std::get<std::string>(json.KeyValues["name"]);
	}

	iter = json.KeyValues.find("bufsize");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["bufsize"].index() == 2)
			bufSize = std::get<unsigned long>(json.KeyValues["bufsize"]);
	}

	iter = json.KeyValues.find("numchannelsin");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["numchannelsin"].index() == 2)
			numChannelsIn = std::get<unsigned long>(json.KeyValues["numchannelsin"]);
	}

	iter = json.KeyValues.find("numchannelsout");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["numchannelsout"].index() == 2)
			numChannelsOut = std::get<unsigned long>(json.KeyValues["numchannelsout"]);
	}

	if ((0 == bufSize) || name.empty())
		return std::nullopt;

	if ((0 == numChannelsIn) && (0 == numChannelsOut))
		return std::nullopt;

	iter = json.KeyValues.find("latency");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["latency"].index() == 2)
			latency = std::get<unsigned long>(json.KeyValues["latency"]);
	}

	AudioSettings audio;
	audio.Name = name;
	audio.BufSize = bufSize;
	audio.Latency = latency;
	audio.NumChannelsIn = numChannelsIn;
	audio.NumChannelsOut = numChannelsOut;
	return audio;
}

std::optional<UserConfig::LoopSettings> UserConfig::LoopSettings::FromJson(Json::JsonPart json)
{
	unsigned int fadeSamps = 3000;

	auto iter = json.KeyValues.find("fadeSamps");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["fadeSamps"].index() == 2)
			fadeSamps = std::get<unsigned long>(json.KeyValues["fadeSamps"]);
	}

	LoopSettings loop;
	loop.FadeSamps = fadeSamps;
	return loop;
}

std::optional<UserConfig::TriggerSettings> UserConfig::TriggerSettings::FromJson(Json::JsonPart json)
{
	unsigned int preDelay = 0;
	unsigned int debounceSamps = 280;

	auto iter = json.KeyValues.find("preDelay");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["preDelay"].index() == 2)
			preDelay = std::get<unsigned long>(json.KeyValues["preDelay"]);
	}

	iter = json.KeyValues.find("debounceSamps");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["debounceSamps"].index() == 2)
			debounceSamps = std::get<unsigned long>(json.KeyValues["debounceSamps"]);
	}

	TriggerSettings trig;
	trig.PreDelay = preDelay;
	trig.DebounceSamps = debounceSamps;
	return trig;
}
