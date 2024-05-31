#include "ProjectSettings.h"
#include "Core/AssetManagement/AssetManager.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "Renderer/Texture.h"

namespace BeeEngine::Editor
{
    ProjectSettings::ProjectSettings(ProjectFile& currentProject,
                                     Locale::Domain& domain,
                                     EditorAssetManager& assetManager)
        : m_CurrentProject(currentProject),
          m_Domain(domain),
          m_AssetManager(assetManager),
          m_ProjectName(m_CurrentProject.GetProjectName())
    {
    }

    void ProjectSettings::Render()
    {
        if (!m_IsOpened)
            return;
        ImGui::Begin(m_Domain.Translate("projectSettings").c_str(), &m_IsOpened);
        ImGui::TextUnformatted(m_Domain.Translate("name").c_str());
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputText("##Project Name", &m_ProjectName, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            m_CurrentProject.RenameProject(m_ProjectName);
            m_CurrentProject.Save();
        }
        ImGui::TextUnformatted(m_Domain.Translate("buildProject.defaultLocale").c_str());
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::BeginCombo("##Default Localization",
                              m_CurrentProject.GetDefaultLocale().GetLanguageString().c_str()))
        {
            if (ImGui::BeginTooltip())
            {
                ImGui::TextUnformatted(m_Domain.Translate("buildProject.defaultLocale.tooltip").c_str());
                ImGui::EndTooltip();
            }
            for (const auto& locale : m_CurrentProject.GetProjectLocaleDomain().GetLocales())
            {
                bool isSelected = locale == m_CurrentProject.GetDefaultLocale();
                if (ImGui::Selectable(locale.c_str(), isSelected))
                {
                    m_CurrentProject.SetDefaultLocale(locale);
                    m_CurrentProject.Save();
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::TextUnformatted(m_Domain.Translate("buildProject.startingScene").c_str());
        ImGui::SameLine();
        if (ImGui::BeginCombo("##StartingScene", m_CurrentProject.GetStartingSceneName().c_str()))
        {
            if (ImGui::BeginTooltip())
            {
                ImGui::TextUnformatted(m_Domain.Translate("buildProject.startingScene.tooltip").c_str());
                ImGui::EndTooltip();
            }
            for (const auto& scene : m_AssetManager.GetAssetsOfType(AssetType::Scene))
            {
                bool isSelected = m_CurrentProject.GetStartingSceneName() == scene.Name;
                if (ImGui::Selectable(scene.Name.data(), isSelected))
                {
                    m_CurrentProject.SetStartingScene(scene.Handle);
                    m_CurrentProject.Save();
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        // TODO: starting scene
        // RenderStartingSceneSelector();
        // RenderIconsSelector();
        // TODO config file selector (select a script that loads the config file)
        ImGui::End();
    }

    void ProjectSettings::RenderIconsSelector()
    {
        ImGui::TextUnformatted(m_Domain.Translate("icon").c_str());
        ImGui::SameLine();
        bool isIconValid = AssetManager::IsAssetHandleValid(m_IconHandle);
        if (!isIconValid)
        {
            // ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::Button("##Icon");
        }
        else
        {
            ImGui::ImageButton(
                (ImTextureID)AssetManager::GetAsset<Texture2D>(m_IconHandle, m_Domain.GetLocale()).GetRendererID(),
                ImVec2(32, 32));
        }
        ImGui::AcceptDragAndDrop<AssetHandle>("ASSET_BROWSER_TEXTURE2D_ITEM",
                                              [this](const AssetHandle& handle)
                                              {
                                                  m_IconHandle = handle;
                                                  // TODO: save icon
                                              });
        if (isIconValid)
        {
            ImGui::TextUnformatted(AssetManager::GetAsset<Texture2D>(m_IconHandle, m_Domain.GetLocale()).Name.data());
        }
    }
} // namespace BeeEngine::Editor