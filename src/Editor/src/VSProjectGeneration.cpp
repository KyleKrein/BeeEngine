//
// Created by alexl on 03.08.2023.
//

#include "VSProjectGeneration.h"
#include "Core/CodeSafety/Expects.h"
#include "Utils/File.h"
#include <fstream>
#include <sstream>
namespace BeeEngine
{
    void BeeEngine::VSProjectGeneration::GenerateAssemblyInfoFile(const Path &path, std::string_view projectName)
    {
        BeeExpects(IsValidString(String(projectName)));
        std::ostringstream source;
        source << "using System.Reflection;\n";
        source << "using System.Runtime.InteropServices;\n";
        source << "\n";
        source << "// General Information about an assembly is controlled through the following\n";
        source << "// set of attributes. Change these attribute values to modify the information\n";
        source << "// associated with an assembly.\n";
        source << "[assembly: AssemblyTitle(\"" << projectName << "\")]\n";
        source << "[assembly: AssemblyDescription(\"\")]\n";
        source << "[assembly: AssemblyConfiguration(\"\")]\n";
        source << "[assembly: AssemblyCompany(\"\")]\n";
        source << "[assembly: AssemblyProduct(\"" << projectName << "\")]\n";
        source << "[assembly: AssemblyCopyright(\"Copyright ©\")]\n";
        source << "[assembly: AssemblyTrademark(\"\")]\n";
        source << "[assembly: AssemblyCulture(\"\")]\n";
        source << "\n";
        source << "// Setting ComVisible to false makes the types in this assembly not visible\n";
        source << "// to COM components.  If you need to access a type in this assembly from\n";
        source << "// COM, set the ComVisible attribute to true on that type.\n";
        source << "[assembly: ComVisible(false)]\n";
        source << "\n";
        source << "// Version information for an assembly consists of the following four values:\n";
        source << "//\n";
        source << "//      Major Version\n";
        source << "//      Minor Version\n";
        source << "//      Build Number\n";
        source << "//      Revision\n";
        source << "//\n";
        source << "// You can specify all the values or you can default the Build and Revision Numbers\n";
        source << "// by using the '*' as shown below:\n";
        source << "// [assembly: AssemblyVersion(\"1.0.*\")]\n";
        source << "[assembly: AssemblyVersion(\"1.0.0.0\")]\n";
        source << "[assembly: AssemblyFileVersion(\"1.0.0.0\")]\n";
        source.flush(); //Might not be necessary
        File::WriteFile(path / "AssemblyInfo.cs", source.str());
    }

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
        static constexpr unsigned char bom[] = { 0xEF,0xBB,0xBF };

        std::ostringstream csproj;
        csproj << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
        csproj << "<Project ToolsVersion=\"15.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";
        csproj << "  <Import Project=\"$(MSBuildExtensionsPath)\\$(MSBuildToolsVersion)\\Microsoft.Common.props\" Condition=\"Exists('$(MSBuildExtensionsPath)\\$(MSBuildToolsVersion)\\Microsoft.Common.props')\" />\n";
        csproj << "  <PropertyGroup>\n";
        csproj << "    <Configuration Condition=\" '$(Configuration)' == '' \">Debug</Configuration>\n";
        csproj << "    <Platform Condition=\" '$(Platform)' == '' \">AnyCPU</Platform>\n";
        csproj << "    <OutputType>Library</OutputType>\n";
        csproj << "    <IntermediateOutputPath>.beeengine\\obj\\</IntermediateOutputPath>\n";
        csproj << "    <BaseIntermediateOutputPath>.beeengine\\obj\\</BaseIntermediateOutputPath>\n";
        csproj << "   <RootNamespace>" << projectName <<"</RootNamespace>";
        csproj << "    <AssemblyName>" << "GameLibrary" << "</AssemblyName>\n";
        csproj << "    <TargetFrameworkVersion>v4.8</TargetFrameworkVersion>\n";
        csproj << "    <FileAlignment>512</FileAlignment>\n";
        csproj << "  </PropertyGroup>\n";
        csproj << "  <PropertyGroup Condition=\" '$(Configuration)|$(Platform)' == 'Debug|AnyCPU' \">\n";
        csproj << "    <PlatformTarget>AnyCPU</PlatformTarget>\n";
        csproj << "    <DebugSymbols>true</DebugSymbols>\n";
        csproj << "    <DebugType>full</DebugType>\n";
        csproj << "    <Optimize>false</Optimize>\n";
        csproj << "    <OutputPath>.beeengine\\build\\</OutputPath>\n";
        csproj << "    <DefineConstants>DEBUG;TRACE</DefineConstants>\n";
        csproj << "    <ErrorReport>prompt</ErrorReport>\n";
        csproj << "    <WarningLevel>4</WarningLevel>\n";
        csproj << "   </PropertyGroup>\n";
        csproj << "  <PropertyGroup Condition=\" '$(Configuration)|$(Platform)' == 'Release|AnyCPU' \">\n";
        csproj << "    <PlatformTarget>AnyCPU</PlatformTarget>\n";
        csproj << "    <DebugType>pdbonly</DebugType>\n";
        csproj << "    <Optimize>true</Optimize>\n";
        csproj << "    <OutputPath>.beeengine\\build\\</OutputPath>\n";
        csproj << "    <DefineConstants>TRACE;RELEASE</DefineConstants>\n";
        csproj << "    <ErrorReport>prompt</ErrorReport>\n";
        csproj << "    <WarningLevel>4</WarningLevel>\n";
        csproj << "  </PropertyGroup>\n";
        csproj << "  <ItemGroup>\n";
        csproj << "    <Reference Include=\"BeeEngine.Core\">\n";
        csproj << "        <HintPath>.beeengine\\BeeEngine.Core.dll</HintPath>\n";
        csproj << "    </Reference>\n";
        csproj << "    <Reference Include=\"System\" />\n";
        csproj << "    <Reference Include=\"System.Core\" />\n";
        csproj << "    <Reference Include=\"System.Xml\" />\n";
        csproj << "    <Reference Include=\"System.Data\" />\n";
        csproj << "  </ItemGroup>\n";
        csproj << "  <ItemGroup>\n";
        auto stdpath = path.ToStdPath();
        for (const auto &sourcePath : sources)
        {
            auto source = sourcePath.ToStdPath();
            if(source.is_absolute())
                csproj << "    <Compile Include=\"" << Path{source.lexically_relative(stdpath)}.AsUTF8() << "\"/>\n";
            else
                csproj << "    <Compile Include=\"" << sourcePath.AsUTF8() << "\"/>\n";
        }
        csproj << "  </ItemGroup>\n";
        csproj << "  <Import Project=\"$(MSBuildToolsPath)\\Microsoft.CSharp.targets\" />\n";
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