//
// Created by Александр Лебедев on 06.06.2023.
//

#include "gtest/gtest.h"
#include "Utils/FileDialogs.h"

class TestFileDialogs : public ::testing::Test
{
protected:
    void SetUp() override
    {

    }
    std::string GetWindowsFilter(int index)
    {
        return m_Filter[index].WindowsFilter();
    }

    BeeEngine::FileDialogs::Filter m_Filter[2] = {
            {"All Files", "*.*"},
            {"BeeEngine Scene", "*.beescene"}
    };;
};
TEST_F(TestFileDialogs, TestWindowsFilter)
{
    std::string filter1 = std::move(GetWindowsFilter(0));
    bool filter1Equal = memcmp(filter1.c_str(), "All Files (*.*)\0*.*\0", 20) == 0;
    EXPECT_TRUE(filter1Equal);
    std::string filter2 = std::move(GetWindowsFilter(1));
    bool filter2Equal = memcmp(filter2.c_str(), "BeeEngine Scene (*.beescene)\0*.beescene\0", 40) == 0;
    EXPECT_TRUE(filter2Equal);
}
