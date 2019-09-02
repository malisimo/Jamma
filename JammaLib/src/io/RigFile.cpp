///////////////////////////////////////////////////////////
//
// Author 2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#include "RigFile.h"

using namespace io;

std::optional<RigFile> RigFile::FromStream(std::stringstream ss)
{
	auto root = Json::FromStream(std::move(ss));

	if (!root.has_value())
		return std::nullopt;

	if (root.value().index() != 6)
		return std::nullopt;

	auto rigParams = std::get<Json::JsonPart>(root.value());

	if (rigParams.KeyValues.find("name") == rigParams.KeyValues.end())
		return std::nullopt;

	if (rigParams.KeyValues["name"].index() != 4)
		return std::nullopt;

	RigFile rig;
	rig.Version = VERSION_V;
	rig.Name = std::get<std::string>(rigParams.KeyValues["name"]);

	auto gotAudio = false;
	auto iter = rigParams.KeyValues.find("audio");
	if (iter != rigParams.KeyValues.end())
	{
		if (rigParams.KeyValues["audio"].index() == 6)
		{
			auto audioJson = std::get<Json::JsonPart>(rigParams.KeyValues["audio"]);
			auto audioOpt = AudioSettings::FromJson(audioJson);

			if (audioOpt.has_value())
			{
				rig.Audio = audioOpt.value();
				gotAudio = true;
			}
		}
	}

	if (!gotAudio)
		return std::nullopt;

	iter = rigParams.KeyValues.find("triggers");
	if (iter != rigParams.KeyValues.end())
	{
		if (rigParams.KeyValues["triggers"].index() == 5)
		{
			auto triggerArr = std::get<Json::JsonArray>(rigParams.KeyValues["triggers"]);
			if (triggerArr.Array.index() == 5)
			{
				auto triggers = std::get<std::vector<Json::JsonPart>>(triggerArr.Array);

				for (auto triggerJson : triggers)
				{
					auto triggerOpt = Trigger::FromJson(triggerJson);
					if (triggerOpt.has_value())
						rig.Triggers.push_back(triggerOpt.value());
				}
			}
		}
	}

	return rig;
}

bool RigFile::ToStream(RigFile rig, std::stringstream ss)
{
	return false;
}

std::optional<RigFile::AudioSettings> RigFile::AudioSettings::FromJson(Json::JsonPart json)
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

std::optional<RigFile::TriggerPair> RigFile::TriggerPair::FromJson(Json::JsonPart json)
{
	unsigned int activateDown = 0;
	unsigned int activateUp = 0;
	unsigned int ditchDown = 0;
	unsigned int ditchUp = 0;

	auto iter = json.KeyValues.find("activatedown");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["activatedown"].index() == 2)
			activateDown = std::get<unsigned long>(json.KeyValues["activatedown"]);
	}

	iter = json.KeyValues.find("activateup");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["activateup"].index() == 2)
			activateUp = std::get<unsigned long>(json.KeyValues["activateup"]);
	}

	iter = json.KeyValues.find("ditchdown");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["ditchdown"].index() == 2)
			ditchDown = std::get<unsigned long>(json.KeyValues["ditchdown"]);
	}

	iter = json.KeyValues.find("ditchup");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["ditchup"].index() == 2)
			ditchUp = std::get<unsigned long>(json.KeyValues["ditchup"]);
	}

	if ((0 == activateDown) && (0 == activateUp))
		return std::nullopt;

	if ((0 == ditchDown) && (0 == ditchUp))
		return std::nullopt;

	TriggerPair pair;
	pair.ActivateDown = activateDown;
	pair.ActivateUp = activateUp;
	pair.DitchDown = ditchDown;
	pair.DitchUp = ditchUp;
	return pair;
}

std::optional<RigFile::Trigger> RigFile::Trigger::FromJson(Json::JsonPart json)
{
	std::string name;
	unsigned int stationType = 0;
	std::vector<TriggerPair> pairs;
	std::vector<unsigned int> inputChannels;

	auto iter = json.KeyValues.find("name");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["name"].index() == 4)
			name = std::get<std::string>(json.KeyValues["name"]);
	}

	iter = json.KeyValues.find("stationtype");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["stationtype"].index() == 2)
			stationType = std::get<unsigned long>(json.KeyValues["stationtype"]);
	}

	iter = json.KeyValues.find("pairs");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["pairs"].index() == 5)
		{
			auto jsonArray = std::get<Json::JsonArray>(json.KeyValues["pairs"]);
			
			if (jsonArray.Array.index() == 5)
			{
				auto pairArray = std::get<std::vector<Json::JsonPart>>(jsonArray.Array);
				for (auto pairJson : pairArray)
				{
					auto pair = TriggerPair::FromJson(pairJson);
					if (pair.has_value())
						pairs.push_back(pair.value());
				}
			}
		}
	}

	iter = json.KeyValues.find("input");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["input"].index() == 5)
		{
			auto jsonArray = std::get<Json::JsonArray>(json.KeyValues["input"]);

			if (jsonArray.Array.index() == 2)
			{
				auto inChans = std::get<std::vector<unsigned long>>(jsonArray.Array);
				for (auto chan : inChans)
				{
					if (inputChannels.end() == std::find(inputChannels.begin(), inputChannels.end(), chan))
						inputChannels.push_back((unsigned int)chan);
				}
			}
		}
	}

	if (pairs.empty() || name.empty())
		return std::nullopt;

	Trigger trigger;
	trigger.Name = name;
	trigger.StationType = stationType;
	trigger.TriggerPairs = pairs;
	trigger.InputChannels = inputChannels;
	return trigger;
}
