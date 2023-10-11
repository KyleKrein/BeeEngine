//
// Created by Александр Лебедев on 11.10.2023.
//

#include <gtest/gtest.h>
#include <BeeEngine.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "gtc/epsilon.hpp"
#include "gtx/matrix_decompose.inl"
using namespace BeeEngine;
TEST(TransformComponentTest, GetTransform)
{
    TransformComponent component;
    component.Translation = glm::vec3(1.0f, 2.0f, 3.0f);
    component.Rotation = glm::vec3(0.0f, glm::radians(90.0f), 0.0f);
    component.Scale = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::mat4 expected = glm::translate(glm::mat4(1.0f), component.Translation) *
                         glm::toMat4(glm::quat(component.Rotation)) *
                         glm::scale(glm::mat4(1.0f), component.Scale);

    EXPECT_EQ(component.GetTransform(), expected);
}

TEST(TransformComponentTest, SetTransform)
{
    glm::mat4 original = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f)) *
                         glm::toMat4(glm::quat(glm::vec3(0.0f, glm::radians(90.0f), 0.0f))) *
                         glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    TransformComponent component;
    component.SetTransform(original);
    auto transform = Math::DecomposeTransform(original);

    EXPECT_EQ(component.Translation, transform.Translation);
    EXPECT_EQ(component.Rotation, transform.Rotation);
    EXPECT_EQ(component.Scale, transform.Scale);
}

TEST(TransformComponentTest, ZeroValues)
{
    TransformComponent component;
    glm::mat4 expected(1.0f);
    EXPECT_EQ(component.GetTransform(), expected);
}

TEST(TransformComponentTest, NegativeValues)
{
    TransformComponent component;
    component.Translation = glm::vec3(-1.0f, -2.0f, -3.0f);
    component.Rotation = glm::vec3(0.0f, glm::radians(-90.0f), 0.0f);
    component.Scale = glm::vec3(-1.0f, -1.0f, -1.0f);

    glm::mat4 expected = glm::translate(glm::mat4(1.0f), component.Translation) *
                         glm::toMat4(glm::quat(component.Rotation)) *
                         glm::scale(glm::mat4(1.0f), component.Scale);

    EXPECT_EQ(component.GetTransform(), expected);
}

TEST(TransformComponentTest, NonUniformScale)
{
    TransformComponent component;
    component.Scale = glm::vec3(2.0f, 2.0f, 3.0f);

    glm::mat4 expected = glm::scale(glm::mat4(1.0f), component.Scale);
    component.SetTransform(expected);

    EXPECT_EQ(component.GetTransform(), expected);
}

TEST(TransformComponentTest, CombinedTransform)
{
    TransformComponent component;
    component.Translation = glm::vec3(1.0f, 1.0f, 1.0f);
    component.Rotation = glm::vec3(glm::radians(45.0f), glm::radians(45.0f), glm::radians(45.0f));
    component.Scale = glm::vec3(2.0f, 2.0f, 2.0f);

    glm::mat4 expected = glm::translate(glm::mat4(1.0f), component.Translation) *
                         glm::toMat4(glm::quat(component.Rotation)) *
                         glm::scale(glm::mat4(1.0f), component.Scale);

    EXPECT_EQ(component.GetTransform(), expected);
}

/*TEST(TransformComponentTest, IdempotentOperation)
{
    TransformComponent component;
    component.Translation = glm::vec3(3.0f, 2.0f, 1.0f);
    component.Rotation = glm::vec3(glm::radians(30.0f), glm::radians(40.0f), glm::radians(50.0f));
    component.Scale = glm::vec3(0.5f, 0.5f, 0.5f);

    glm::mat4 transform = component.GetTransform();
    TransformComponent newComponent;
    newComponent.SetTransform(transform);
    glm::mat4 newTransform = newComponent.GetTransform();

    EXPECT_EQ(transform, newTransform);
}*/

TEST(TransformComponentTest, ExtremeValues)
{
    TransformComponent component;
    component.Translation = glm::vec3(1e9f, 1e9f, 1e9f);
    component.Rotation = glm::vec3(glm::radians(180.0f), glm::radians(180.0f), glm::radians(180.0f));
    component.Scale = glm::vec3(1e-9f, 1e-9f, 1e-9f);

    glm::mat4 expected = glm::translate(glm::mat4(1.0f), component.Translation) *
                         glm::toMat4(glm::quat(component.Rotation)) *
                         glm::scale(glm::mat4(1.0f), component.Scale);

    EXPECT_EQ(component.GetTransform(), expected);
}

TEST(DecomposeTransformTest, IdentityMatrix) {
    glm::mat4 identity(1.0f);
    auto [translation, rotation, scale] = Math::DecomposeTransform(identity);
    EXPECT_EQ(translation, glm::vec3(0, 0, 0));
    EXPECT_EQ(rotation, glm::vec3(0, 0, 0));
    EXPECT_EQ(scale, glm::vec3(1, 1, 1));
}

TEST(DecomposeTransformTest, OnlyTranslation) {
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(1, 2, 3));
    auto [translation, rotation, scale] = Math::DecomposeTransform(translate);
    EXPECT_EQ(translation, glm::vec3(1, 2, 3));
    EXPECT_EQ(rotation, glm::vec3(0, 0, 0));
    EXPECT_EQ(scale, glm::vec3(1, 1, 1));
}

TEST(TransformComponentTest, IdempotentOperation) {
    TransformComponent originalComponent;
    originalComponent.Translation = glm::vec3(1.0f, 2.0f, 3.0f);
    originalComponent.Rotation = glm::vec3(glm::radians(30.0f), glm::radians(45.0f), 0);
    originalComponent.Scale = glm::vec3(1.0f, 2.0f, 1.0f);

    glm::mat4 originalMatrix = originalComponent.GetTransform();
    TransformComponent decomposedComponent;
    decomposedComponent.SetTransform(originalMatrix);

    glm::mat4 decomposedMatrix = decomposedComponent.GetTransform();

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_NEAR(originalMatrix[i][j], decomposedMatrix[i][j], 1e-5f);
        }
    }
}
