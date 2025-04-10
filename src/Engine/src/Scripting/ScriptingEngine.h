//
// Created by alexl on 29.07.2023.
//

#pragma once
#include "Core/AssetManagement/Asset.h"
#include "Core/Path.h"
#include "Core/Time.h"
#include "Core/TypeDefines.h"
#include "Core/UUID.h"
#include "KeyCodes.h"
#include "Locale/Locale.h"
#include "MTypes.h"
#include "MUtils.h"
#include "Renderer/CommandBuffer.h"
#include <functional>
#include <glm/glm.hpp>

namespace BeeEngine
{
    struct Entity;
    class MClass;
    class MField;
    class ScriptingEngine
    {
    public:
        static void Init(std::function<void(AssetHandle)> onSceneChangeCallback);
        static void Shutdown();
        static bool IsInitialized();
        static void SetLocaleDomain(Locale::Domain& domain);
        static Locale::Domain& GetLocaleDomain();
        static const String& GetScriptingLocale();
        static void EnableDebugging();

        static void LoadGameAssembly(const Path& path);
        static void LoadGameAssembly(const Path& path, const Path& debugSymbolsPath);

        static void LoadCoreAssembly(const Path& path);
        static void LoadCoreAssembly(const Path& path, const Path& debugSymbolsPath);
        static void ReloadAssemblies();
        static class MAssembly& GetCoreAssembly();
        static void OnRuntimeStart(class Scene* scene);

        static void OnRuntimeStop();
        static MClass& GetGameScript(const String& name);

        static bool HasGameScript(const String& name);
        static const std::unordered_map<String, Ref<MClass>>& GetGameScripts();
        static void RegisterNativeFunction(const String& name, void* function);

        static void OnEntityCreated(Entity entity, MClass* pClass);

        static void OnEntityDestroyed(UUID uuid);
        static void OnEntityUpdate(Entity entity);
        static void OnEntityRender(Entity entity, CommandBuffer& cmd);
        static class MObject* GetEntityScriptInstance(Entity entity);

        static void UpdateAllocatorStatistics();

        static Scene* GetSceneContext();

        static MClass& GetEntityClass();
        static class GameScript* GetEntityScriptInstance(BeeEngine::UUID uuid);

        static std::vector<class GameScriptField>& GetDefaultScriptFields(MClass* klass);
        static void SetAssetHandle(MObject& obj, MField& field, AssetHandle& handle, MType type);

        static void GetAssetHandle(void* monoObject, AssetHandle& handle);

        static void UpdateTime(Time::secondsD deltaTime, Time::secondsD totalTime);

        static void SetMousePosition(int x, int y);
        static glm::vec2 GetMousePosition();

        static glm::vec2 GetViewportSize();
        static void SetViewportSize(float width, float height);

        static void UnloadAppContext();

        static void RequestSceneChange(AssetHandle sceneHandle);

        static void OnCollisionStart(UUID entity1, UUID entity2);
        static void OnCollisionEnd(UUID entity1, UUID entity2);

        static void OnMouseClick(UUID entity, MouseButton button);
        static void OnMouseEnter(UUID entity);
        static void OnMouseLeave(UUID entity);

    private:
        static void InitDotNetHost();
        static class MAssembly& LoadAssembly(const Path& path, const Path& debugSymbolsPath);
        static void InitMono();
        static bool IsGameScript(const MClass& klass);
        static bool AreAllManagedHandlesLoaded();
        static struct ScriptingEngineData s_Data;

        static void MonoShutdown();

        static void CreateAppDomain();
    };
} // namespace BeeEngine
