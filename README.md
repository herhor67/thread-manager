# thread-manager
C++ thread manager, for calling a function in separate threads, with a limit of concurrent threads.
Function arguments are pushed to a queue.
Such code is guaranteed to finish in the shortest time possible, provided the arguments in the queue are ordered by execution time, descending.


Example:
```C++
#include <iostream>
#include <chrono>
#include "thread_manager.h"

using namespace std;
using namespace std::chrono;

void func(int arg)
{
	for (int i = 0; i < 10; ++i)
	{
		cout << arg << ' ';
		this_thread::sleep_for(arg * 100ms);
	}
}

int main()
{
	thread_manager tm(func);

	tm.push(1);
	tm.push(2);
	tm.push(3);
	tm.push(4);

	auto start = high_resolution_clock::now();
	tm.run(2);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	cout << endl << duration.count() << "ms" << endl;
}
```

Possible output:
```
2 1 1 2 1 1 2 1 1 2 1 1 2 1 1 2 3 2 3 2 2 3 2 3 4 3 4 3 3 4 3 4 3 4 3 4 4 4 4 4
6406ms
```


Slowest to fastest:
```C++
	[...]
	tm.push(4);
	tm.push(3);
	tm.push(2);
	tm.push(1);
	[...]
}
```

Possible output:
```
3 4 3 4 3 4 3 4 3 3 4 3 4 3 4 3 3 4 2 4 2 2 4 2 2 1 2 1 1 2 1 1 2 1 1 2 1 1 2 1
5443ms
```


Additional function arguments should be provided in `push` as an argument pack, like so:
```C++
push(arg1, arg2, arg3, ...)
```
