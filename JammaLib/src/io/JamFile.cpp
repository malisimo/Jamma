///////////////////////////////////////////////////////////
//
// Author 2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#include "JamFile.h"

using namespace io;
using audio::BehaviourParams;


std::optional<JamFile::LoopMix> JamFile::LoopMix::FromJson(JsonPart json)
{
	std::string typeStr;
	std::vector<Loop> loops;

	auto iter = json.KeyValues.find("type");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["type"].index() == 4)
			typeStr = std::get<std::string>(json.KeyValues["type"]);
	}

	LoopMix mix;

	if (typeStr.compare("wire") == 0)
		mix.Mix = MIX_WIRE;
	else if (typeStr.compare("pan") == 0)
		mix.Mix = MIX_PAN;
	else
		return std::nullopt;

	switch (mix.Mix)
	{
	case MIX_WIRE:
		iter = json.KeyValues.find("chans");
		if (iter == json.KeyValues.end())
			return std::nullopt;

		if (json.KeyValues["chans"].index() == 5)
		{
			auto arr = std::get<JsonArray>(json.KeyValues["chans"]);
			if (arr.Array.index() == 2)
				mix.Params = std::get<std::vector<unsigned long>>(arr.Array);
		}
		break;
	case MIX_PAN:
		iter = json.KeyValues.find("chans");
		if (iter == json.KeyValues.end())
			return std::nullopt;

		if (json.KeyValues["chans"].index() == 5)
		{
			auto arr = std::get<JsonArray>(json.KeyValues["chans"]);
			if (arr.Array.index() == 3)
				mix.Params = std::get<std::vector<double>>(arr.Array);
		}
		break;
	}

	return mix;
}

std::optional<JamFile::Loop> JamFile::Loop::FromJson(JsonPart json)
{
	std::string name;
	unsigned long length = 0;
	unsigned long index = 0;
	unsigned long masterLoopCount = 0;
	double level = 1.0;
	double speed = 1.0;
	unsigned int muteGroups = 0;
	unsigned int selectGroups = 0;
	bool isMuted = false;
	LoopMix mix;

	auto iter = json.KeyValues.find("name");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["name"].index() == 4)
			name = std::get<std::string>(json.KeyValues["name"]);
	}

	iter = json.KeyValues.find("length");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["length"].index() == 2)
			length = std::get<unsigned long>(json.KeyValues["length"]);
	}

	if ((0 == length) || name.empty())
		return std::nullopt;

	iter = json.KeyValues.find("index");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["index"].index() == 2)
			index = std::get<unsigned long>(json.KeyValues["index"]);
	}

	iter = json.KeyValues.find("masterloopcount");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["masterloopcount"].index() == 2)
			masterLoopCount = std::get<unsigned long>(json.KeyValues["masterloopcount"]);
	}

	iter = json.KeyValues.find("level");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["level"].index() == 3)
			level = std::get<double>(json.KeyValues["level"]);
	}

	iter = json.KeyValues.find("speed");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["speed"].index() == 3)
			speed = std::get<double>(json.KeyValues["speed"]);
	}

	iter = json.KeyValues.find("mutegroups");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["mutegroups"].index() == 2)
			muteGroups = std::get<unsigned long>(json.KeyValues["mutegroups"]);
	}

	iter = json.KeyValues.find("selectgroups");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["selectgroups"].index() == 2)
			selectGroups = std::get<unsigned long>(json.KeyValues["selectgroups"]);
	}

	iter = json.KeyValues.find("muted");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["muted"].index() == 0)
			isMuted = std::get<bool>(json.KeyValues["muted"]);
	}

	iter = json.KeyValues.find("mix");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["mix"].index() == 6)
		{
			auto mixOpt = LoopMix::FromJson(std::get<JsonPart>(json.KeyValues["mix"]));
			if (mixOpt.has_value())
				mix = mixOpt.value();
		}
	}

	Loop loop;
	loop.Name = name;
	loop.Length = length;
	loop.Index = index;
	loop.MasterLoopCount = masterLoopCount;
	loop.Level = level;
	loop.Speed = speed;
	loop.MuteGroups = muteGroups;
	loop.SelectGroups = selectGroups;
	loop.Muted = isMuted;
	loop.Mix = mix;
	return loop;
}

std::optional<JamFile::LoopTake> JamFile::LoopTake::FromJson(JsonPart json)
{
	std::string name;
	std::vector<Loop> loops;

	auto iter = json.KeyValues.find("name");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["name"].index() == 4)
			name = std::get<std::string>(json.KeyValues["name"]);
	}

	iter = json.KeyValues.find("loops");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["loops"].index() == 5)
		{
			auto jsonArray = std::get<JsonArray>(json.KeyValues["loops"]);
			
			if (jsonArray.Array.index() == 4)
			{
				auto loopArray = std::get<std::vector<JsonPart>>(jsonArray.Array);
				for (auto loopJson : loopArray)
				{
					auto loop = Loop::FromJson(loopJson);
					if (loop.has_value())
						loops.push_back(loop.value());
				}
			}
		}
	}

	if (loops.empty() || name.empty())
		return std::nullopt;

	LoopTake take;
	take.Name = name;
	take.Loops = loops;
	return take;
}

