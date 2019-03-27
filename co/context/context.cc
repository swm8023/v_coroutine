#include "context.h"

#include <co/common/logger.h>

using namespace v::co;

thread_local Context* v::co::g_cur_ctx = nullptr;

Context* Context::Create(StackAllocator *allocator, Code && code) {
	StackContext sctx = allocator->Alloc();

	// align
	const std::size_t bound = 64;
	std::size_t space = bound + sizeof(Context);
	void *sp = static_cast<char*>(sctx.sp) - space;
	sp = std::align(bound, sizeof(Context), sp, space);
	assert(sp != nullptr);

	const size_t stk_size = sctx.size - (static_cast<char*>(sctx.sp) - static_cast<char*>(sp));

	auto ctx = new(sp) Context();
	ctx->state_ = ContextState::CREATE;
	ctx->fctx_ = make_fcontext(sp, stk_size, &RunContext);
	ctx->from_fctx_ = nullptr;
	ctx->code_ = std::move(code);
	return ctx;
}

void Context::UpdatetFromTransferData(TransferData* data, fcontext_t fctx) {
	// A->B->C->A.resume(), A->B.yield()->A

	// B->A.resume()->A
	// 1. update A.from_ctx_
	// 2. update B.fctx_
	// B->A.resume()->A.yield()->B
	// 1. update A.fctx

	if (data->to) data->to->from_fctx_ = fctx;
	if (data->from) data->from->fctx_ = fctx;
}

void Context::RunContext(transfer_t t) {
	TransferData *tdata = static_cast<TransferData*>(t.data);
	UpdatetFromTransferData(tdata, t.fctx);

	Context *ctx = tdata->to;
	g_cur_ctx = ctx;
	ctx->state_ = ContextState::ACTIVE;

	if (ctx->code_) {
		ctx->code_();
	}

	ctx->state_ = ContextState::DONE;
	ctx->Yield();
}


void Context::Yield() {
	if (state_ != ContextState::ACTIVE && state_ != ContextState::DONE) {
		LogError("only active coroutine can be yield.");
		return;
	}
	state_ = ContextState::WAIT;
	TransferData tdata;
	tdata.from = this;
	tdata.to = nullptr;
	JumpTo(from_fctx_, tdata);
}

void Context::Resume() {
	if (state_ != ContextState::CREATE && state_ != ContextState::WAIT) {
		LogError("only wait coroutine can be resume.");
		return;
	}
	state_ = ContextState::ACTIVE;
	TransferData tdata;
	tdata.from = g_cur_ctx;
	tdata.to = this;
	JumpTo(fctx_, tdata);
}

void Context::JumpTo(fcontext_t &to, TransferData &tdata) {
	transfer_t ret = jump_fcontext(to, &tdata);
	TransferData *jump_data_from = static_cast<TransferData*>(ret.data);

	UpdatetFromTransferData(jump_data_from, ret.fctx);

	g_cur_ctx = jump_data_from->from;

}