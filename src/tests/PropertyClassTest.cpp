//
// Created by alexl on 05.06.2023.
//
#include "gtest/gtest.h"
#include "Property.h"

TEST(Property, Constructor)
{
    int testValue = 11;
    BeeEngine::Property<int> property
            (
                    testValue,
                    [&property]() -> const auto& { return property.value(); },
                    [&property](const int& value)->void { property.value() = value; }
            );
    EXPECT_EQ(property.Get(), 11);

    property = 12;
    EXPECT_EQ(property.Get(), 12);

    auto suck = property.Get();
    EXPECT_EQ(suck, 12);
/*
    BeeEngine::Property<int> property2
            (
                    [&property]() -> const auto& { return property.value(); }
            );*/
}