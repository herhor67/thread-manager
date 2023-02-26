# thread-manager
C++ thread manager, for calling a function in separate threads, with arguments provided in a queue, with limit of concurrent threads

Example:
```C++
#include <iostream>
#include "thread_manager.h"

using namespace std;

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

	tm.run(2);
}
```

Possible output:
```1 2 1 1 2 1 2 1 1 2 1 1 2 1 1 2 3 2 2 3 2 3 2 4 3 3 4 3 4 3 4 3 3 4 3 4 4 4 4 4```

Additional function arguments should be provided in `push` as an argument pack, like so:
```C++
push(arg1, arg2, arg3, ...)
```
