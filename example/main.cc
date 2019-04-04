


#include <functional>
#include <co/context/context.h>

#include <co/coroutine.h>

using namespace v::co;

int main(int argc, char *argv) {

	auto co = Coroutine<int>([]() {
		for (int i = 0; i < 1000; i++) {
			CoLog("generate", i);
			int *ret = CoYield<int, int>(i);
			if (*ret == 0) {
				break;
			}
		}
	});

	int co_param = 0;
	while (co(co_param)) {
		int v = co.YieldValue();
		if (v == 5) {
			co_param = 0;
		} else {
			co_param = 1;
		}
	}

	getchar();



	return 0;
}