std::optional<JamFile::Station> JamFile::Station::FromJson(JsonPart json)
{
	std::string name;
	std::vector<LoopTake> takes;

	auto iter = json.KeyValues.find("name");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["name"].index() == 4)
			name = std::get<std::string>(json.KeyValues["name"]);
	}

	iter = json.KeyValues.find("takes");
	if (iter != json.KeyValues.end())
	{
		if (json.KeyValues["takes"].index() == 5)
		{
			auto jsonArray = std::get<JsonArray>(json.KeyValues["loops"]);

			if (jsonArray.Array.index() == 4)
			{
				auto takeArray = std::get<std::vector<JsonPart>>(jsonArray.Array);
				for (auto takeJson : takeArray)
				{
					auto take = LoopTake::FromJson(takeJson);
					if (take.has_value())
						takes.push_back(take.value());
				}
			}
		}
	}

	if (name.empty())
		return std::nullopt;

	Station station;
	station.Name = name;
	station.LoopTakes = takes;
	return station;
}

std::optional<JamFile> JamFile::FromFile(std::wstring file)
{
	std::ifstream ifstream(file);

	if (!ifstream.good())
		return std::nullopt;

	std::stringstream stream;
	stream << ifstream.rdbuf();

	auto json = FromStream(std::move(stream));
	return std::nullopt;
}

bool JamFile::ToFile(std::wstring file, JamFile jam)
{
	return false;
}

std::optional<JamFile::JsonValue> JamFile::FromStream(std::stringstream ss)
{
	auto root = ParseValue(std::move(ss));
	return root.Value;
}

bool JamFile::ToStream(JamFile::JsonPart json, std::stringstream ss)
{
	return false;
}

JamFile::KeyResult JamFile::ParseKey(std::stringstream ss)
{	
	std::string key;
	char c;
	//bool isInsideQuotes = false;
	std::vector<char> charBuf;

	while (ss >> c)
	{
		/*if (!isInsideQuotes)
		{
			if ('"' == c)
				isInsideQuotes = true;
		}
		else
		{*/
			if ('"' == c)
			{
				//isInsideQuotes = false;
				charBuf.push_back('\0');
				key = std::string(charBuf.data());
				break;
			}
			else
				charBuf.push_back(c);
		//}
	}

	if (key.empty())
		return { std::move(ss), std::nullopt };

	return { std::move(ss), key };
}

JamFile::ValueResult JamFile::ParseValue(std::stringstream ss)
{
	char c;
	bool isInsideQuotes = false;
	std::vector<char> charBuf;

	while (ss >> c)
	{
		if ('{' == c)
		{
			// Value is a JsonPart (map of key values)
			auto res = ParseJsonPart(std::move(ss));

			if (std::nullopt != res.Part)
				return { std::move(res.Stream), res.Part };
			else
				return { std::move(res.Stream), std::nullopt };
		}
		else if ('[' == c)
		{
			charBuf.clear();
			// Value is array
			auto isArrayEnd = false;
			auto isFirstIt = true;
			std::vector<std::string> values;

			while (ss >> c)
			{
				if (' ' == c)
					continue;

				if (isFirstIt)
				{
					if ('{' == c)
					{
						std::vector<JsonPart> parts;

						while (ss && !isArrayEnd)
						{
							auto part = ParseJsonPart(std::move(ss));
							if (part.Part.has_value())
								parts.push_back(part.Part.value());

							while (ss >> c)
							{
								isArrayEnd = ']' == c;
								if ((',' == c) || isArrayEnd)
									break;
							}
						}

						JsonArray jsonArray;
						jsonArray.Length = (unsigned int)parts.size();
						jsonArray.Array = parts;
						return { std::move(ss), jsonArray };
					}

					isFirstIt = false;
				}

				if ('"' == c)
					continue;

				isArrayEnd = ']' == c;
				if ((',' == c) || isArrayEnd)
				{
					charBuf.push_back('\0');
					values.push_back(std::string(charBuf.data()));

					if (isArrayEnd)
						break;
					else
						charBuf.clear();
				}
				else
					charBuf.push_back(c);
			}

			return { std::move(ss), ParseJsonArray(values) };
		}
		else if ('"' == c)
		{
			charBuf.clear();

			while (ss >> c)
			{
				if ('"' == c)
				{
					charBuf.push_back('\0');
					return { std::move(ss), std::string(charBuf.data()) };
				}
				else
					charBuf.push_back(c);
			}
		}
		else
		{
			charBuf.clear();
			charBuf.push_back(c);

			bool finished = false;

			while (!finished)
			{
				if ('}' == ss.peek())
					finished = true;
				else if (ss >> c)
					finished = (',' == c);

				if (finished)
				{
					charBuf.push_back('\0');
					std::string str(charBuf.data());
					std::stringstream ss2(str);

					if ((str.size() > 0) && IsAllDigits(str, false))
					{
						if ('-' == str[0])
						{
							long v;
							if (ss2 >> v)
								return { std::move(ss), v };
						}
						else
						{
							unsigned long v;
							if (ss2 >> v)
								return { std::move(ss), unsigned long(v) };
						}
					}
					else if (IsAllDigits(str, true))
					{
						double v;
						if (ss2 >> v)
							return { std::move(ss), v };
					}
					else
					{
						std::vector<char> charBuf2;
						for (auto letter : str)
							charBuf2.push_back(std::tolower(letter));

						charBuf2.push_back('\0');
						std::string strLower(charBuf2.data());
						if ("true" == strLower)
							return { std::move(ss), true };
						else if ("false" == strLower)
							return { std::move(ss), false };
					}

					charBuf.push_back('\0');
					return { std::move(ss), std::string(charBuf.data()) };
				}
				else
					charBuf.push_back(c);
			}
		}
	}

	return { std::move(ss), std::nullopt };
}

