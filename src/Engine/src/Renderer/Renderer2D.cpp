//
// Created by alexl on 14.05.2023.
//

#include "Renderer2D.h"
#include "Renderer.h"
#include "Debug/DebugUtils.h"
#include "ext/matrix_transform.hpp"

namespace BeeEngine
{
    Renderer2DAPI *Renderer2D::m_API = nullptr;
    Ref<Renderer2D::Statistics> Renderer2D::m_Statistics = CreateRef<Statistics>();

    void Renderer2D::Init(Renderer2DAPI *api)
    {
        BEE_PROFILE_FUNCTION();
        m_API = api;
        m_API->Init();
    }
    void Renderer2D::DrawRectangle(float x, float y, float z, float width, float height, const Color4 &color, float rotation)
    {
        BEE_PROFILE_FUNCTION();
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), {x, y, z}) * glm::rotate(glm::mat4(1.0f), rotation, {0, 0, 1}) *
                              glm::scale(glm::mat4(1.0f), {width, height, 1.0f});
        m_API->DrawRectangle(transform, color);
    }

    void Renderer2D::DrawImage(float x, float y, float z, float width, float height, const Ref<Texture2D> &texture,
                               float rotation, const Color4 &color, float textureMultiplier)
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(texture != nullptr && texture->GetRendererID() != 0);
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), {x, y, z}) * glm::rotate(glm::mat4(1.0f), rotation, {0, 0, 1}) *
                              glm::scale(glm::mat4(1.0f), {width, height, 1.0f});
        DrawImage(transform, texture, color, textureMultiplier);
    }

    void Renderer2D::ResetStatistics()
    {
        m_Statistics->DrawCalls = 0;
        m_Statistics->QuadCount = 0;
        m_Statistics->SpriteCount = 0;
    }

    void Renderer2D::BeginScene(const ICamera &camera)
    {
        BEE_PROFILE_FUNCTION();
        m_API->SetCameraTransform(camera.GetViewProjectionMatrix());
        m_API->BeginScene();
    }

    void Renderer2D::EndScene()
    {
        BEE_PROFILE_FUNCTION();
        m_API->EndScene();
    }

    void Renderer2D::DrawRectangle(const RectangleProperties &properties)
    {
        Renderer2D::DrawRectangle(properties.X, properties.Y, properties.Z, properties.Width, properties.Height, properties.Color,
                      properties.Rotation);
    }

    void Renderer2D::DrawRectangle(float x, float y, float z, float width, float height, const Color4 &color)
    {
        Renderer2D::DrawRectangle(x, y, z, width, height, color, 0);
    }

    void Renderer2D::DrawRectangle(const glm::vec2 &position, const glm::vec2 &size, const Color4 &color)
    {
        Renderer2D::DrawRectangle(position.x, position.y, 0, size.x, size.y, color, 0);
    }

    void Renderer2D::DrawImage(float x, float y, float width, float height, const Ref<Texture2D> &texture)
    {
        Renderer2D::DrawImage(x, y, 0, width, height, texture, 0, Color4::White, 1);
    }

    void Renderer2D::DrawImage(float x, float y, float z, float width, float height, const Ref<Texture2D> &texture)
    {
        Renderer2D::DrawImage(x, y, z, width, height, texture, 0, Color4::White, 1);
    }

    void Renderer2D::DrawImage(float x, float y, float z, float width, float height, const Ref<Texture2D> &texture,
                               float rotation)
    {
        Renderer2D::DrawImage(x, y, z, width, height, texture, rotation, Color4::White, 1);
    }

    void Renderer2D::DrawImage(float x, float y, float z, float width, float height, const Ref<Texture2D> &texture,
                               float rotation, const Color4 &color)
    {
        Renderer2D::DrawImage(x, y, z, width, height, texture, rotation, color, 1);
    }

    void Renderer2D::Shutdown()
    {
        delete m_API;
    }

    void Renderer2D::DrawImage(const glm::mat4& transform, const Ref<Texture2D> &texture, const Color4 &color,
                               float textureMultiplier)
    {
        m_API->DrawImage(transform, texture, color, textureMultiplier);
    }

    void Renderer2D::BeginScene(const Camera &camera, const glm::mat4 &transform)
    {
        m_API->SetCameraTransform(camera.GetProjectionMatrix() * glm::inverse(transform));
        m_API->BeginScene();
    }
}
