#include <stdio.h>

#include "aws/sts/STSClient.h"
#include "aws/sts/model/GetCallerIdentityResult.h"
#include "gui/aws/session.hpp"
#include "gui/aws/window.hpp"
#include "platform/platform.hpp"
#include "platform/aws.hpp"
#include "gui/imaws.hpp"
#include "util/async.hpp"

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

namespace {
    std::vector<std::unique_ptr<ImAws::Session>> gSessions;
}

static bool gShowDemoWindow = true;
static bool gShowPlotDemoWindow = false;
static bool gShowPlot3dDemoWindow = false;
static bool gShowAwsSdkInfoWindow = false;

using Aws::STS::Model::GetCallerIdentityOutcome;
using Aws::STS::Model::GetCallerIdentityResult;

class CreateSessionWindow {
    ImAws::AwsRegion mRegion;
    ImAws::AwsCredentialState mCredentials;
    sm::AsyncAction<GetCallerIdentityOutcome> mCallerIdentity;
    bool mLoggedIn{false};
    std::string mSessionTitle;

public:
    bool render() {
        if (mCallerIdentity.isComplete()) {
            auto outcome = mCallerIdentity.getResult();
            if (outcome.IsSuccess()) {
                mLoggedIn = true;
                auto result = outcome.GetResult();
                mSessionTitle = std::format("Session - {}", result.GetArn());
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

        if (ImGui::Begin("Create AWS Session")) {
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

        return mLoggedIn;
    }

    std::string getSessionTitle() const {
        return mSessionTitle;
    }

    GetCallerIdentityResult getCallerIdentity() const {
        return mCallerIdentity.getResult().GetResult();
    }

    std::string getSelectedRegionId() const {
        return mRegion.getSelectedRegionId();
    }

    std::shared_ptr<Aws::Auth::AWSCredentialsProvider> createProvider() const {
        return mCredentials.createProvider();
    }

    void reset() {
        mLoggedIn = false;
        mCallerIdentity.clear();
        mSessionTitle.clear();
    }
};

static CreateSessionWindow gCreateSessionWindow;

void loop() {
    if (!sm::Platform::begin()) {
        return;
    }

    ImGui::DockSpaceOverViewport();

    if (ImGui::BeginMainMenuBar()) {
        ImGui::TextUnformatted("AWS C++ WASM Example");
        ImGui::Separator();

        if (ImGui::BeginMenu("Sessions")) {
            for (auto& session : gSessions) {
                session->drawMenu();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows")) {
            ImGui::MenuItem("ImGui Demo Window", nullptr, &gShowDemoWindow);
            ImGui::MenuItem("ImPlot Demo Window", nullptr, &gShowPlotDemoWindow);
            ImGui::MenuItem("ImPlot3D Demo Window", nullptr, &gShowPlot3dDemoWindow);
            ImGui::MenuItem("AWS SDK Info", nullptr, &gShowAwsSdkInfoWindow);
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

    if (gCreateSessionWindow.render()) {
        ImAws::SessionInfo info {
            .title = gCreateSessionWindow.getSessionTitle(),
            .callerIdentity = gCreateSessionWindow.getCallerIdentity(),
            .region = gCreateSessionWindow.getSelectedRegionId(),
            .provider = gCreateSessionWindow.createProvider(),
        };

        gCreateSessionWindow.reset();

        gSessions.push_back(std::make_unique<ImAws::Session>(std::move(info)));
    }

    if (gShowAwsSdkInfoWindow) {
        if (auto _ = ImAws::Begin("AWS SDK Info", &gShowAwsSdkInfoWindow)) {
            ImGui::Text("AWS SDK Version: %s", Aws::Version::GetVersionString());
            ImGui::Text("Compiler: %s", Aws::Version::GetCompilerVersionString());
            ImGui::Text("C++ Standard: %s", Aws::Version::GetCPPStandard());
        }
    }

    for (auto& session : gSessions) {
        session->draw();
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
        sm::Platform::run(loop);
    }

    Aws::ShutdownAPI(options);

    sm::Platform::finalize();

    return 0;
}
