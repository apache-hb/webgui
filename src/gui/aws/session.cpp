#include "session.hpp"
#include "window.hpp"

#include <imgui.h>

#include <aws/sts/STSClient.h>

ImAws::Session::Session(int id)
    : mLoggedIn(false)
    , mSessionTitle(std::format("Session {}", id))
{ }

void ImAws::Session::draw() {
    if (!mLoggedIn) {
        if (mCallerIdentity.isComplete()) {
            auto outcome = mCallerIdentity.getResult();
            if (outcome.IsSuccess()) {
                mLoggedIn = true;
                auto result = outcome.GetResult();
                mSessionTitle = std::format("Session - {} ({})",
                                            result.GetArn(),
                                            result.GetAccount());
            } else {
                ImGui::OpenPopup("Login Failed");
            }

            if (ImGui::BeginPopupModal("Login Failed")) {
                ImAws::ApiErrorTooltip(outcome.GetError());
                if (ImGui::Button("OK")) {
                    mCallerIdentity.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        if (ImGui::Begin("Login")) {
            ImAws::RegionCombo("##Region", &mRegion, ImGuiComboFlags_WidthFitPreview);
            ImGui::SameLine();
            ImAws::InputCredentials(&mCredentials);

            ImGui::BeginDisabled(mCallerIdentity.isWorking());
            const char *label = mCallerIdentity.isWorking() ? "Logging in..." : "Login";
            if (ImGui::Button(label)) {
                mCallerIdentity.run([this, provider = mCredentials.createProvider()]() {
                    Aws::Client::ClientConfigurationInitValues clientConfigInitValues;
                    clientConfigInitValues.shouldDisableIMDS = true;

                    Aws::STS::STSClientConfiguration config{clientConfigInitValues};
                    config.region = mRegion.getSelectedRegionId();

                    Aws::STS::STSClient stsClient{provider, config};
                    return stsClient.GetCallerIdentity({});
                });
            }
            ImGui::EndDisabled();
        }

        ImGui::End();
        return;
    }

    for (auto& window : mWindows) {
        window->drawWindow();
    }
}

void ImAws::SessionMenu(Session& session) {
    ImGui::SeparatorText(session.mSessionTitle.c_str());
    for (auto& window : session.mWindows) {
        ImAws::WindowMenuItem(*window);
    }
}
