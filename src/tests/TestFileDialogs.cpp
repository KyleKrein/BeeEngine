//
// Created by Александр Лебедев on 06.06.2023.
//

#include "Core/String.h"
#include "Utils/FileDialogs.h"
#include "gtest/gtest.h"

class TestFileDialogs : public ::testing::Test
{
protected:
    void SetUp() override {}
#if defined(WINDOWS)
    BeeEngine::String GetWindowsFilter(int index) { return m_Filter[index].WindowsFilter(); }
#endif

    BeeEngine::FileDialogs::Filter m_Filter[2] = {{"All Files", "*.*"}, {"BeeEngine Scene", "*.beescene"}};
    ;
};
#if defined(WINDOWS)
TEST_F(TestFileDialogs, TestWindowsFilter)
{
    BeeEngine::String filter1 = std::move(GetWindowsFilter(0));
    bool filter1Equal = memcmp(filter1.c_str(), "All Files (*.*)\0*.*\0", 20) == 0;
    EXPECT_TRUE(filter1Equal);
    BeeEngine::String filter2 = std::move(GetWindowsFilter(1));
    bool filter2Equal = memcmp(filter2.c_str(), "BeeEngine Scene (*.beescene)\0*.beescene\0", 40) == 0;
    EXPECT_TRUE(filter2Equal);
}
#endif