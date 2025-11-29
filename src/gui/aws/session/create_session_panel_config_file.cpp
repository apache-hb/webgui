#include "aws/core/config/AWSConfigFileProfileConfigLoader.h"
#include "create_session_panel.hpp"
#include "platform/generic.hpp"
#include "platform/platform.hpp"

#include <aws/core/auth/AWSCredentialsProvider.h>

#include <imgui.h>

namespace {
    class CreateSessionPanel_ConfigFile final : public sm::ICreateSessionPanel {
        Aws::String mCredentialsFile;
        Aws::Config::AWSConfigFileProfileConfigLoader mConfigLoader;
        Aws::String mProfileName;

        static Aws::String selectBestProfile(Aws::Config::AWSConfigFileProfileConfigLoader& loader) {
            loader.Load();

            const auto& profiles = loader.GetProfiles();

            //
            // If "default" exists, use it; otherwise use the first profile found.
            //
            if (auto it = profiles.find("default"); it != profiles.end()) {
                return "default";
            }

            if (auto next = profiles.begin(); next != profiles.end()) {
                return next->first;
            }

            return {};
        }

    public:
        CreateSessionPanel_ConfigFile()
            : mCredentialsFile(Aws::Auth::ProfileConfigFileAWSCredentialsProvider::GetCredentialsProfileFilename())
            , mConfigLoader(mCredentialsFile)
            , mProfileName(selectBestProfile(mConfigLoader))
        { }

        std::string_view getName() const override {
            return "Config File";
        }

        bool isDisabled() const override {
            return sm::isWebPlatform(sm::Platform::type());
        }

        std::string_view getDisabledReason() const override {
            return "Config file access is not supported on web platforms.";
        }

        std::shared_ptr<Aws::Auth::AWSCredentialsProvider> getCredentialsProvider() const override {
            if (mProfileName.empty()) {
                return nullptr;
            }

            return Aws::MakeShared<Aws::Auth::ProfileConfigFileAWSCredentialsProvider>(
                "CreateSessionPanel_ConfigFile",
                mProfileName.c_str()
            );
        }

        bool render() override {
            ImGui::TextUnformatted(mCredentialsFile.c_str());

            if (ImGui::Button("Reload Profiles")) {
                mProfileName = selectBestProfile(mConfigLoader);
            }

            ImGui::SameLine();

            const auto& profiles = mConfigLoader.GetProfiles();
            const char *selected = profiles.empty() ? "No Profiles Found" : mProfileName.c_str();
            ImGui::BeginDisabled(profiles.empty());

            if (ImGui::BeginCombo("Profile", selected, ImGuiComboFlags_WidthFitPreview)) {
                for (const auto& [name, profile] : mConfigLoader.GetProfiles()) {
                    bool isSelected = (mProfileName == name);

                    if (ImGui::Selectable(name.c_str(), isSelected)) {
                        mProfileName = name;
                    }

                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::EndDisabled();

            return !profiles.empty() && !mProfileName.empty();
        }
    };
}

std::unique_ptr<sm::ICreateSessionPanel> sm::CreateSessionPanel_ForConfigFile() {
    return std::make_unique<CreateSessionPanel_ConfigFile>();
}
