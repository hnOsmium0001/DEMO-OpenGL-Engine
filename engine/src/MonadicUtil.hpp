#pragma once

#include <optional>
#include <functional>

namespace HOEngine {

template <typename T, typename Func>
auto operator|(const std::optional<T>& opt, Func&& func) -> std::optional<decltype(func(*opt))> {
	return opt ? func(*opt) : std::nullopt;
}
template <typename Func, typename... Ts>
auto Fmap(Func&& func, const std::optional<Ts>&... opts) -> std::optional<decltype(func((*opts)...))> {
	return (... && opts) ? func((*opts)...) : std::nullopt;
}

template <typename T, typename Func>
auto operator>>(const std::optional<T>& opt, Func&& func) -> decltype(func(*opt)) {
	return opt ? func(*opt) : std::nullopt;
}
template <typename Func, typename... Ts>
auto Bind(Func&& func, const std::optional<Ts>&... opts) -> decltype(func((*opts)...)) {
	return (... && opts) ? func((*opts)...) : std::nullopt;
}

} // namespace HOEngine
