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
			BLOCK_LOOP,
			BLOCK_LOOPMIX
		};

		struct JsonArray;
		struct JsonPart;
		typedef std::variant<bool, long, unsigned long, double, std::string, JsonArray, JsonPart> JsonValue;
		typedef std::variant<bool, long, unsigned long, double, std::string> JsonScalar;

		struct Block
		{
			virtual JamFile::BlockType BlockType() const { return BLOCK_NONE; }
		};

		struct LoopMix :
			public Block
		{
			enum MixType
			{
				MIX_WIRE,
				MIX_PAN
			};

			MixType Mix;
			std::variant<std::vector<unsigned long>,std::vector<double>> Params;

			virtual JamFile::BlockType BlockType() const override { return BLOCK_LOOPMIX; }
			static std::optional<LoopMix> FromJson(JsonPart json);
		};

		struct Loop :
			public Block
		{
			std::string Name;
			unsigned long Length;
			unsigned long Index;
			unsigned long MasterLoopCount;
			double Level;
			double Speed;
			unsigned int MuteGroups;
			unsigned int SelectGroups;
			bool Muted;
			LoopMix Mix;

			virtual JamFile::BlockType BlockType() const override { return BLOCK_LOOP; }
			static std::optional<Loop> FromJson(JsonPart json);
		};

		struct LoopTake :
			public Block
		{
			std::string Name;
			std::vector<Loop> Loops;

			virtual JamFile::BlockType BlockType() const override { return BLOCK_LOOPTAKE; }
			static std::optional<LoopTake> FromJson(JsonPart json);
		};

		struct Station :
			public Block
		{
			std::string Name;
			std::vector<LoopTake> LoopTakes;

			virtual JamFile::BlockType BlockType() const override { return BLOCK_STATION; }
			static std::optional<Station> FromJson(JsonPart json);
		};

	public:
		static std::optional<JamFile> FromFile(std::wstring file);
		static bool ToFile(std::wstring file, JamFile jam);
		
	public:
		enum JsonScalarType
		{
			JSON_UNKNOWN,
			JSON_BOOL,
			JSON_LONG,
			JSON_ULONG,
			JSON_DOUBLE,
			JSON_STRING,
			JSON_STRUCT
		};

		struct JsonArray
		{
			unsigned int Length;
			std::variant<std::vector<bool>, std::vector<long>, std::vector<unsigned long>, std::vector<double>, std::vector<std::string>, std::vector<JsonPart>> Array;
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
