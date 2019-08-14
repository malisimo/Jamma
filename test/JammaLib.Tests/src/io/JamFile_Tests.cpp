
#include "gtest/gtest.h"
#include "resources/ResourceLib.h"
#include "io/JamFile.h"

using io::JamFile;

class JamFileMock :
	public JamFile
{
public:
	static std::optional<JamFileMock::JsonPart> JsonFromStream(std::stringstream ss)
	{
		return std::get<JamFile::JsonPart>(FromStream(std::move(ss)).value());
	}
	static std::optional<Loop> LoopFromJson(JamFile::JsonPart json)
	{
		return Loop::FromJson(json);
	}
};

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
	auto str = "{\"name\":\"loop\",\"length\":220,\"index\":2,\"masterloopcount\":7,\"level\":0.56,\"speed\":1.2,\"mutegroups\":11,\"selectgroups\":15,\"muted\":false,\"mix\":{\"type\":\"pan\",\"chans\":[0.2,0.8]}}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));
	auto loop = JamFileMock::LoopFromJson(json.value());

	ASSERT_TRUE(loop.has_value());
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