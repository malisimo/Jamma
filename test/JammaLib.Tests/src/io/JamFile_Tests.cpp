
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
		return FromStream(std::move(ss));
	}
};

struct VisitJsonBool
{
	bool operator()(bool b) { return b; }
};

struct VisitJsonInt
{
	int operator()(int i) { return i; }
};

struct VisitJsonDouble
{
	double operator()(double d) { return d; }
};

struct VisitJsonString
{
	const std::string& operator()(std::string& str) { return str; }
};

struct VisitJsonBoolArray
{
	const std::vector<bool>& operator()(std::vector<bool>& v) { return v; }
};

struct VisitJsonIntArray
{
	const std::vector<int>& operator()(std::vector<int>& v) { return v; }
};

struct VisitJsonDoubleArray
{
	const std::vector<double>& operator()(std::vector<double>& v) { return v; }
};

struct VisitJsonStringArray
{
	const std::vector<std::string>& operator()(std::vector<std::string>& v) { return v; }
};

struct VisitJsonArray
{
	const std::vector<double>& operator()(std::vector<double>& v) { return v; }
	const std::vector<double>& operator()(std::vector<std::string>& v) { return {}; }
};

TEST(JamFile, ParsesBool) {
	auto str = "{\"bool\":true}";
	auto testStream = std::stringstream(str);
	auto res = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(res.has_value());
	ASSERT_FALSE(res.value().KeyValues.empty());

	auto kv = res.value().KeyValues["bool"];
	bool res2 = false;
	std::visit([&res2](auto && arg) {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, bool>)
			res2 = arg;
	}, kv);

	ASSERT_EQ(true,res2);
}

TEST(JamFile, ParsesInt) {
	auto str = "{\"int\":12}";
	auto testStream = std::stringstream(str);
	auto res = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(res.has_value());
	ASSERT_FALSE(res.value().KeyValues.empty());

	auto kv = res.value().KeyValues["int"];
	int res2 = 0;
	std::visit([&res2](auto&& arg) {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, int>)
			res2 = arg;
	}, kv);

	ASSERT_EQ(12, res2);
}

TEST(JamFile, ParsesDouble) {
	auto str = "{\"double\":0.34}";
	auto testStream = std::stringstream(str);
	auto res = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(res.has_value());
	ASSERT_FALSE(res.value().KeyValues.empty());

	auto kv = res.value().KeyValues["double"];
	double res2 = 0;
	std::visit([&res2](auto&& arg) {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, double>)
			res2 = arg;
	}, kv);

	ASSERT_EQ(0.34, res2);
}

TEST(JamFile, ParsesString) {
	auto str = "{\"string\":\"hello\"}";
	auto testStream = std::stringstream(str);
	auto res = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(res.has_value());
	ASSERT_FALSE(res.value().KeyValues.empty());

	auto kv = res.value().KeyValues["string"];
	std::string res2 = "";
	std::visit([&res2](auto&& arg) {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, std::string>)
			res2 = arg;
	}, kv);

	ASSERT_EQ("hello", res2);
}

TEST(JamFile, ParsesStringArray) {
	auto str = "{\"arr\":[\"hello\",\"you\"]}";
	auto testStream = std::stringstream(str);
	auto res = JamFileMock::JsonFromStream(std::move(testStream));

	ASSERT_TRUE(res.has_value());
	ASSERT_FALSE(res.value().KeyValues.empty());

	auto kv = res.value().KeyValues["arr"];
	auto g = std::get<JamFile::JsonArray>(kv);
	auto res2 = std::get<std::vector<std::string>>(g.Array);

	ASSERT_EQ(2, res2.size());
	ASSERT_EQ("hello", res2[0]);
	ASSERT_EQ("you", res2[1]);
}
