#pragma once

#include <co/context/context.h>
#include <co/stack/stack_allocator_fixsized.h>

namespace v {
namespace co {


extern StackAllocatorFixSizedDefault default_allocator;
template<typename PullT>
class Coroutine {
public:
	Coroutine(Code && code, StackAllocator *allocator = &default_allocator) {
		ctx_ = Context::Create(allocator, std::forward<Code>(code));
	}

	bool operator()() {
		return ctx_->Resume();
	}

	PullT YieldValue() {
		void *p = ctx_->ArgAddr();
		if (p == nullptr) {
			return PullT();
		}
		return std::forward<PullT>(*static_cast<PullT*>(p));
	}

private:
	Context *ctx_{ nullptr };
};

template<typename PullT>
inline void CoYield(PullT t) {
	PullT cache_t = std::forward<PullT>(t);
	g_cur_ctx->Yield(static_cast<void*>(&cache_t));
}

inline void CoYield() {
	g_cur_ctx->Yield();
}


}
}