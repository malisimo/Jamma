#pragma once

#include <optional>
#include <memory>
#include "../utils/CommonTypes.h"
#include "../engine/Timer.h"
#include "../io/UserConfig.h"

namespace base
{
	class ActionReceiver;

	class Action
	{
	public:
		Action() :
			_actionTime(std::chrono::steady_clock::now()),
			_userConfig(std::nullopt)
		{};

		~Action() {};

	public:
		Time GetActionTime() const { return _actionTime; }
		void SetActionTime(Time time) { _actionTime = time; }

		std::optional<io::UserConfig> GetUserConfig() const { return _userConfig; }
		void SetUserConfig(io::UserConfig cfg) { _userConfig = cfg; }

	protected:
		Time _actionTime;
		std::optional<io::UserConfig> _userConfig;
	};
}
