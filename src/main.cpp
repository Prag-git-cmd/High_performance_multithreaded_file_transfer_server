#include <iostream>
#include <thread>
#include <chrono>

#include "ThreadPool.h"

void performTask(int taskId)
{
    std::cout << "Task " << taskId
              << " started on thread "
              << std::this_thread::get_id()
              << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Task " << taskId
              << " completed"
              << std::endl;
}

int main()
{
    ThreadPool pool(3);

    for (int i = 1; i <= 10; ++i)
    {
        pool.enqueue([i]()
        {
            performTask(i);
        });
    }

    std::cout << "All tasks submitted." << std::endl;

    return 0;
}
