#pragma once
#include <optional>
#include <functional>

namespace Functional
{
	template<class T, class U>
	class Option
	{
	public:
		static std::optional<T> Return(T t)
		{
			return std::make_optional<T>(t);
		}

		static std::optional<U> Map(std::function<U(T)> fun, std::optional<T> t)
		{
			if (!t.has_value())
				return std::nullopt;

			return std::make_optional<U>(fun(t.value()));
		}

		static std::optional<U> Bind(std::function<std::optional<U>(T)> fun, std::optional<T> t)
		{
			if (!t.has_value())
				return std::nullopt;

			return fun(t.value());
		}
	};
}

