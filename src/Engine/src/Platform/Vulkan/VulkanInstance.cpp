//
// Created by alexl on 09.06.2023.
//

#include "VulkanInstance.h"
#include "Core/Logging/Log.h"


namespace BeeEngine::Internal
{

    VulkanInstance::VulkanInstance(std::string_view windowTitle, WindowHandlerAPI windowApi)
    : m_WindowApi(windowApi)
    {
        /*
         * An instance stores all per-application state info, it is a vulkan handle
         * (An opaque integer or pointer type used by an API to refer to an object internally)
         */

        uint32_t version = 0;
        vkEnumerateInstanceVersion(&version);
        BeeCoreInfo("System supports Vulkan Variant {0}. Major: {1}, Minor {2}, Patch {3}",
                    VK_API_VERSION_VARIANT(version), VK_API_VERSION_MAJOR(version),
                    VK_API_VERSION_MINOR(version), VK_API_VERSION_PATCH(version));

        version &= ~(0xFFFU);
        version = VK_MAKE_API_VERSION(0, 1, 2, 0);

        /*
         * from vulkan_structs.hpp:
         *
         * VULKAN_HPP_CONSTEXPR ApplicationInfo( const char* pApplicationName_ = {},
         *                                      uint32_t applicationVersion_ = {},
         *                                      const char* pEngineName_ = {},
         *                                      uint32_t engineVersion_ = {},
         *                                      uint32_t apiVersion_ = {} ) VULKAN_HPP_NOEXCEPT
         */
        vk::ApplicationInfo appInfo(windowTitle.data(),
                                    1,
                                    "BeeEngine",
                                    1,
                                    version);
        switch (windowApi)
        {
            case WindowHandlerAPI::GLFW:
            {
                ManageGLFW(appInfo);
                break;
            }
            case WindowHandlerAPI::SDL:
            BeeCoreFatalError("SDL is not supported yet!");
        }
        m_DynamicLoader = vk::DispatchLoaderDynamic(m_Instance, vkGetInstanceProcAddr);
#if defined(DEBUG)
        MakeDebugMessenger();
#endif
    }

    VulkanInstance::~VulkanInstance()
    {
#if defined(DEBUG)
        m_Instance.destroyDebugUtilsMessengerEXT(m_DebugMessenger, nullptr, m_DynamicLoader);
#endif
        m_Instance.destroy();
    }

    bool VulkanInstance::ExtensionsSupported(const std::vector<const char *> &extensions) const
    {
        std::vector<vk::ExtensionProperties> supportedExtensions = vk::enumerateInstanceExtensionProperties();

        BeeCoreTrace("Supported Extensions: ");
        for (const vk::ExtensionProperties& extension : supportedExtensions)
        {
            BeeCoreTrace("\t{0}", extension.extensionName);
        }

        for(const char* extension : extensions)
        {
            bool found = false;
            for(const vk::ExtensionProperties& supportedExtension : supportedExtensions)
            {
                if(strcmp(extension, supportedExtension.extensionName) == 0)
                {
                    found = true;
                    BeeCoreTrace("Extension {0} is supported", extension);
                    break;
                }
            }
            if(!found)
            {
                BeeCoreError("Extension {0} is not supported", extension);
                return false;
            }
        }
        return true;
    }

    bool VulkanInstance::LayersSupported(const std::vector<const char *> &layers) const
    {
        std::vector<vk::LayerProperties> supportedLayers = vk::enumerateInstanceLayerProperties();
        BeeCoreTrace("Supported Layers: ");
        for (const vk::LayerProperties& layer : supportedLayers)
        {
            BeeCoreTrace("\t{0}", layer.layerName);
        }

        for(const char* layer: layers)
        {
            bool found = false;
            for(const vk::LayerProperties& supportedLayer : supportedLayers)
            {
                if(strcmp(layer, supportedLayer.layerName) == 0)
                {
                    found = true;
                    BeeCoreTrace("Layer {0} is supported", layer);
                    break;
                }
            }
            if(!found)
            {
                BeeCoreError("Layer {0} is not supported", layer);
                return false;
            }
        }
        return true;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData)
    {
        BeeCoreError("Validation Layer: {0}", pCallbackData->pMessage);
        return VK_FALSE;
    }

    void VulkanInstance::MakeDebugMessenger()
    {
        vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
                vk::DebugUtilsMessengerCreateFlagsEXT(),
                /*vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | */vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
                DebugCallback,
                nullptr
                );
        m_DebugMessenger = m_Instance.createDebugUtilsMessengerEXT(createInfo, nullptr, m_DynamicLoader);
    }

    void VulkanInstance::ManageGLFW(vk::ApplicationInfo& appInfo)
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#if defined(DEBUG)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        BeeCoreTrace("Extensions to be requested: ");
        for (const char* extension : extensions)
        {
            BeeCoreTrace("\t{0}", extension);
        }

        std::vector<const char*> layers;
#if defined(BEE_VULKAN_ENABLE_VALIDATION_LAYERS)
        layers.push_back("VK_LAYER_KHRONOS_validation");
        if(!LayersSupported(layers))
        {
            BeeCoreError("Required layers are not supported!");
            layers.clear();
        }
#endif


        if(!ExtensionsSupported(extensions))
        {
            BeeCoreError("Required extensions are not supported!");
            m_Instance = nullptr;
            return;
        }

        /*
         * from vulkan_structs.hpp:
         *
         * VULKAN_HPP_CONSTEXPR InstanceCreateInfo( uint32_t flags_ = {},
         *                                          const ApplicationInfo* pApplicationInfo_ = {},
         *                                          uint32_t enabledLayerCount_ = {},
         *                                          const char* const* ppEnabledLayerNames_ = {},
         *                                          uint32_t enabledExtensionCount_ = {},
         *                                          const char* const* ppEnabledExtensionNames_ = {} ) VULKAN_HPP_NOEXCEPT
         */
        vk::InstanceCreateInfo createInfo(vk::InstanceCreateFlags()
                , &appInfo,
                                          layers.size(),
                                          layers.data(),
                                          extensions.size(),
                                          extensions.data());
        try
        {
            m_Instance = vk::createInstance(createInfo, nullptr);
        }
        catch (vk::SystemError& err)
        {
            BeeCoreError("Failed to create Vulkan Instance: {0}", err.what());
            m_Instance = nullptr;
        }

    }

    Ref<VulkanSurface> VulkanInstance::CreateSurface()
    {
        return CreateRef<VulkanSurface>(m_WindowApi, m_Instance);
    }
}
