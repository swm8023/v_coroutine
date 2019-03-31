#pragma once

#include <co/context/context.h>
#include <co/stack/stack_allocator_fixsized.h>

namespace v {
namespace co {


extern StackAllocatorFixSizedDefault default_allocator;

struct RunCoroutine {
	RunCoroutine(Code && code) {
		Context::Create(&default_allocator, std::forward<Code>(code))->CoResume();
	}
};

template<typename PullT>
class Coroutine : NonCopyable {
public:
	Coroutine(Code && code, StackAllocator *allocator = &default_allocator) {
		ctx_ = Context::Create(allocator, std::forward<Code>(code));
	}

	Coroutine(Coroutine && other) {
		ctx_ = other.ctx_;
		other.ctx_ = nullptr;
	}

	Coroutine& operator = (Coroutine && other) {
		if (this = &other) return *this;
		this->ctx_ = other.ctx_;
		other.ctx_ = nullptr;
		return *this;
	}

	~Coroutine() {
		// if ctx != nullptr, this running coroutine will never be called
		assert(ctx_ == nullptr);
	}


	bool operator()() {
		bool ret = ctx_->CoResume();
		if (ret == false) {
			ctx_ = nullptr;
		}
		return ret;
	}

	PullT YieldValue() {
		if (ctx_ == nullptr) {
			return PullT();
		}
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
	g_cur_ctx->CoYield(static_cast<void*>(&cache_t));
}

inline void CoYield() {
	g_cur_ctx->CoYield();
}


}
}