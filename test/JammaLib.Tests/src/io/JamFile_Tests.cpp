
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
	auto str = "{\"int\":12}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(json.has_value());
	ASSERT_FALSE(json.value().KeyValues.empty());

	auto kv = json.value().KeyValues["int"];
	auto res = std::get<int>(kv);

	ASSERT_EQ(12, res);
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
	auto str = "{\"string\":\"hello\",\"int\":7}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(json.has_value());
	ASSERT_FALSE(json.value().KeyValues.empty());

	auto kv1 = json.value().KeyValues["string"];
	auto res1 = std::get<std::string>(kv1);
	auto kv2 = json.value().KeyValues["int"];
	auto res2 = std::get<int>(kv2);

	ASSERT_EQ("hello", res1);
	ASSERT_EQ(7, res2);
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
	auto str = "{\"arr\":[3,4]}";
	auto testStream = std::stringstream(str);
	auto json = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(json.has_value());
	ASSERT_FALSE(json.value().KeyValues.empty());

	auto kv = json.value().KeyValues["arr"];
	auto g = std::get<JamFile::JsonArray>(kv);
	auto res = std::get<std::vector<int>>(g.Array);

	ASSERT_EQ(2, res.size());
	ASSERT_EQ(3, res[0]);
	ASSERT_EQ(4, res[1]);
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
