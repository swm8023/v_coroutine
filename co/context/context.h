#pragma once

#include <memory>
#include <functional>
#include <cassert>

#include <co/common/noncopyable.h>
#include <co/stack/stack_traits.h>
#include "fcontext.h"

namespace v {
namespace co {

class Context;

struct TransferData {
	Context *from{ nullptr };
	Context *to{ nullptr };
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

	void Yield();
	void Resume();

private:
	void JumpTo(fcontext_t &to, TransferData &tdata);
	
private:
	fcontext_t fctx_{ nullptr };
	fcontext_t from_fctx_{ nullptr };
	Code code_{ nullptr };
	ContextState state_;
};



}
}