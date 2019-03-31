//          Copyright Oliver Kowalke 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "stack_traits.h"

extern "C" {
#include <signal.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
}

//#if _POSIX_C_SOURCE >= 200112L

#include <algorithm>
#include <cmath>

#include <mutex>

#if !defined (SIGSTKSZ)
# define SIGSTKSZ (32768) // 32kb minimum allowable stack
# define UDEF_SIGSTKSZ
#endif

#if !defined (MINSIGSTKSZ)
# define MINSIGSTKSZ (131072) // 128kb recommended stack size
# define UDEF_MINSIGSTKSZ
#endif

namespace {

void pagesize_(std::size_t * size) {
	// conform to POSIX.1-2001
	*size = ::sysconf(_SC_PAGESIZE);
}

void stacksize_limit_(rlimit * limit) {
	// conforming to POSIX.1-2001
	::getrlimit(RLIMIT_STACK, limit);
}

std::size_t pagesize() {
	static std::size_t size = 0;
	static std::once_flag flag;
	std::call_once(flag, pagesize_, &size);
	return size;
}

rlimit stacksize_limit() {
	static rlimit limit;
	static std::once_flag flag;
	std::call_once(flag, stacksize_limit_, &limit);
	return limit;
}

}

namespace v {
namespace co {

bool StackTraits::is_unbounded() {
	return RLIM_INFINITY == stacksize_limit().rlim_max;
}

std::size_t StackTraits::page_size() {
	return pagesize();
}

std::size_t StackTraits::default_size() {
	return 128 * 1024;
}

std::size_t StackTraits::minimum_size() {
	return MINSIGSTKSZ;
}

std::size_t StackTraits::maximum_size() {
	return static_cast<std::size_t>(stacksize_limit().rlim_max);
}

}
}


#ifdef UDEF_SIGSTKSZ
# undef SIGSTKSZ;
#endif

#ifdef UDEF_MINSIGSTKSZ
# undef MINSIGSTKSZ
#endif
