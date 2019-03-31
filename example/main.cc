


#include <functional>
#include <co/context/context.h>

#include <co/coroutine.h>

using namespace v::co;

int main(int argc, char *argv) {

	auto co = Coroutine<int>([]() {
		for (int i = 0; i < 5; i++) {
			CoLog("generate", i);
			CoYield<int>(i);
		}
	});

	auto co2 = Coroutine<void>([&co]() {
		while (co()) {
			int v = co.YieldValue();
			CoLog("get result", v);
		}
	})();
	getchar();


	return 0;
}