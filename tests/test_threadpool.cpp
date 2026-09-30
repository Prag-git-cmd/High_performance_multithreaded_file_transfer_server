#include <iostream>
#include <atomic>
#include <functional>
#include "ThreadPool.h"

int main()
{
    constexpr int NUM_TASKS = 100;
    std::atomic<int> completedTasks{0};

    {
        ThreadPool pool(4);

        for (int i = 0; i < NUM_TASKS; ++i)
        {
            pool.enqueue([&completedTasks]()
            {
                completedTasks.fetch_add(1);
            });
        }
    }

    // ThreadPool destructor waits for all worker threads,
    // so all queued tasks should be completed here.
    if (completedTasks == NUM_TASKS)
    {
        std::cout << "ThreadPool test PASSED\n";
        std::cout << "Completed tasks: "
                  << completedTasks << "/" << NUM_TASKS << "\n";
        return 0;
    }

    std::cerr << "ThreadPool test FAILED\n";
    std::cerr << "Completed tasks: "
              << completedTasks << "/" << NUM_TASKS << "\n";

    return 1;
}
