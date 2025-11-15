#include <chrono>
#include <stdio.h>

#include "gui/aws/session.hpp"
#include "gui/aws/window.hpp"
#include "platform/platform.hpp"
#include "platform/aws.hpp"
#include "gui/imaws.hpp"
#include "util/describe.hpp"

#include <concurrentqueue.h>

#include <imgui.h>
#include <implot.h>
#include <implot3d.h>
#include <misc/cpp/imgui_stdlib.h>

#include <aws/core/Region.h>
#include <aws/core/Version.h>
#include <aws/core/Aws.h>

#include <aws/core/auth/AWSCredentials.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/http/HttpClient.h>
#include <aws/core/http/HttpClientFactory.h>
#include <aws/core/http/standard/StandardHttpRequest.h>
#include <aws/core/http/standard/StandardHttpResponse.h>

#include <aws/logs/CloudWatchLogsClient.h>
#include <aws/logs/CloudWatchLogsServiceClientModel.h>
#include <aws/logs/model/DescribeLogGroupsRequest.h>

#include <aws/iam/IAMClient.h>
#include <aws/iam/model/ListRolesRequest.h>
#include <aws/iam/model/ListPoliciesRequest.h>


static ImAws::AwsRegion gAwsRegion;
static ImAws::AwsCredentialState gAwsCredentials;

namespace {
    std::vector<std::unique_ptr<ImAws::Session>> gSessions;

    Aws::CloudWatchLogs::CloudWatchLogsClient createCwlClient() {
        auto provider = gAwsCredentials.createProvider();

        Aws::Client::ClientConfigurationInitValues clientConfigInitValues;
        clientConfigInitValues.shouldDisableIMDS = true;

        Aws::CloudWatchLogs::CloudWatchLogsClientConfiguration config{clientConfigInitValues};
        config.region = gAwsRegion.getSelectedRegionId();

        return Aws::CloudWatchLogs::CloudWatchLogsClient{provider, config};
    }

    Aws::IAM::IAMClient createIamClient() {
        auto provider = gAwsCredentials.createProvider();

        Aws::Client::ClientConfigurationInitValues clientConfigInitValues;
        clientConfigInitValues.shouldDisableIMDS = true;

        Aws::IAM::IAMClientConfiguration config{clientConfigInitValues};
        config.region = gAwsRegion.getSelectedRegionId();

        return Aws::IAM::IAMClient{provider, config};
    }

    std::string unix_epoch_ms_to_datetime_string(long long epochMs) {
        std::chrono::system_clock::time_point tp{std::chrono::milliseconds{epochMs}};
        return std::format("{0:%Y}-{0:%m}-{0:%d}:{0:%H}:{0:%M}:{0:%S}", tp);
    }
}

class LogGroupsPanel {
    using LogGroup = Aws::CloudWatchLogs::Model::LogGroup;
    using CwlError = Aws::CloudWatchLogs::CloudWatchLogsError;

    sm::AsyncDescribe<LogGroup, CwlError> mLogGroupDescribe;

    ImGuiTableFlags mTableFlags{ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV};

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
    void render() {
        bool isFetching = mLogGroupDescribe.isWorking();
        ImGui::BeginDisabled(isFetching);
        if (ImGui::Button(isFetching ? "Working..." : "Fetch")) {
            mLogGroupDescribe.run([this](auto&& add, auto&& err, std::stop_token stop) {
                fetchAllLogGroups(add, err, stop);
            });
        }
        ImGui::EndDisabled();

        if (mLogGroupDescribe.hasError()) {
            ImGui::SameLine();
            ImAws::ApiErrorTooltip(mLogGroupDescribe.error());
        }

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

class IamRolesPanel {
    using Role = Aws::IAM::Model::Role;
    using IamError = Aws::IAM::IAMError;

    sm::AsyncDescribe<Role, IamError> mRoleDescribe;

    ImGuiTableFlags mTableFlags{ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV};

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
    void render() {
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
                auto time = unix_epoch_ms_to_datetime_string(role.GetCreateDate().Millis());
                ImGui::TextUnformatted(time.c_str());
            }

            ImGui::EndTable();
        }
    }
};

