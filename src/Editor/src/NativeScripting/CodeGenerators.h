//
// Created by alexl on 21.07.2023.
//

#pragma once
#include <vector>
#include <filesystem>
#include <string>
#include "ScriptParser.h"
#include <sstream>
namespace BeeEngine::Editor
{
    namespace CodeGenerators
    {
        static std::vector<std::filesystem::path> s_Visited;

        static std::filesystem::path s_WorkingDirectory;
        static std::filesystem::path s_OutputDirectory;

        static bool WasSourceFileVisited(const BeeClass& beeClass)
        {
            return std::find(s_Visited.begin(), s_Visited.end(), beeClass.Path) != s_Visited.end();
        }

        static void GenerateInitFile(const std::vector<BeeClass> beeClasses, const std::filesystem::path filepath)
        {
            std::ostringstream source;

            source << "#include <BeeEngine.h>\n";

            for (const auto& beeClass : beeClasses)
            {
                if (WasSourceFileVisited(beeClass))
                    continue;
                if(!beeClass.Path.is_absolute())
                    source << "#include \"" << s_WorkingDirectory.string() << "\\" << beeClass.Path.filename().string() << "\"\n";
                else
                    source << "#include \"" << beeClass.Path.filename().string() << "\"\n";
                s_Visited.push_back(beeClass.Path);
            }

            source << "#define ENTT_STANDARD_CPP\n";
            source << "#include <entt/entt.hpp>\n";

            source << "extern \"C\" namespace BeeEngine\n";
            source << "{\n";
            source << "\textern \"C\" namespace Generated\n";
            source << "\t{\n";

            // Init Component Id's -------------------------------------------------------------------------------------------
            source << "\t\tstatic void InitComponentIds(SceneData & scene)\n";
            source << "\t\t{\n";
            source << "\t\t\tNEntity::RegisterComponentType<TransformComponent>();\n";
            source << "\t\t\tNEntity::RegisterComponentType<TagComponent>();\n";
            source << "\t\t\tNEntity::RegisterComponentType<SpriteRendererComponent>();\n";
            source << "\t\t\tNEntity::RegisterComponentType<CameraComponent>();\n";

            for (auto& beeClass : beeClasses)
            {
                source << "\t\t\tNEntity::RegisterComponentType<" << beeClass.Name << ">();\n";
            }

            source << "\t\t}\n";

            source << "\t}\n";
            source << "}\n";
        }

        static void GenerateCMakeFile(const std::filesystem::path& filepath)
        {

        }
    };
}
