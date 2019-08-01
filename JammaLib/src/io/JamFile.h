///////////////////////////////////////////////////////////
//
// Author 2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <stack>
#include <optional>
#include <fstream>
#include <iostream>
#include <sstream>
#include "../audio/AudioMixer.h"

namespace io
{
	struct JamFile
	{
		enum Version
		{
			VERSION_V,
			VERSION_LEGACY
		};

		enum BlockType
		{
			BLOCK_NONE,
			BLOCK_ROOT,
			BLOCK_STATION,
			BLOCK_LOOPTAKE,
			BLOCK_LOOP
		};

		struct Block
		{
			virtual JamFile::BlockType BlockType() const { return BLOCK_NONE; }
		};

		struct Loop :
			public Block
		{
			bool Muted;
			unsigned int MuteGroups;
			unsigned int SelectGroups;
			double Speed;
			double Level;
			unsigned long Index;
			unsigned long MasterLoopCount;
			audio::BehaviourParams Behaviour;

			virtual JamFile::BlockType BlockType() const override { return BLOCK_LOOP; }
			static Loop FromString(std::string str);
		};

		struct LoopTake :
			public Block
		{
			std::vector<Loop> Loops;

			virtual JamFile::BlockType BlockType() const override { return BLOCK_LOOPTAKE; }
			static LoopTake FromString(std::string str);
		};

		struct Station :
			public Block
		{
			std::vector<LoopTake> LoopTakes;

			virtual JamFile::BlockType BlockType() const override { return BLOCK_STATION; }
			static Station FromString(std::string str);
		};

	public:
		static std::optional<JamFile> FromFile(std::wstring file);
		static bool ToFile(std::wstring file, JamFile jam);
		
	public:
		enum JsonScalarType
		{
			JSON_UNKNOWN,
			JSON_BOOL,
			JSON_INT,
			JSON_DOUBLE,
			JSON_STRING
		};

		struct JsonArray;
		struct JsonPart;
		typedef std::variant<bool, int, double, std::string, JsonArray, JsonPart> JsonValue;
		typedef std::variant<bool, int, double, std::string> JsonScalar;

		struct JsonArray
		{
			unsigned int Length;
			std::variant<std::vector<bool>, std::vector<int>, std::vector<double>, std::vector<std::string>> Array;
		};

		struct JsonPart
		{
			std::map<std::string, JsonValue> KeyValues;
		};

		struct KeyResult
		{
			std::stringstream Stream;
			std::optional<std::string> Key;
		};

		struct ValueResult
		{
			std::stringstream Stream;
			std::optional<JsonValue> Value;

		};

		struct PartResult
		{
			std::stringstream Stream;
			std::optional<JsonPart> Part;
		};

		static std::optional<JsonValue> FromStream(std::stringstream ss);
		static bool ToStream(JsonPart json, std::stringstream ss);

		static KeyResult ParseKey(std::stringstream ss);
		static ValueResult ParseValue(std::stringstream ss);
		static PartResult ParseJsonPart(std::stringstream ss);
		static JsonArray ParseJsonArray(std::vector<std::string> values);

		static bool IsAllDigits(std::string str, bool includePeriod);
		static bool IsTrue(std::string str);

	public:
		Version Version;
		std::vector<Station> Stations;
		unsigned long TimerTicks;
	};
}