static bool gShowDemoWindow = true;
static bool gShowPlotDemoWindow = true;
static bool gShowPlot3dDemoWindow = true;
static bool gShowIamRolesWindow = false;
static bool gShowLogGroupsWindow = false;
static bool gShowAwsSdkInfoWindow = false;

static IamRolesPanel gIamRolesPanel;
static LogGroupsPanel gLogGroupsPanel;

void loop() {
    sm::Platform::begin();

    ImGui::DockSpaceOverViewport();

    if (ImGui::BeginMainMenuBar()) {
        ImGui::TextUnformatted("AWS C++ WASM Example");
        ImGui::Separator();

        if (ImGui::BeginMenu("Sessions")) {
            for (auto& session : gSessions) {
                ImAws::SessionMenu(*session);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows")) {
            ImGui::MenuItem("ImGui Demo Window", nullptr, &gShowDemoWindow);
            ImGui::MenuItem("ImPlot Demo Window", nullptr, &gShowPlotDemoWindow);
            ImGui::MenuItem("ImPlot3D Demo Window", nullptr, &gShowPlot3dDemoWindow);
            ImGui::MenuItem("AWS SDK Info", nullptr, &gShowAwsSdkInfoWindow);
            ImGui::MenuItem("IAM Roles", nullptr, &gShowIamRolesWindow);
            ImGui::MenuItem("CloudWatch Log Groups", nullptr, &gShowLogGroupsWindow);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Theme")) {
            if (ImGui::MenuItem("Light")) {
                ImGui::StyleColorsLight();
            }
            if (ImGui::MenuItem("Dark")) {
                ImGui::StyleColorsDark();
            }
            if (ImGui::MenuItem("Classic")) {
                ImGui::StyleColorsClassic();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (ImGui::Begin("Aws Credentials")) {
        ImAws::RegionCombo("##Region", &gAwsRegion, ImGuiComboFlags_WidthFitPreview);
        ImGui::SameLine();
        ImAws::InputCredentials(&gAwsCredentials);
    }
    ImGui::End();

    if (gShowAwsSdkInfoWindow) {
        if (ImGui::Begin("AWS SDK Info", &gShowAwsSdkInfoWindow)) {
            ImGui::Text("AWS SDK Version: %s", Aws::Version::GetVersionString());
            ImGui::Text("Compiler: %s", Aws::Version::GetCompilerVersionString());
            ImGui::Text("C++ Standard: %s", Aws::Version::GetCPPStandard());
        }
        ImGui::End();
    }

    for (auto& session : gSessions) {
        session->draw();
    }

    if (gShowIamRolesWindow) {
        if (ImGui::Begin("IAM", &gShowIamRolesWindow)) {
            gIamRolesPanel.render();
        }
        ImGui::End();
    }

    if (gShowLogGroupsWindow) {
        if (ImGui::Begin("CloudWatch Log Groups", &gShowLogGroupsWindow)) {
            gLogGroupsPanel.render();
        }
        ImGui::End();
    }

    if (gShowDemoWindow) {
        ImGui::ShowDemoWindow(&gShowDemoWindow);
    }

    if (gShowPlotDemoWindow) {
        ImPlot::ShowDemoWindow(&gShowPlotDemoWindow);
    }

    if (gShowPlot3dDemoWindow) {
        ImPlot3D::ShowDemoWindow(&gShowPlot3dDemoWindow);
    }

    sm::Platform::end();
}

int main(int argc, char **argv) {
    sm::PlatformCreateInfo createInfo {
        .title = "AWS C++ WASM",
        .clear = {0.45f, 0.55f, 0.60f, 1.00f}
    };

    if (int err = sm::Platform::setup(createInfo)) {
        return err;
    }

    sm::initAwsByoCrypto();

    Aws::SDKOptions options;
    sm::Platform::configureAwsSdkOptions(options);

    Aws::InitAPI(options);

    {
        gSessions.push_back(std::make_unique<ImAws::Session>(1));
        sm::Platform::run(loop);
    }

    Aws::ShutdownAPI(options);

    sm::Platform::finalize();

    return 0;
}
