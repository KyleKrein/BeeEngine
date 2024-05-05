//
// Created by alexl on 20.07.2023.
//

#pragma once
#include "Core/AssetManagement/EditorAssetManager.h"
#include "Core/Coroutines/Generator.h"
#include "Core/OsPlatform.h"
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
        ProjectFile(const Path& projectPath, const std::string& projectName, EditorAssetManager* assetManager) noexcept;

        [[nodiscard]] const Path& GetProjectPath() const noexcept;
        [[nodiscard]] const std::string& GetProjectName() const noexcept;
        [[nodiscard]] const Path& GetProjectFilePath() const noexcept;
        [[nodiscard]] const Path& GetProjectAssetRegistryPath() const noexcept;

        Path GetLastUsedScenePath() const noexcept
        {
            return m_LastUsedScenePath.IsEmpty() ? m_LastUsedScenePath : m_ProjectPath / m_LastUsedScenePath;
        }
        void SetLastUsedScenePath(const Path& path) noexcept;

        void SetLastUsedSceneToNull() noexcept
        {
            m_LastUsedScenePath = "";
            Save();
        }

        bool IsAssemblyReloadPending() const noexcept
        {
            bool temp = m_AssemblyReloadPending;
            m_AssemblyReloadPending = false;
            return temp;
        }

        UUID GetAssetRegistryID() const noexcept { return m_AssetRegistryID; }

        void RenameProject(const std::string& newName) noexcept;

        void RegenerateSolution();

        void BuildProject(const BuildProjectOptions& options);

        void Save() noexcept;

        void Update() noexcept;

        Locale::Domain& GetProjectLocaleDomain() noexcept { return m_ProjectLocaleDomain; }

        const Path& GetAssemblyPath() const { return m_AppAssemblyPath; }

        void ReloadAndRebuildGameLibrary();

        void SetOnAssetRemovedCallback(std::function<void(AssetHandle)> callback) noexcept
        {
            m_OnAssetRemoved = std::move(callback);
        }

    private:
        std::vector<std::pair<OSPlatform, Path>> CheckForAvailablePlatforms();
        Path BuildWindowsGame(const Path& gameLibraryPath, const Path& outputDirectory);
        Path BuildMacOSGame(const Path& gameLibraryPath, const Path& outputDirectory);
        void OnAppAssemblyFileSystemEvent(const Path& path, FileWatcher::Event changeType);
        void OnAssetFileSystemEvent(const Path& path, FileWatcher::Event changeType);

        void LoadLocalizationFiles();

        Path m_ProjectPath;
        std::string m_ProjectName;
        Path m_ProjectFilePath = m_ProjectPath / (m_ProjectName + ".beeproj");
        Path m_ProjectAssetRegistryPath = m_ProjectPath / (m_ProjectName + ".beeassetregistry");
        Path m_LastUsedScenePath{""};
        Scope<FileWatcher> m_AppAssemblyFileWatcher = nullptr;
        mutable bool m_AssemblyReloadPending = false;
        Path m_AppAssemblyPath;
        BeeEngine::UUID m_AssetRegistryID;
        Locale::Domain m_ProjectLocaleDomain;

        Scope<FileWatcher> m_AssetFileWatcher = nullptr;
        EditorAssetManager* m_AssetManager;
        std::function<void(AssetHandle)> m_OnAssetRemoved;

        void HandleChangedScriptFile(const Path& path, FileWatcher::Event event);

        Generator<const AssetMetadata&> GetAssetsForDirectory(const Path& directory);

        bool m_MustReload;
    };
} // namespace BeeEngine::Editor
