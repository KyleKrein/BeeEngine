//
// Created by alexl on 09.06.2023.
//
#if defined(BEE_COMPILE_VULKAN)
#pragma once

#include "Renderer/Instance.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "vulkan/vulkan.hpp"

namespace BeeEngine::Internal
{
    class VulkanInstance : public Instance
    {
    public:
        VulkanInstance(std::string_view windowTitle, WindowHandlerAPI windowApi);
        ~VulkanInstance() override;

        [[nodiscard]] const vk::Instance& GetHandle() const { return m_Instance; }

    private:
        vk::Instance m_Instance{nullptr};
        vk::DebugUtilsMessengerEXT m_DebugMessenger{nullptr};
        vk::DispatchLoaderDynamic m_DynamicLoader{vk::DispatchLoaderDynamic(vkGetInstanceProcAddr)};
        WindowHandlerAPI m_WindowApi;

        bool ExtensionsSupported(const std::vector<const char*>& extensions) const;
        bool LayersSupported(const std::vector<const char*>& layers) const;

        void MakeDebugMessenger();

        void ManageInstance(vk::ApplicationInfo& info);
    };
} // namespace BeeEngine::Internal
#endif