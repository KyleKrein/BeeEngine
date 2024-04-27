//
// Created by alexl on 03.08.2023.
//

#include "VSProjectGeneration.h"
#include "Core/CodeSafety/Expects.h"
#include "FileSystem/File.h"
#include <fstream>
#include <sstream>
namespace BeeEngine
{

    std::vector <Path>
    BeeEngine::VSProjectGeneration::GetSourceFiles(const Path &path)
    {
        std::vector <Path> sources;
        auto stdpath = path.ToStdPath();
        for (const auto &entry : std::filesystem::recursive_directory_iterator(stdpath))
        {
            if(entry.path().string().contains(".beeengine"))
                continue;
            if (entry.path().extension() == ".cs")
            {
                sources.emplace_back(entry.path());
            }
        }
        return sources;
    }

    void BeeEngine::VSProjectGeneration::GenerateProject(const Path &path,
                                                         const std::vector <Path> &sources,
                                                         const std::string& projectName)
    {
        BeeExpects(IsValidString(String(projectName)));
        static constexpr unsigned char bom[] = {0xEF, 0xBB, 0xBF};

        std::ostringstream csproj;
        csproj << "<Project Sdk=\"Microsoft.NET.Sdk\">\n";
        csproj << '\n';
        csproj << "  <PropertyGroup>\n";
        csproj << "    <TargetFramework>net8.0</TargetFramework>\n";
        csproj << "    <ImplicitUsings>enable</ImplicitUsings>\n";
        csproj << "    <Nullable>enable</Nullable>\n";
        csproj << "    <LangVersion>12</LangVersion>\n";
        csproj << "    <EnableDefaultCompileItems>false</EnableDefaultCompileItems>\n";
        csproj << "  </PropertyGroup>\n";
        csproj << '\n';
        csproj << "  <PropertyGroup Condition=\" '$(Configuration)' == 'Debug' \">\n";
        csproj << "    <OutputType>Library</OutputType>\n";
        csproj << "    <IntermediateOutputPath>.beeengine\\obj\\</IntermediateOutputPath>\n";
        csproj << "    <BaseIntermediateOutputPath>.beeengine\\obj\\</BaseIntermediateOutputPath>\n";
        csproj << "   <RootNamespace>" << projectName << "</RootNamespace>";
        csproj << "    <AssemblyName>" << "GameLibrary" << "</AssemblyName>\n";
        csproj << "    <OutputPath>.beeengine\\build\\</OutputPath>\n";
        csproj << "    <DefineConstants>DEBUG;TRACE</DefineConstants>\n";
        csproj << "  </PropertyGroup>\n";
        csproj << "  <PropertyGroup Condition=\" '$(Configuration)' == 'Release' \">\n";
        csproj << "    <OutputType>Library</OutputType>\n";
        csproj << "    <IntermediateOutputPath>.beeengine\\obj\\</IntermediateOutputPath>\n";
        csproj << "    <BaseIntermediateOutputPath>.beeengine\\obj\\</BaseIntermediateOutputPath>\n";
        csproj << "   <RootNamespace>" << projectName << "</RootNamespace>";
        csproj << "    <AssemblyName>" << "GameLibrary" << "</AssemblyName>\n";
        csproj << "    <OutputPath>.beeengine\\build\\</OutputPath>\n";
        csproj << "    <DefineConstants>RELEASE;TRACE</DefineConstants>\n";
        csproj << "  </PropertyGroup>\n";
        csproj << "  <ItemGroup>\n";
        csproj << "    <Reference Include=\"BeeEngine.Core\">\n";
        csproj << "        <HintPath>.beeengine\\BeeEngine.Core.dll</HintPath>\n";
        csproj << "    </Reference>\n";
        csproj << "  </ItemGroup>\n";
        csproj << "  <ItemGroup>\n";
        auto stdpath = path.ToStdPath();
        for (const auto &sourcePath: sources)
        {
            auto source = sourcePath.ToStdPath();
            if (source.is_absolute())
            {
#if defined(WINDOWS)
                String sourcePathString = Path{source.lexically_relative(stdpath)}.AsUTF8();
                std::replace(sourcePathString.begin(), sourcePathString.end(), '/', '\\');
                csproj << "    <Compile Include=\"" << sourcePathString << "\"/>\n";
#else
                csproj << "    <Compile Include=\"" << Path{source.lexically_relative(stdpath)}.AsUTF8() << "\"/>\n";
#endif
            }
            else
            {
#if defined(WINDOWS)
                String sourcePathString = sourcePath.AsUTF8();
                std::replace(sourcePathString.begin(), sourcePathString.end(), '/', '\\');
                csproj << "    <Compile Include=\"" << sourcePathString << "\"/>\n";
#else
                csproj << "    <Compile Include=\"" << sourcePath.AsUTF8() << "\"/>\n";
#endif
            }
        }
        csproj << "  </ItemGroup>\n";
        csproj << "</Project>\n";

        csproj.flush();
        {
            std::ofstream ofs((path / (projectName + ".csproj")).ToStdPath());
            ofs.write((char*)bom, sizeof(bom));
            ofs << csproj.str();
            ofs.close();
        }

        std::ostringstream sln;
        sln << '\n';
        sln << "Microsoft Visual Studio Solution File, Format Version 12.00\n";
        sln << "# Visual Studio Version 17\n";
        sln << "VisualStudioVersion = 17.6.33829.357\n";
        sln << "MinimumVisualStudioVersion = 10.0.40219.1\n";
        sln << "Project(\"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}\") = \"" << projectName << "\", \"" << projectName << ".csproj\", \"{877FEBCD-BDEB-41C4-9887-F072CB32A77D}\"\n"; //TODO: GUID
        sln << "EndProject\n";
        sln << "Global\n";
        sln << "	GlobalSection(SolutionConfigurationPlatforms) = preSolution\n";
        sln << "		Debug|Any CPU = Debug|Any CPU\n";
        sln << "		Release|Any CPU = Release|Any CPU\n";
        sln << "	EndGlobalSection\n";
        sln << "	GlobalSection(ProjectConfigurationPlatforms) = postSolution\n";
        sln << "		{877FEBCD-BDEB-41C4-9887-F072CB32A77D}.Debug|Any CPU.ActiveCfg = Debug|Any CPU\n";
        sln << "		{877FEBCD-BDEB-41C4-9887-F072CB32A77D}.Debug|Any CPU.Build.0 = Debug|Any CPU\n";
        sln << "		{877FEBCD-BDEB-41C4-9887-F072CB32A77D}.Release|Any CPU.ActiveCfg = Release|Any CPU\n";
        sln << "		{877FEBCD-BDEB-41C4-9887-F072CB32A77D}.Release|Any CPU.Build.0 = Release|Any CPU\n";
        sln << "	EndGlobalSection\n";
        sln << "	GlobalSection(SolutionProperties) = preSolution\n";
        sln << "		HideSolutionNode = FALSE\n";
        sln << "	EndGlobalSection\n";
        sln << "	GlobalSection(ExtensibilityGlobals) = postSolution\n";
        sln << "		SolutionGuid = {B0F3E0A0-0B0A-4F0B-9F0A-0B0A0B0A0B0A}\n";
        sln << "	EndGlobalSection\n";
        sln << "EndGlobal\n";

        sln.flush();
        {
            std::ofstream ofs((path / (projectName + ".sln")).ToStdPath());
            ofs.write((char *) bom, sizeof(bom));
            ofs << sln.str();
            ofs.close();
        }
    }
}