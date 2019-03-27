#pragma once

#include <cstdlib>

namespace v {
namespace co {


struct StackTraits {
	static bool is_unbounded();

	static std::size_t page_size();
	static std::size_t default_size();
	static std::size_t minimum_size();
	static std::size_t maximum_size();
};


struct StackContext {
    std::size_t size{ 0 };
    void *sp{ nullptr };
};


class StackAllocator {
public:
	virtual StackContext Alloc() = 0;
	virtual void Free(StackContext &sctx) = 0;
};


}
}