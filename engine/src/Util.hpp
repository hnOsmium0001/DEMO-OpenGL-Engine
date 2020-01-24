#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <optional>

namespace HOEngine {

class Dimension {
public:
	int32_t width;
	int32_t height;
};

std::ostream& operator<<(std::ostream& strm, const Dimension& dim);

template <class F>
class ScopeGuard {
private:
	F function_;

public:
	ScopeGuard() = delete;
	explicit ScopeGuard(F&& function) noexcept
		: function_{ std::move(function) } {
	}
	ScopeGuard(const ScopeGuard&) = delete;
	ScopeGuard& operator=(const ScopeGuard&) = delete;
	ScopeGuard(ScopeGuard&&) = delete;
	ScopeGuard& operator=(ScopeGuard&&) = delete;
	~ScopeGuard() noexcept {
		function_();
	}
};

std::optional<std::string> ReadFileAsStr(const std::string& path);

} // namespace HOEngine
