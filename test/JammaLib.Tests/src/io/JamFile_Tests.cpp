
#include "gtest/gtest.h"
#include <regex>
#include "resources/ResourceLib.h"
#include "io/JamFile.h"

using io::JamFile;

class JamFileMock :
	public JamFile
{
public:
	static std::optional<JamFileMock::JsonPart> JsonFromStream(std::stringstream ss)
	{
		return std::get<JamFile::JsonPart>(JamFile::JsonFromStream(std::move(ss)).value());
	}
	static std::optional<Loop> LoopFromJson(JamFile::JsonPart json)
	{
		return Loop::FromJson(json);
	}
	static std::optional<LoopTake> LoopTakeFromJson(JamFile::JsonPart json)
	{
		return LoopTake::FromJson(json);
	}
	static std::optional<Station> StationFromJson(JamFile::JsonPart json)
	{
		return Station::FromJson(json);
	}
};

const std::string LoopString = "{\"name\":\"%NAME%\",\"length\":220,\"index\":%INDEX%,\"masterloopcount\":7,\"level\":0.56,\"speed\":1.2,\"mutegroups\":11,\"selectgroups\":15,\"muted\":false,\"mix\":{\"type\":\"pan\",\"chans\":[0.2,0.8]}}";

TEST(JamFile, ParsesBool) {
	auto str = "{\"bool\":true}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(json.has_value());
	ASSERT_FALSE(json.value().KeyValues.empty());

	auto kv = json.value().KeyValues["bool"];
	auto res = std::get<bool>(kv);

	ASSERT_EQ(true,res);
}

TEST(JamFile, ParsesInt) {
	auto str = "{\"int\":-12}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(json.has_value());
	ASSERT_FALSE(json.value().KeyValues.empty());

	auto kv = json.value().KeyValues["int"];
	auto res = (int)std::get<long>(kv);

	ASSERT_EQ(-12, res);
}

TEST(JamFile, ParsesUnsignedLong) {
	auto str = "{\"ulong\":2147483648}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(json.has_value());
	ASSERT_FALSE(json.value().KeyValues.empty());

	auto kv = json.value().KeyValues["ulong"];
	auto res = std::get<unsigned long>(kv);

	ASSERT_EQ(2147483648ul, res);
}

TEST(JamFile, ParsesDouble) {
	auto str = "{\"double\":0.34}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(json.has_value());
	ASSERT_FALSE(json.value().KeyValues.empty());

	auto kv = json.value().KeyValues["double"];
	auto res = std::get<double>(kv);

	ASSERT_EQ(0.34, res);
}

TEST(JamFile, ParsesString) {
	auto str = "{\"string\":\"hello\"}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(json.has_value());
	ASSERT_FALSE(json.value().KeyValues.empty());

	auto kv = json.value().KeyValues["string"];
	auto res = std::get<std::string>(kv);

	ASSERT_EQ("hello", res);
}

TEST(JamFile, ParsesStringInt) {
	auto str = "{\"string\":\"hello\",\"int\":-7}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(json.has_value());
	ASSERT_FALSE(json.value().KeyValues.empty());

	auto kv1 = json.value().KeyValues["string"];
	auto res1 = std::get<std::string>(kv1);
	auto kv2 = json.value().KeyValues["int"];
	auto res2 = (int)std::get<long>(kv2);

	ASSERT_EQ("hello", res1);
	ASSERT_EQ(-7, res2);
}

TEST(JamFile, ParsesBoolArray) {
	auto str = "{\"arr\":[TrUe,fAlsE]}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(json.has_value());
	ASSERT_FALSE(json.value().KeyValues.empty());

	auto kv = json.value().KeyValues["arr"];
	auto g = std::get<JamFile::JsonArray>(kv);
	auto res = std::get<std::vector<bool>>(g.Array);

	ASSERT_EQ(2, res.size());
	ASSERT_EQ(true, res[0]);
	ASSERT_EQ(false, res[1]);
}

TEST(JamFile, ParsesIntArray) {
	auto str = "{\"arr\":[-3,4]}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(json.has_value());
	ASSERT_FALSE(json.value().KeyValues.empty());

	auto kv = json.value().KeyValues["arr"];
	auto g = std::get<JamFile::JsonArray>(kv);
	auto res = std::get<std::vector<long>>(g.Array);

	ASSERT_EQ(2, res.size());
	ASSERT_EQ(-3, (int)res[0]);
	ASSERT_EQ(4, (int)res[1]);
}

