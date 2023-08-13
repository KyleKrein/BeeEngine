//
// Created by alexl on 03.08.2023.
//

#include "VSProjectGeneration.h"
#include <fstream>
#include <sstream>
namespace BeeEngine
{

    void BeeEngine::VSProjectGeneration::GenerateAssemblyInfoFile(const std::filesystem::path &path, std::string_view projectName)
    {
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
        std::ofstream file(path / "AssemblyInfo.cs", std::ios::out);
        file << source.str();
        file.close();
    }

    std::vector <std::filesystem::path>
    BeeEngine::VSProjectGeneration::GetSourceFiles(const std::filesystem::path &path)
    {
        std::vector <std::filesystem::path> sources;
        for (const auto &entry : std::filesystem::recursive_directory_iterator(path))
        {
            if(entry.path().string().contains(".beeengine"))
                continue;
            if (entry.path().extension() == ".cs")
            {
                sources.push_back(entry.path());
            }
        }
        return sources;
    }

    void BeeEngine::VSProjectGeneration::GenerateProject(const std::filesystem::path &path,
                                                         const std::vector <std::filesystem::path> &sources,
                                                         const std::string& projectName)
    {
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
        for (const auto &source : sources)
        {
            if(source.is_absolute())
                csproj << "    <Compile Include=" << source.lexically_relative(path) << "/>\n";
            else
                csproj << "    <Compile Include=" << source << "/>\n";
        }
        csproj << "  </ItemGroup>\n";
        csproj << "  <Import Project=\"$(MSBuildToolsPath)\\Microsoft.CSharp.targets\" />\n";
        csproj << "</Project>\n";

        std::fstream csprojStream(path / (projectName + ".csproj"), std::ios::out);
        csproj.flush();
        csprojStream << csproj.str();
        csprojStream.close();

        std::ostringstream sln;
        sln << "Microsoft Visual Studio Solution File, Format Version 12.00\n";
        sln << "Project(\"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}\") = \"" << projectName << "\", \"" << projectName << ".csproj\", \"{425D7973-9025-4564-B598-73F36A5979FE}\"\n"; //TODO: GUID
        sln << "EndProject\n";
        sln << "Global\n";
        sln << "	GlobalSection(SolutionConfigurationPlatforms) = preSolution\n";
        sln << "		Debug|Any CPU = Debug|Any CPU\n";
        sln << "		Release|Any CPU = Release|Any CPU\n";
        sln << "	EndGlobalSection\n";
        sln << "	GlobalSection(ProjectConfigurationPlatforms) = postSolution\n";
        sln << "		{425D7973-9025-4564-B598-73F36A5979FE}.Debug|Any CPU.ActiveCfg = Debug|Any CPU\n";
        sln << "		{425D7973-9025-4564-B598-73F36A5979FE}.Debug|Any CPU.Build.0 = Debug|Any CPU\n";
        sln << "		{425D7973-9025-4564-B598-73F36A5979FE}.Release|Any CPU.ActiveCfg = Release|Any CPU\n";
        sln << "		{425D7973-9025-4564-B598-73F36A5979FE}.Release|Any CPU.Build.0 = Release|Any CPU\n";
        sln << "	EndGlobalSection\n";
        sln << "EndGlobal\n";

        sln.flush();
        std::fstream slnStream(path / (projectName + ".sln"), std::ios::out);
        slnStream << sln.str();
        slnStream.close();
    }
}