#pragma once

#include <algorithm>
#include <functional>
#include <mutex>
#include <thread>

#include <list>
#include <queue>
#include <unordered_set>
#include <tuple>



template<class Ret, class... Args>
class thread_manager
{
public:
	using ArgTpl = std::tuple<Args...>;

	using Function = std::function<Ret(Args...)>;
	typedef Ret(*FunctionPtr)(Args...);

private:
	const Function callback;
	std::queue<ArgTpl, std::list<ArgTpl>> queue; // underlaying storage as 2nd arg

public:
	thread_manager(Function f) : callback(f) {}
	thread_manager(FunctionPtr fp) : callback(static_cast<Function>(fp)) {}
	~thread_manager() = default;


	void push(const Args&... args)
	{
		//queue.push(std::forward_as_tuple(args...));
		queue.emplace(args...);
	}
	void push(const ArgTpl& args)
	{
		queue.push(args);
	}


	void run(size_t maxthrdcnt, const std::chrono::nanoseconds& slpdrtn = 100ms)
	{
		std::list<std::thread> callers;

		std::unordered_set<std::thread::id> ready_ids;
		std::mutex ready_mutex;

		DEBUGECHO;

		while (queue.size() || callers.size()) // while there are threads to do
		{

			// join finished threads
			if (!ready_ids.empty())
			{
				std::unique_lock<std::mutex> lock(ready_mutex);

				for (const std::thread::id& thrdid : ready_ids)
				{
					auto ptr = std::find_if(std::begin(callers), std::end(callers),
						[&thrdid](const std::thread& th)
						{
							return th.get_id() == thrdid;
						}
					);
					//if (it != std::end(callers))
					ptr->join();
					callers.erase(ptr);
				}

				ready_ids.clear();
				lock.unlock();
			}

			// run new threads if available
			while (queue.size() && callers.size() < maxthrdcnt)
			{
				callers.emplace_back( // create in-place new thread from lambda, which calls callback with args and calls finisher
					[this, &ready_ids, &ready_mutex](ArgTpl argin)
					{
						std::apply(callback, argin);

						std::unique_lock<std::mutex> lock(ready_mutex);
						ready_ids.insert(std::this_thread::get_id());
					},
					queue.front()
				);

				queue.pop();
			}

			std::this_thread::sleep_for(slpdrtn);
		}
	}

};