TEST(JamFile, ParsesUnsignedLongArray) {
	auto str = "{\"arr\":[333,444]}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(json.has_value());
	ASSERT_FALSE(json.value().KeyValues.empty());

	auto kv = json.value().KeyValues["arr"];
	auto g = std::get<JamFile::JsonArray>(kv);
	auto res = std::get<std::vector<unsigned long>>(g.Array);

	ASSERT_EQ(2, res.size());
	ASSERT_EQ(333ul, res[0]);
	ASSERT_EQ(444ul, res[1]);
}

TEST(JamFile, ParsesDoubleArray) {
	auto str = "{\"arr\":[1.1,2.2]}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(json.has_value());
	ASSERT_FALSE(json.value().KeyValues.empty());

	auto kv = json.value().KeyValues["arr"];
	auto g = std::get<JamFile::JsonArray>(kv);
	auto res = std::get<std::vector<double>>(g.Array);

	ASSERT_EQ(2, res.size());
	ASSERT_EQ(1.1, res[0]);
	ASSERT_EQ(2.2, res[1]);
}

TEST(JamFile, ParsesStringArray) {
	auto str = "{\"arr\":[\"hello\",\"you\"]}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(json.has_value());
	ASSERT_FALSE(json.value().KeyValues.empty());

	auto kv = json.value().KeyValues["arr"];
	auto g = std::get<JamFile::JsonArray>(kv);
	auto res = std::get<std::vector<std::string>>(g.Array);

	ASSERT_EQ(2, res.size());
	ASSERT_EQ("hello", res[0]);
	ASSERT_EQ("you", res[1]);
}

TEST(JamFile, ParsesStruct) {
	auto str = "{\"struct\":{\"name\":\"you\",\"business\":\"monkeying\"}}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(json.has_value());
	ASSERT_FALSE(json.value().KeyValues.empty());

	auto kv = json.value().KeyValues["struct"];
	auto h = std::get<JamFile::JsonPart>(kv);
	auto res1 = std::get<std::string>(h.KeyValues["name"]);
	auto res2 = std::get<std::string>(h.KeyValues["business"]);

	ASSERT_EQ("you", res1);
	ASSERT_EQ("monkeying", res2);
}

TEST(JamFile, ParsesLoop) {
	auto str = std::regex_replace(std::regex_replace(LoopString, std::regex("%NAME%"), "loop"), std::regex("%INDEX%"), "2");
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));
	auto loop = JamFileMock::LoopFromJson(json.value());

	ASSERT_TRUE(loop.has_value());
	ASSERT_EQ(0, loop.value().Name.compare("loop"));
	ASSERT_EQ(2, loop.value().Index);
	ASSERT_EQ(7, loop.value().MasterLoopCount);
	ASSERT_EQ(0.56, loop.value().Level);
	ASSERT_EQ(1.2, loop.value().Speed);
	ASSERT_EQ(11, loop.value().MuteGroups);
	ASSERT_EQ(15, loop.value().SelectGroups);
	ASSERT_EQ(false, loop.value().Muted);

	auto loopMix = loop.value().Mix;
	auto loopMixParams = std::get<std::vector<double>>(loopMix.Params);

	ASSERT_EQ(2, loopMixParams.size());
	ASSERT_EQ(0.2, loopMixParams[0]);
	ASSERT_EQ(0.8, loopMixParams[1]);
}

TEST(JamFile, ParsesLoopTake) {
	auto loop1 = std::regex_replace(std::regex_replace(LoopString, std::regex("%NAME%"), "loop1"), std::regex("%INDEX%"), "1");
	auto loop2 = std::regex_replace(std::regex_replace(LoopString, std::regex("%NAME%"), "loop2"), std::regex("%INDEX%"), "2");
	auto loop3 = std::regex_replace(std::regex_replace(LoopString, std::regex("%NAME%"), "loop3"), std::regex("%INDEX%"), "3");

	auto str = "{\"name\":\"take\",\"loops\":[" + loop1 + "," + loop2 + "," + loop3 + "]}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));
	auto take = JamFileMock::LoopTakeFromJson(json.value());

	ASSERT_TRUE(take.has_value());
	ASSERT_EQ(0, take.value().Name.compare("take"));
	ASSERT_EQ(3, take.value().Loops.size());

	ASSERT_EQ(0, take.value().Loops[0].Name.compare("loop1"));
	ASSERT_EQ(1, take.value().Loops[0].Index);
	ASSERT_EQ(1.2, take.value().Loops[0].Speed);

	ASSERT_EQ(0, take.value().Loops[1].Name.compare("loop2"));
	ASSERT_EQ(2, take.value().Loops[1].Index);
	ASSERT_EQ(1.2, take.value().Loops[1].Speed);

	ASSERT_EQ(0, take.value().Loops[2].Name.compare("loop3"));
	ASSERT_EQ(3, take.value().Loops[2].Index);
	ASSERT_EQ(1.2, take.value().Loops[2].Speed);
}

