#pragma once

#include <memory>
#include <functional>
#include <cassert>

#include <co/common.h>
#include <co/stack/stack_traits.h>
#include "fcontext.h"

namespace v {
namespace co {

class Context;

struct TransferData {
	Context *from{ nullptr };
	Context *to{ nullptr };
	void *arg_addr{ nullptr };
};

enum ContextState {
	CREATE = 0,
	ACTIVE,
	WAIT,
	DONE,
	EXIT,
};

// save current context
extern thread_local Context* g_cur_ctx;

typedef std::function<void()> Code;

class Context : NonCopyable {
public:
	static Context* Create(StackAllocator *allocator, Code && code);
	static void UpdatetFromTransferData(TransferData* data, fcontext_t fctx);
	static void RunContext(transfer_t t);

	void Yield(void* arg_addr = nullptr);
	bool Resume();

	void* ArgAddr() {
		return arg_addr_;
	}

private:
	void JumpTo(fcontext_t &to, TransferData &tdata);
	
private:
	fcontext_t fctx_{ nullptr };
	fcontext_t from_fctx_{ nullptr };
	Code code_{ nullptr };
	void *arg_addr_{ nullptr };
	ContextState state_;
};

namespace this_co {
inline Context *Get() {
	return g_cur_ctx;
}

inline bool InCoroutine() {
	return g_cur_ctx != nullptr;
}
}



}
}