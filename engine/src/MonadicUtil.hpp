#pragma once

#include <optional>

template <typename T, typename Func>
auto fmap(const std::optional<T>& opt, Func&& func) -> std::optional<decltype(func(*opt))> {
	return opt ? func(*opt) : std::nullopt;
}
template <typename T, typename Func>
auto operator|(const std::optional<T>& opt, Func&& func) -> std::optional<decltype(func(*opt))> {
	return fmap(opt, std::forward<Func>(func));
}

template <typename T, typename U, typename Func>
auto fmap2(
		const std::optional<T>& optT,
		const std::optional<U>& optU,
		Func&& func
) -> std::optional<decltype(func(*optT, *optU))> {
	return (optT && optU) ? func(*optT, *optU) : std::nullopt;
}

template <typename T, typename Func>
auto bind(const std::optional<T>& opt, Func&& func) -> decltype(func(*opt)) {
	return opt ? func(*opt) : std::nullopt;
}
template <typename T, typename Func>
auto operator>>(const std::optional<T>& opt, Func&& func) -> decltype(func(*opt)) {
	return bind(opt, std::forward<Func>(func));
}

template <typename T, typename U, typename Func>
auto bind2(
		const std::optional<T>& optT,
		const std::optional<U>& optU,
		Func&& func
) -> decltype(func(*optT, *optU)) {
	return (optT && optU) ? func(*optT, *optU) : std::nullopt;
}