TEST(JamFile, ParsesStation) {
	auto loop1 = std::regex_replace(std::regex_replace(LoopString, std::regex("%NAME%"), "loop1"), std::regex("%INDEX%"), "1");
	auto loop2 = std::regex_replace(std::regex_replace(LoopString, std::regex("%NAME%"), "loop2"), std::regex("%INDEX%"), "2");
	auto take1 = "{\"name\":\"take1\",\"loops\":[" + loop1 + "," + loop2 + "]}";

	auto loop3 = std::regex_replace(std::regex_replace(LoopString, std::regex("%NAME%"), "loop3"), std::regex("%INDEX%"), "3");
	auto take2 = "{\"name\":\"take2\",\"loops\":[" + loop3 + "]}";

	auto str = "{\"name\":\"station\",\"takes\":[" + take1 + "," + take2 + "]}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));
	auto station = JamFileMock::StationFromJson(json.value());

	ASSERT_TRUE(station.has_value());
	ASSERT_EQ(0, station.value().Name.compare("station"));
	ASSERT_EQ(2, station.value().LoopTakes.size());

	ASSERT_EQ(0, station.value().LoopTakes[0].Name.compare("take1"));
	ASSERT_EQ(2, station.value().LoopTakes[0].Loops.size());

	ASSERT_EQ(0, station.value().LoopTakes[0].Loops[0].Name.compare("loop1"));
	ASSERT_EQ(1, station.value().LoopTakes[0].Loops[0].Index);
	ASSERT_EQ(1.2, station.value().LoopTakes[0].Loops[0].Speed);

	ASSERT_EQ(0, station.value().LoopTakes[0].Loops[1].Name.compare("loop2"));
	ASSERT_EQ(2, station.value().LoopTakes[0].Loops[1].Index);
	ASSERT_EQ(1.2, station.value().LoopTakes[0].Loops[1].Speed);

	ASSERT_EQ(0, station.value().LoopTakes[1].Name.compare("take2"));
	ASSERT_EQ(1, station.value().LoopTakes[1].Loops.size());

	ASSERT_EQ(0, station.value().LoopTakes[1].Loops[0].Name.compare("loop3"));
	ASSERT_EQ(3, station.value().LoopTakes[1].Loops[0].Index);
	ASSERT_EQ(1.2, station.value().LoopTakes[1].Loops[0].Speed);
}

