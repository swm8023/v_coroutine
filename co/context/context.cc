#include "context.h"

#include <co/common.h>

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
	ctx->allocator_ = allocator;
	ctx->sctx_ = sctx;
	ctx->code_ = std::move(code);
	return ctx;
}

void Context::Free() {
	allocator_->Free(sctx_);
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

	ctx->CoYield();
}


void Context::CoYield(void *arg_addr) {
	if (state_ != ContextState::ACTIVE && state_ != ContextState::DONE) {
		CoLog("only active coroutine can be yield.");
		return;
	}
	if (state_ == ContextState::ACTIVE) {
		state_ = ContextState::WAIT;
	} 
	TransferData tdata;
	tdata.from = this;
	tdata.to = nullptr;
	tdata.arg_addr = arg_addr;
	JumpTo(from_fctx_, tdata);
}

bool Context::CoResume() {
	if (state_ != ContextState::CREATE && state_ != ContextState::WAIT) {
		return false;
	}
	state_ = ContextState::ACTIVE;
	TransferData tdata;
	tdata.from = g_cur_ctx;
	tdata.to = this;
	JumpTo(fctx_, tdata);
	if (state_ == ContextState::DONE) {
		// already done, free context here
		Free();
		return false;
	}
	return true;
}

void Context::JumpTo(fcontext_t &to, TransferData &tdata) {
	transfer_t ret = jump_fcontext(to, &tdata);
	TransferData *tdata_from = static_cast<TransferData*>(ret.data);

	if (ret.fctx != nullptr) {
		UpdatetFromTransferData(tdata_from, ret.fctx);
		arg_addr_ = tdata_from->arg_addr;
	}

	g_cur_ctx = tdata.from;

}