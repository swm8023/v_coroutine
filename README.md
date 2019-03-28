# v_coroutine

developing......

c++ coroutine support

## use
```c++
	auto co = Coroutine<int>([]() {
		for (int i = 0; i < 5; i++) {
			Log("generate", i);
			CoYield<int>(i);
		}
	});

	while (co()) {
		int v = co.YieldValue();
		Log("get result", v);
	}
```
output
```
generate 0
get result 0
generate 1
get result 1
generate 2
get result 2
generate 3
get result 3
generate 4
get result 4
```
