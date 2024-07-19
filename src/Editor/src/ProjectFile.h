//
// Created by alexl on 20.07.2023.
//

#pragma once
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/EditorAssetManager.h"
#include "Core/Coroutines/Generator.h"
#include "Core/OsPlatform.h"
#include "Core/Property.h"
#include "Core/TypeDefines.h"
#include "Core/UUID.h"
#include "FileSystem/FileWatcher.h"
#include "Locale/Locale.h"
#include <filesystem>

namespace BeeEngine::Editor
{
    struct BuildProjectOptions
    {
        enum class BuildType
        {
            Debug,
            Release
        };
        BuildType BuildType = BuildType::Debug;
        Path OutputPath;
        Locale::Localization DefaultLocale = Locale::Localization::Default;
    };
    class ProjectFile
    {
    public:
        ProjectFile(const Path& projectPath, const String& projectName, EditorAssetManager* assetManager) noexcept;

        Property<String> Name;
        Property<Path> FolderPath;
        Property<Path> FilePath;
        Property<Path> AssetRegistryPath;
        Property<Path> GameAssemblyPath;
        Property<Locale::Localization> DefaultLocale{Locale::Localization::Default};

        Signal<AssetHandle> onAssetRemoved;

        AssetHandle GetLastUsedScene() const noexcept { return m_LastUsedScene; }
        void SetLastUsedScene(const AssetHandle& handle);

        void SetLastUsedSceneToNull()
        {
            m_LastUsedScene = {0, 0};
            Save();
        }

        bool IsAssemblyReloadPending() const noexcept
        {
            bool temp = m_AssemblyReloadPending;
            m_AssemblyReloadPending = false;
            return temp;
        }

        void StartFileWatchers();
        void StopFileWatchers();

        UUID GetAssetRegistryID() const noexcept { return m_AssetRegistryID; }

        void RegenerateSolution();

        void BuildProject(const BuildProjectOptions& options);

        void Save();

        void Update() noexcept;

        const Locale::Domain& GetProjectLocaleDomain() const noexcept { return m_ProjectLocaleDomain; }

        void ReloadAndRebuildGameLibrary();

        const String& GetStartingSceneName() const
        {
            if (m_StartingScene == AssetHandle{0, 0})
            {
                static String notAvailable = "Not available";
                return notAvailable;
            }
            return m_AssetManager->GetAssetMetadata(m_StartingScene).Name;
        }
        void SetStartingScene(const AssetHandle& handle) { m_StartingScene = handle; }

    private:
        std::vector<std::pair<OSPlatform, Path>> CheckForAvailablePlatforms();
        Path BuildWindowsGame(const Path& gameLibraryPath, const Path& outputDirectory);
        Path BuildMacOSGame(const Path& gameLibraryPath, const Path& outputDirectory);
        void OnAppAssemblyFileSystemEvent(const Path& path, FileWatcher::Event changeType);
        void OnAssetFileSystemEvent(const Path& path, FileWatcher::Event changeType);

        void LoadLocalizationFiles();
        AssetHandle m_LastUsedScene{0, 0};
        AssetHandle m_StartingScene{0, 0};
        Scope<FileWatcher> m_AppAssemblyFileWatcher = nullptr;
        mutable bool m_AssemblyReloadPending = false;
        BeeEngine::UUID m_AssetRegistryID;
        Locale::Domain m_ProjectLocaleDomain;

        Scope<FileWatcher> m_AssetFileWatcher = nullptr;
        EditorAssetManager* m_AssetManager;

        void HandleChangedScriptFile(const Path& path, FileWatcher::Event event);

        Generator<const AssetMetadata&> GetAssetsForDirectory(const Path& directory);

        bool m_MustReload;
    };
} // namespace BeeEngine::Editor
