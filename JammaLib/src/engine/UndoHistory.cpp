#include "UndoHistory.h"

using namespace engine;
using base::ActionUndo;

UndoHistory::UndoHistory()
{
}

UndoHistory::~UndoHistory()
{
}

void engine::UndoHistory::Add(std::shared_ptr<ActionUndo> actionUndo)
{
	while (!_poppedHistory.empty())
		_poppedHistory.pop();

	_history.push(actionUndo);
}

void engine::UndoHistory::Clear()
{
	while (!_poppedHistory.empty())
		_poppedHistory.pop();

	while (!_history.empty())
		_history.pop();
}

bool engine::UndoHistory::Undo()
{
	auto lastOpt = Pop();

	if (!lastOpt.has_value())
		return false;

	auto last = lastOpt.value();

	if (last)
		return last->Undo();

	return false;
}

bool engine::UndoHistory::Redo()
{
	auto nextOpt = UnPop();

	if (!nextOpt.has_value())
		return false;

	auto next = nextOpt.value();

	if (next)
		return next->Redo();

	return false;
}

std::optional<std::shared_ptr<ActionUndo>> engine::UndoHistory::Pop()
{
	if (_history.empty())
		return std::nullopt;

	_poppedHistory.push(_history.front());
	_history.pop();

	return _poppedHistory.front();
}

std::optional<std::shared_ptr<ActionUndo>> engine::UndoHistory::UnPop()
{
	if (_poppedHistory.empty())
		return std::nullopt;

	_history.push(_poppedHistory.front());
	_poppedHistory.pop();

	return _history.front();
}
