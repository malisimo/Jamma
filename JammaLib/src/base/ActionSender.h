#pragma once

#include <memory>

namespace base
{
	class ActionSender
	{
	public:
		void SetReceiver(std::shared_ptr<ActionReceiver> receiver) { _receiver = receiver; }
		const std::shared_ptr<ActionReceiver> GetReceiver() { return _receiver; }

	protected:
		std::shared_ptr<ActionReceiver> _receiver;
	};
}
