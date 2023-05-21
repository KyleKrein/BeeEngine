//
// Created by alexl on 22.05.2023.
//
#include "BeeEngine.h"
#include "gtest/gtest.h"

TEST(RendererTest, ColorAndFillScreenTest)
{
    BeeEngine::Renderer::SetClearColor(BeeEngine::Color4::CornflowerBlue);
    BeeEngine::Renderer::Clear();
    ASSERT_EQ(BeeEngine::Renderer::ReadPixel(0, 0), BeeEngine::Color4::CornflowerBlue);
}