TEST(JamFile, ParsesFile) {

	auto loop1 = std::regex_replace(std::regex_replace(LoopString, std::regex("%NAME%"), "loop1"), std::regex("%INDEX%"), "1");
	auto loop2 = std::regex_replace(std::regex_replace(LoopString, std::regex("%NAME%"), "loop2"), std::regex("%INDEX%"), "2");
	auto take1 = "{\"name\":\"take1\",\"loops\":[" + loop1 + "," + loop2 + "]}";

	auto loop3 = std::regex_replace(std::regex_replace(LoopString, std::regex("%NAME%"), "loop3"), std::regex("%INDEX%"), "3");
	auto take2 = "{\"name\":\"take2\",\"loops\":[" + loop3 + "]}";

	auto station1 = "{\"name\":\"station1\",\"takes\":[" + take1 + "," + take2 + "]}";

	auto loop4 = std::regex_replace(std::regex_replace(LoopString, std::regex("%NAME%"), "loop4"), std::regex("%INDEX%"), "4");
	auto loop5 = std::regex_replace(std::regex_replace(LoopString, std::regex("%NAME%"), "loop5"), std::regex("%INDEX%"), "5");
	auto take3 = "{\"name\":\"take3\",\"loops\":[" + loop4 + "," + loop5 + "]}";
	
	auto loop6 = std::regex_replace(std::regex_replace(LoopString, std::regex("%NAME%"), "loop6"), std::regex("%INDEX%"), "6");
	auto take4 = "{\"name\":\"take4\",\"loops\":[" + loop6 + "]}";

	auto loop7 = std::regex_replace(std::regex_replace(LoopString, std::regex("%NAME%"), "loop7"), std::regex("%INDEX%"), "7");
	auto take5 = "{\"name\":\"take5\",\"loops\":[" + loop7 + "]}";

	auto station2 = "{\"name\":\"station2\",\"takes\":[" + take3 + "," + take4 + "," + take5 + "]}";

	auto str = "{\"name\":\"jam\",\"stations\":[" + station1 + "," + station2 + "]}";
	auto testStream = std::stringstream(str);
	auto jam = JamFile::FromStream(std::move(testStream));

	ASSERT_TRUE(jam.has_value());
	ASSERT_EQ(JamFile::VERSION_V, jam.value().Version);
	ASSERT_EQ(0, jam.value().Name.compare("jam"));
	ASSERT_EQ(2, jam.value().Stations.size());


	ASSERT_EQ(0, jam.value().Stations[0].Name.compare("station1"));
	ASSERT_EQ(2, jam.value().Stations[0].LoopTakes.size());

	ASSERT_EQ(0, jam.value().Stations[0].LoopTakes[0].Name.compare("take1"));
	ASSERT_EQ(2, jam.value().Stations[0].LoopTakes[0].Loops.size());

	ASSERT_EQ(0, jam.value().Stations[0].LoopTakes[0].Loops[0].Name.compare("loop1"));
	ASSERT_EQ(1, jam.value().Stations[0].LoopTakes[0].Loops[0].Index);
	ASSERT_EQ(1.2, jam.value().Stations[0].LoopTakes[0].Loops[0].Speed);

	ASSERT_EQ(0, jam.value().Stations[0].LoopTakes[0].Loops[1].Name.compare("loop2"));
	ASSERT_EQ(2, jam.value().Stations[0].LoopTakes[0].Loops[1].Index);
	ASSERT_EQ(1.2, jam.value().Stations[0].LoopTakes[0].Loops[1].Speed);

	ASSERT_EQ(0, jam.value().Stations[0].LoopTakes[1].Name.compare("take2"));
	ASSERT_EQ(1, jam.value().Stations[0].LoopTakes[1].Loops.size());

	ASSERT_EQ(0, jam.value().Stations[0].LoopTakes[1].Loops[0].Name.compare("loop3"));
	ASSERT_EQ(3, jam.value().Stations[0].LoopTakes[1].Loops[0].Index);
	ASSERT_EQ(1.2, jam.value().Stations[0].LoopTakes[1].Loops[0].Speed);


	ASSERT_EQ(0, jam.value().Stations[1].Name.compare("station2"));
	ASSERT_EQ(3, jam.value().Stations[1].LoopTakes.size());

	ASSERT_EQ(0, jam.value().Stations[1].LoopTakes[0].Name.compare("take3"));
	ASSERT_EQ(2, jam.value().Stations[1].LoopTakes[0].Loops.size());

	ASSERT_EQ(0, jam.value().Stations[1].LoopTakes[0].Loops[0].Name.compare("loop4"));
	ASSERT_EQ(4, jam.value().Stations[1].LoopTakes[0].Loops[0].Index);
	ASSERT_EQ(1.2, jam.value().Stations[1].LoopTakes[0].Loops[0].Speed);

	ASSERT_EQ(0, jam.value().Stations[1].LoopTakes[0].Loops[1].Name.compare("loop5"));
	ASSERT_EQ(5, jam.value().Stations[1].LoopTakes[0].Loops[1].Index);
	ASSERT_EQ(1.2, jam.value().Stations[1].LoopTakes[0].Loops[1].Speed);

	ASSERT_EQ(0, jam.value().Stations[1].LoopTakes[1].Name.compare("take4"));
	ASSERT_EQ(1, jam.value().Stations[1].LoopTakes[1].Loops.size());

	ASSERT_EQ(0, jam.value().Stations[1].LoopTakes[1].Loops[0].Name.compare("loop6"));
	ASSERT_EQ(6, jam.value().Stations[1].LoopTakes[1].Loops[0].Index);
	ASSERT_EQ(1.2, jam.value().Stations[1].LoopTakes[1].Loops[0].Speed);

	ASSERT_EQ(0, jam.value().Stations[1].LoopTakes[2].Name.compare("take5"));
	ASSERT_EQ(1, jam.value().Stations[1].LoopTakes[2].Loops.size());

	ASSERT_EQ(0, jam.value().Stations[1].LoopTakes[2].Loops[0].Name.compare("loop7"));
	ASSERT_EQ(7, jam.value().Stations[1].LoopTakes[2].Loops[0].Index);
	ASSERT_EQ(1.2, jam.value().Stations[1].LoopTakes[2].Loops[0].Speed);
}