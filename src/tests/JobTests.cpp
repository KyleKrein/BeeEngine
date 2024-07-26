#include <JobSystem/JobScheduler.h> // Include the Jobs module
#include <JobSystem/SpinLock.h>
#include <gtest/gtest.h>

TEST(JobCompletionTest, WaitForJobsToComplete)
{
    BeeEngine::Jobs::Counter counter;
    int result1 = 0, result2 = 0;

    auto job1 = BeeEngine::Jobs::CreateJob(counter, [&result1](int a, int b) { result1 = a + b; }, 2, 3);
    auto job2 = BeeEngine::Jobs::CreateJob(counter, [&result2](int a, int b) { result2 = a + b; }, 4, 6);

    BeeEngine::Jobs::Schedule(std::move(job1));
    BeeEngine::Jobs::Schedule(std::move(job2));

    // Simulate job execution

    BeeEngine::Jobs::WaitForJobsToComplete(counter);

    EXPECT_EQ(result1, 5);
    EXPECT_EQ(result2, 10);
}

// Test fixture for Jobs::this_job::SleepFor function
class JobSleepForTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Set up any necessary resources or state before each test
    }

    void TearDown() override
    {
        // Clean up any resources or state after each test
    }
};

// Test case 1: Job sleeps for a time less than the maximum limit
TEST_F(JobSleepForTest, SleepForLessThanMaxLimit)
{
    using namespace std::chrono_literals;

    // Create a counter for tracking job completion
    BeeEngine::Jobs::Counter counter;

    // Create a job that sleeps for a time less than the maximum limit
    auto job = BeeEngine::Jobs::CreateJob(counter, []() { BeeEngine::Jobs::this_job::SleepFor(100ms); });

    // Schedule the job
    BeeEngine::Jobs::Schedule(std::move(job));

    // Wait for the job to complete
    BeeEngine::Jobs::WaitForJobsToComplete(counter);

    // Verify that the job completed successfully
    EXPECT_TRUE(counter.IsZero());
}

// Test case 2: Job sleeps for zero time
TEST_F(JobSleepForTest, SleepForZeroTime)
{
    using namespace std::chrono_literals;
    // Create a counter for tracking job completion
    BeeEngine::Jobs::Counter counter;

    // Create a job that sleeps for zero time
    auto job = BeeEngine::Jobs::CreateJob(counter, []() { BeeEngine::Jobs::this_job::SleepFor(0ms); });

    // Schedule the job
    BeeEngine::Jobs::Schedule(std::move(job));

    // Wait for the job to complete
    BeeEngine::Jobs::WaitForJobsToComplete(counter);

    // Verify that the job completed successfully
    EXPECT_TRUE(counter.IsZero());
}

// Test case 3: Job sleeps for a negative time
TEST_F(JobSleepForTest, SleepForNegativeTime)
{
    using namespace std::chrono_literals;

    // Create a counter for tracking job completion
    BeeEngine::Jobs::Counter counter;

    // Create a job that sleeps for a negative time
    auto job = BeeEngine::Jobs::CreateJob(counter, []() { BeeEngine::Jobs::this_job::SleepFor(-100ms); });

    // Schedule the job
    BeeEngine::Jobs::Schedule(std::move(job));

    // Wait for the job to complete
    BeeEngine::Jobs::WaitForJobsToComplete(counter);

    EXPECT_TRUE(counter.IsZero());
}