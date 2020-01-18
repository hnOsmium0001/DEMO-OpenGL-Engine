#include "Aliases.h"

namespace HOEngine {

class Dimension {
public:
	i32 width;
	i32 height;
};

template <class>
class FnPtr;

template <class Return, class... Args>
class FnPtr<auto(Args...)->Return> {
public:
	Return(*const ptr)(Args...);

	FnPtr(Return(*_ptr)(Args...)) : ptr{ _ptr } {
	}

	Return operator()(Args... args) const {
		return this->ptr(args);
	}
};

}
