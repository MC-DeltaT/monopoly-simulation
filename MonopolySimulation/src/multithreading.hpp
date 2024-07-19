#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <thread>
#include <vector>


namespace monopoly {

	// Runs func on thread_count threads and reduces the results.
	template<typename Result, typename Reducer = std::plus<Result>>
	Result map_multithreaded(auto func, unsigned const thread_count, Reducer reducer = {}) {
		assert(thread_count >= 1);
		std::vector<std::thread> threads;
		threads.reserve(thread_count - 1);
		std::vector<Result> results(thread_count);

		for (unsigned i = 1; i < thread_count; ++i) {
			threads.emplace_back(func, std::ref(results[i]));
		}

		func(results.front());

		for (auto& thread : threads) {
			thread.join();
		}

		return std::reduce(results.cbegin(), results.cend(), Result{}, reducer);
	}

}
