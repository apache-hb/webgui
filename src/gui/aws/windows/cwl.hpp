#pragma once

#include "gui/aws/errors.hpp"
#include "gui/aws/window.hpp"
#include "gui/imaws.hpp"
#include "util/describe.hpp"

#include <imgui.h>

#include <aws/logs/CloudWatchLogsClient.h>

namespace ImAws {
    class CloudWatchLogsPanel final : public IWindow {
        using LogGroup = Aws::CloudWatchLogs::Model::LogGroup;
        using CwlError = Aws::CloudWatchLogs::CloudWatchLogsError;

        sm::AsyncDescribe<LogGroup, CwlError> mLogGroupDescribe;
        sm::ErrorPanel mErrorPanel;

        ImGuiTableFlags mTableFlags{ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV};

        Aws::CloudWatchLogs::CloudWatchLogsClient createCwlClient() {
            auto provider = getSessionCredentialsProvider();

            Aws::Client::ClientConfigurationInitValues clientConfigInitValues;
            clientConfigInitValues.shouldDisableIMDS = true;

            Aws::CloudWatchLogs::CloudWatchLogsClientConfiguration config{clientConfigInitValues};
            config.region = getSessionRegion();

            return Aws::CloudWatchLogs::CloudWatchLogsClient{provider, config};
        }

        std::string unix_epoch_ms_to_datetime_string(long long epochMs) {
            std::chrono::system_clock::time_point tp{std::chrono::milliseconds{epochMs}};
            return std::format("{0:%Y}-{0:%m}-{0:%d}:{0:%H}:{0:%M}:{0:%S}", tp);
        }

        template<typename F, typename E>
        void fetchAllLogGroups(F&& add, E&& err, std::stop_token stop) {
            auto cwlClient = createCwlClient();

            Aws::CloudWatchLogs::Model::DescribeLogGroupsRequest request;
            request.SetLimit(50);

            Aws::String nextToken;
            do {
                if (!nextToken.empty()) {
                    request.SetNextToken(nextToken);
                }

                auto outcome = cwlClient.DescribeLogGroups(request);
                if (!outcome.IsSuccess()) {
                    err(outcome.GetError());
                    break;
                }

                const auto& result = outcome.GetResult();
                for (const auto& logGroup : result.GetLogGroups()) {
                    add(logGroup);
                }

                nextToken = result.GetNextToken();
            } while (!nextToken.empty() && !stop.stop_requested());
        }
    public:
        using IWindow::IWindow;

        void draw() override {
            bool isFetching = mLogGroupDescribe.isWorking();
            ImGui::BeginDisabled(isFetching);
            if (ImGui::Button(isFetching ? "Working..." : "Fetch")) {
                mLogGroupDescribe.run([this](auto&& add, auto&& err, std::stop_token stop) {
                    fetchAllLogGroups(add, err, stop);
                });
            }
            ImGui::EndDisabled();

            if (mLogGroupDescribe.hasError()) {
                mErrorPanel.addError(mLogGroupDescribe.error());
                mLogGroupDescribe.clear();
            }

            mErrorPanel.draw();

            auto logGroups = mLogGroupDescribe.getItems();

            if (ImGui::BeginTable("Log Groups", 3, mTableFlags)) {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("ARN", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Creation Time", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                for (const auto& group : logGroups) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(group.GetLogGroupName().c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImAws::ArnTooltip(group.GetArn());

                    ImGui::TableSetColumnIndex(2);
                    auto time = unix_epoch_ms_to_datetime_string(group.GetCreationTime());
                    ImGui::TextUnformatted(time.c_str());
                }

                ImGui::EndTable();
            }
        }
    };
}
