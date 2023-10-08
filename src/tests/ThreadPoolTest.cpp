//
// Created by alexl on 07.10.2023.
//

#include "gtest/gtest.h"
#include "BeeEngine.h"
#include "Threading/ThreadPool.h"
using namespace BeeEngine;

TEST(ThreadPoolTest, CreateThreadPool) {
    ThreadPool pool(4);
    EXPECT_EQ(pool.QueueSize(), 0);
}

TEST(ThreadPoolTest, AddTask) {
    ThreadPool pool(2);

    auto future1 = pool.AddTask([]() -> int { return 1 + 1; });
    auto future2 = pool.AddTask([]() -> int { return 2 + 2; });

    EXPECT_EQ(future1.get(), 2);
    EXPECT_EQ(future2.get(), 4);
}

TEST(ThreadPoolTest, Shutdown) {
    ThreadPool pool(2);

    auto future1 = pool.AddTask([]() -> int { return 1 + 1; });
    auto future2 = pool.AddTask([]() -> int { return 2 + 2; });

    pool.Shutdown();

    EXPECT_EQ(future1.get(), 2);
    EXPECT_EQ(future2.get(), 4);

    // Здесь нужно как-то проверить, что все потоки действительно завершены,
    // но это зависит от вашей реализации.
}

TEST(ThreadPoolTest, MultipleTasks) {
    ThreadPool pool(4);
    std::vector<std::future<int>> futures;

    for (int i = 0; i < 10; ++i) {
        futures.push_back(pool.AddTask([i]() -> int { return i * i; }));
    }

    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(futures[i].get(), i * i);
    }
}