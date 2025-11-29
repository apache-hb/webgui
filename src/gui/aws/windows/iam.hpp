#pragma once

#include "gui/aws/window.hpp"
#include "gui/imaws.hpp"
#include "util/describe.hpp"

#include <imgui.h>

#include <aws/iam/IAMClient.h>

namespace ImAws {
    class IamPanel final : public IWindow {
        using Role = Aws::IAM::Model::Role;
        using IamError = Aws::IAM::IAMError;

        sm::AsyncDescribe<Role, IamError> mRoleDescribe;

        ImGuiTableFlags mTableFlags{ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV};

        static std::string unixEpochMsToDateTimeString(long long epochMs) {
            std::chrono::system_clock::time_point tp{std::chrono::milliseconds{epochMs}};
            return std::format("{0:%Y}-{0:%m}-{0:%d}:{0:%H}:{0:%M}:{0:%S}", tp);
        }

        Aws::IAM::IAMClient createIamClient() {
            auto provider = getSessionCredentialsProvider();

            Aws::Client::ClientConfigurationInitValues clientConfigInitValues;
            clientConfigInitValues.shouldDisableIMDS = true;

            Aws::IAM::IAMClientConfiguration config{clientConfigInitValues};
            config.region = getSessionRegion();

            return Aws::IAM::IAMClient{provider, config};
        }

        template<typename F, typename E>
        void fetchAllRoles(F&& add, E&& err, std::stop_token stop) {
            auto iamClient = createIamClient();

            Aws::IAM::Model::ListRolesRequest request;
            request.SetMaxItems(50);

            Aws::String marker;
            do {
                if (!marker.empty()) {
                    request.SetMarker(marker);
                }

                auto outcome = iamClient.ListRoles(request);
                if (!outcome.IsSuccess()) {
                    err(outcome.GetError());
                    break;
                }

                const auto& result = outcome.GetResult();
                for (const auto& role : result.GetRoles()) {
                    add(role);
                }

                marker = result.GetMarker();
            } while (!marker.empty() && !stop.stop_requested());
        }

    public:
        using IWindow::IWindow;

        void draw() override {
            bool isFetching = mRoleDescribe.isWorking();
            ImGui::BeginDisabled(isFetching);
            if (ImGui::Button(isFetching ? "Working..." : "Fetch")) {
                mRoleDescribe.run([this](auto&& add, auto&& err, std::stop_token stop) {
                    fetchAllRoles(add, err, stop);
                });
            }
            ImGui::EndDisabled();

            if (mRoleDescribe.hasError()) {
                ImGui::SameLine();
                ImAws::ApiErrorTooltip(mRoleDescribe.error());
            }

            auto roles = mRoleDescribe.getItems();

            if (ImGui::BeginTable("IAM Roles", 3, mTableFlags)) {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("ARN", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Creation Time", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                for (const auto& role : roles) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(role.GetRoleName().c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImAws::ArnTooltip(role.GetArn());

                    ImGui::TableSetColumnIndex(2);
                    auto time = unixEpochMsToDateTimeString(role.GetCreateDate().Millis());
                    ImGui::TextUnformatted(time.c_str());
                }

                ImGui::EndTable();
            }
        }
    };
}
