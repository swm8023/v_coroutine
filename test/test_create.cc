#include <functional>
#include <chrono>

#include <co/context/context.h>
#include <co/coroutine.h>

using namespace v::co;

int main(int argc, char *argv) {
	std::chrono::high_resolution_clock::time_point g_begin, g_end;
	g_begin = std::chrono::high_resolution_clock::now();

	const int TEST_N = 10000000;
	for (int i = 0; i < TEST_N; i++)
		RunCoroutine([]() {});

	g_end = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(g_end - g_begin).count();

	CoLog("switch speed times/sec ", TEST_N * 1.0 / dur * 1000);
	getchar();
}