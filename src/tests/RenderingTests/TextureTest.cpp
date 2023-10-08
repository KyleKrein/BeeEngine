//
// Created by alexl on 21.05.2023.
//

#include "Renderer/Texture.h"
#include "gtest/gtest.h"

/*TEST(OpenGLTexture2DTest, CreateTest)
{
    ASSERT_NO_THROW(BeeEngine::Texture2D::Create(1, 1));
    BeeEngine::Ref<BeeEngine::Texture2D> texture = BeeEngine::Texture2D::Create(1, 1);
    ASSERT_NE(texture, nullptr);
}*/

/* пока выключен по причине того, что я еще не разобрался с относительными путями
TEST(OpenGLTexture2DTest, CreateFromPathTest)
{
    ASSERT_NO_THROW(BeeEngine::Texture2D::Create("AssetsForTests\\chessTexture2D.jpg"));
    BeeEngine::Ref<BeeEngine::Texture2D> texture = BeeEngine::Texture2D::Create("AssetsForTests\\chessTexture2D.jpg");
    ASSERT_NE(texture, nullptr);
}*/