JamFile::PartResult JamFile::ParseJsonPart(std::stringstream ss)
{
	char c;
	JsonPart part;

	while (ss >> c)
	{
		if ('}' == c)
			return { std::move(ss), part };
		else if ('\"' == c)
		{
			auto keyRes = ParseKey(std::move(ss));
			if (keyRes.Key.has_value())
			{
				ss = std::move(keyRes.Stream);

				while (ss >> c)
				{
					if (':' == c)
					{
						auto valueRes = ParseValue(std::move(ss));
						if (valueRes.Value.has_value())
						{
							part.KeyValues[keyRes.Key.value()] = valueRes.Value.value();
							ss = std::move(valueRes.Stream);
						}
						
						break;
					}
				}
			}
		}
	}

	return { std::move(ss), part };
}

JamFile::JsonArray JamFile::ParseJsonArray(std::vector<std::string> values)
{
	// Try to parse as bool, long, unsigned long, double, string or struct
	JsonArray jsonArray;
	jsonArray.Length = (unsigned int)values.size();

	// Work out type of the first entry
	auto jsonType = JSON_UNKNOWN;
	if (!values.empty())
	{
		auto firstValue = values[0];
		if (!firstValue.empty())
		{
			if (IsAllDigits(firstValue, false))
			{
				if ('-' == firstValue[0])
					jsonType = JSON_LONG;
				else
					jsonType = JSON_ULONG;
			}
			else if (IsAllDigits(firstValue, true))
				jsonType = JSON_DOUBLE;
			else
			{
				std::vector<char> charBuf;
				for (auto letter : firstValue)
					charBuf.push_back(std::tolower(letter));

				charBuf.push_back('\0');
				firstValue = std::string(charBuf.data());
				if (("true" == firstValue) || ("false" == firstValue))
					jsonType = JSON_BOOL;
				else
					jsonType = JSON_STRING;
			}
		}
	}

	std::vector<bool> boolVec;
	std::vector<long> longVec;
	std::vector<unsigned long> ulongVec;
	std::vector<double> doubleVec;

	switch (jsonType)
	{
	case JSON_BOOL:
		for (auto str : values)
		{
			boolVec.push_back(IsTrue(str));
		}
		jsonArray.Array = boolVec;
		break;
	case JSON_LONG:
		for (auto str : values)
		{
			std::stringstream ss(str);
			long v;
			if (ss >> v)
				longVec.push_back(v);
			else
				longVec.push_back(0);
		}
		jsonArray.Array = longVec;
		break;
	case JSON_ULONG:
		for (auto str : values)
		{
			std::stringstream ss(str);
			unsigned long v;
			if (ss >> v)
				ulongVec.push_back(v);
			else
				ulongVec.push_back(0);
		}
		jsonArray.Array = ulongVec;
		break;
	case JSON_DOUBLE:
		for (auto str : values)
		{
			std::stringstream ss(str);
			double v;
			if (ss >> v)
				doubleVec.push_back(v);
			else
				doubleVec.push_back(0.0);
		}
		jsonArray.Array = doubleVec;
		break;
	case JSON_STRING:
		jsonArray.Array = values;
		break;
	}

	return jsonArray;
}

bool JamFile::IsAllDigits(std::string str, bool includePeriod)
{
	auto isFirstChar = true;

	for (auto ch : str)
	{
		if (!std::isdigit(ch))
		{
			if (includePeriod)
			{
				if ('.' == ch)
					continue;
			}
			
			if (isFirstChar)
			{
				if ('-' != ch)
					return false;
			}
			else
				return false;
		}

		isFirstChar = false;
	}

	return true;
}

bool JamFile::IsTrue(std::string str)
{
	std::vector<char> charBuf;
	for (auto letter : str)
		charBuf.push_back(std::tolower(letter));

	charBuf.push_back('\0');
	auto strLower = std::string(charBuf.data());
	return "true" == strLower;
}