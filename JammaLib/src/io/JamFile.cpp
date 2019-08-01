///////////////////////////////////////////////////////////
//
// Author 2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#include "JamFile.h"

using namespace io;
using audio::BehaviourParams;

JamFile::Loop JamFile::Loop::FromString(std::string str)
{
	double level;
	double speed;
	bool isMuted;
	unsigned int muteGoups;
	unsigned int selectGoups;
	unsigned long index;
	unsigned long length;

	return Loop();
}

JamFile::LoopTake JamFile::LoopTake::FromString(std::string str)
{
	return LoopTake();
}

JamFile::Station JamFile::Station::FromString(std::string str)
{
	return Station();
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
			std::vector<std::string> values;

			while (ss >> c)
			{
				if ('"' == c)
					continue;

				auto isArrayEnd = ']' == c;
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

					if (IsAllDigits(str, false))
					{
						int v;
						if (ss2 >> v)
							return { std::move(ss), v };
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
	// Try to parse as bool, int, double or string
	JsonArray jsonArray;
	jsonArray.Length = (unsigned int)values.size();

	// Work out type of the first entry
	auto jsonType = JSON_UNKNOWN;
	if (!values.empty())
	{
		auto firstValue = values[0];
		if (!firstValue.empty())
		{
			if (IsAllDigits(values[0], false))
				jsonType = JSON_INT;
			else if (IsAllDigits(values[0], true))
				jsonType = JSON_DOUBLE;
			else
			{
				std::vector<char> charBuf;
				for (auto letter : values[0])
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
	std::vector<int> intVec;
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
	case JSON_INT:
		for (auto str : values)
		{
			std::stringstream ss(str);
			int v;
			if (ss >> v)
				intVec.push_back(v);
			else
				intVec.push_back(0);
		}
		jsonArray.Array = intVec;
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
	for (auto ch : str)
	{
		if (!std::isdigit(ch))
		{
			if (includePeriod)
			{
				if ('.' != ch)
					return false;
			}
			else
				return false;
		}
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