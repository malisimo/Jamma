
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
	ASSERT_TRUE(res);
}
