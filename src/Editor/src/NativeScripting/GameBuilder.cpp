//
// Created by alexl on 26.07.2023.
//

#include "GameBuilder.h"
#include "Core/ResourceManager.h"
#include "NativeScripting/CodeGenerators.h"
#include <RuntimeObjectSystem.h>
#include <RuntimeInclude.h>
#include "NativeScripting/StdioLogSystem.h"
#include <Compiler.h>
#include <string>
#include <filesystem>

namespace BeeEngine::Editor
{
    static IRuntimeObjectSystem* g_pRuntimeObjectSystem = nullptr;
    static ICompilerLogger* g_pLogger = nullptr;
    void GameBuilder::RCCPPLoad(const std::filesystem::path& compilerPath)
    {
        if(!g_pRuntimeObjectSystem)
        {
            g_pRuntimeObjectSystem = new RuntimeObjectSystem();
            if(!g_pRuntimeObjectSystem)
            {
                BeeCoreError("Failed to create runtime object system");
                return;
            }
            g_pLogger = new StdioLogSystem();
            if(!g_pRuntimeObjectSystem->Initialise(g_pLogger, nullptr))
            {
                delete g_pRuntimeObjectSystem;
                g_pRuntimeObjectSystem = nullptr;
                BeeCoreError("Failed to initialize runtime object system");
                return;
            }
            auto currentDirectory = std::filesystem::current_path();
            auto engineIncludePath = currentDirectory / "EngineInclude";
            g_pRuntimeObjectSystem->AddIncludeDir((engineIncludePath / "include").string().c_str());
            g_pRuntimeObjectSystem->AddIncludeDir((engineIncludePath / "src").string().c_str());
            g_pRuntimeObjectSystem->AddIncludeDir((engineIncludePath / "vendor" / "gsl" / "include").string().c_str());
            g_pRuntimeObjectSystem->AddIncludeDir((engineIncludePath / "vendor" / "glm" / "glm").string().c_str());
            g_pRuntimeObjectSystem->AddIncludeDir((engineIncludePath / "vendor" / "debugbreak").string().c_str());
            g_pRuntimeObjectSystem->AddIncludeDir((engineIncludePath / "vendor" / "magic_enum" / "include").string().c_str());
            g_pRuntimeObjectSystem->AddIncludeDir((engineIncludePath / "vendor" / "entt" / "single_include").string().c_str());
            g_pRuntimeObjectSystem->AddIncludeDir((engineIncludePath / "vendor" / "spdlog" / "include").string().c_str());

            BeeCoreTrace("Compiler path: {}", compilerPath.string());
            g_pRuntimeObjectSystem->SetCompilerLocation(compilerPath.string().c_str());

            //TODO: FIX ON APPLE CLANG g_pRuntimeObjectSystem->AddLibraryDir((currentDirectory / "libs" / ResourceManager::GetStaticLibraryName("BeeEngine")).string().c_str());
            auto sourcesPath = (m_WorkingDirectory/ "Scripts");
            g_pRuntimeObjectSystem->AddPathToSourceSearch(sourcesPath.string().c_str());
            g_pRuntimeObjectSystem->SetIntermediateDir((m_WorkingDirectory / ".beeengine" / "build").string().c_str());
            g_pRuntimeObjectSystem->SetOptimizationLevel(RCppOptimizationLevel::RCCPPOPTIMIZATIONLEVEL_DEBUG);

            g_pRuntimeObjectSystem->AddToRuntimeFileList((sourcesPath / "Init.cpp").string().c_str());
            g_pRuntimeObjectSystem->CompileAll(true);
        }

    }

    GameBuilder::GameBuilder(const std::filesystem::path &path,const std::filesystem::path& compilerPath)
    : m_WorkingDirectory(path), m_NativeGameLibraryName(ResourceManager::GetDynamicLibraryName(s_GameLibraryName))
    {
        m_TemporaryNames.emplace_back("GameLibraryTemp1");
        if(!std::filesystem::exists(m_WorkingDirectory / ".beeengine"))
            std::filesystem::create_directory(m_WorkingDirectory / ".beeengine");
        RCCPPLoad(compilerPath);
    }

    void GameBuilder::Load()
    {
        /*TODO: FIX ON APPLE CLANG if(!std::filesystem::exists(m_WorkingDirectory / ".beeengine" / m_NativeGameLibraryName))
        {
            BeeCoreError("Failed to find game library {0}", m_NativeGameLibraryName);
            return;
        }*/
        Unload();
        //TODO: FIX ON APPLE CLANG std::filesystem::copy(m_WorkingDirectory / ".beeengine" / s_GameLibraryName, m_WorkingDirectory / ".beeengine" / ResourceManager::GetDynamicLibraryName(m_TemporaryNames[0]), std::filesystem::copy_options::overwrite_existing);
        m_GameLibrary = CreateScope<DynamicLibrary>(m_WorkingDirectory / ".beeengine", m_TemporaryNames[0]);
    }

    void GameBuilder::Unload()
    {
        m_GameLibrary.reset();
    }

    void GameBuilder::Build()
    {
        system("cmake --build . --target GameLibrary --config Debug -DBEE_ENGINE_DIR=C://Projects//BeeEngineCPP//src//Engine -DBEE_GAME_PROJECT_FOLDER=\"C://Users//alexl//OneDrive//Рабочий стол//BeeEngineProjects//TestProject\" -DBEE_ENGINE_LIBRARY_PATH=C:\\Projects\\BeeEngineCPP\\build\\Debug\\src\\Engine\\libBeeEngine.a");
    }

    GameBuilder::~GameBuilder()
    {
        if(g_pRuntimeObjectSystem)
        {
            delete g_pRuntimeObjectSystem;
            g_pRuntimeObjectSystem = nullptr;
        }
        if(g_pLogger)
        {
            delete g_pLogger;
            g_pLogger = nullptr;
        }
    }

    bool GameBuilder::UpdateAndCompile()
    {
        //check status of any compile
        if( g_pRuntimeObjectSystem->GetIsCompiledComplete() )
        {
            // load module when compile complete
            g_pRuntimeObjectSystem->LoadCompiledModule();
            return true;
        }

        if( !g_pRuntimeObjectSystem->GetIsCompiling() )
        {
            g_pRuntimeObjectSystem->GetFileChangeNotifier()->Update( Time::DeltaTime() );
        }
        return false;
    }
}
