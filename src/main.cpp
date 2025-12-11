#include "gui/aws/session.hpp"
#include "gui/aws/session/create_session_panel.hpp"
#include "gui/aws/window.hpp"
#include "platform/platform.hpp"
#include "platform/aws.hpp"
#include "gui/imaws.hpp"
#include "util/async.hpp"

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

#include <aws/sts/STSClient.h>
#include <aws/sts/model/GetCallerIdentityResult.h>

#include <aws/logs/CloudWatchLogsClient.h>
#include <aws/logs/CloudWatchLogsServiceClientModel.h>
#include <aws/logs/model/DescribeLogGroupsRequest.h>

#include <aws/iam/IAMClient.h>
#include <aws/iam/model/ListRolesRequest.h>
#include <aws/iam/model/ListPoliciesRequest.h>

#include <gui_config.hpp>
#include <sqlite3.h>
#include <s2n.h>

#include <openssl/opensslconf.h>

using Aws::Client::ClientConfigurationInitValues;
using Aws::STS::Model::GetCallerIdentityOutcome;
using Aws::STS::Model::GetCallerIdentityResult;
using Aws::STS::STSClientConfiguration;
using Aws::STS::STSClient;

namespace {
    std::vector<std::unique_ptr<ImAws::Session>> gSessions;
    bool gShowDemoWindow = true;
    bool gShowPlotDemoWindow = false;
    bool gShowPlot3dDemoWindow = false;
    bool gShowAwsSdkInfoWindow = false;
}

class CreateSessionWindow {
    static constexpr char kLoginFailedModal[] = "Login Failed";

    std::vector<std::unique_ptr<sm::ICreateSessionPanel>> mPanels;

    ImAws::AwsRegion mRegion;
    std::shared_ptr<Aws::Auth::AWSCredentialsProvider> mCredentialsProvider;
    sm::AsyncAction<GetCallerIdentityOutcome> mCallerIdentity;
    bool mLoggedIn{false};
    std::string mSessionTitle;

    void drawSinglePanel(sm::ICreateSessionPanel *panel) {
        bool disabled = panel->isDisabled();
        ImGui::BeginDisabled(disabled);

        if (ImGui::BeginTabItem(panel->getName().data())) {
            bool allowLoginAttempt = panel->render();
            bool isWorking = mCallerIdentity.isWorking();

            const char *label = isWorking ? "Logging in..." : "Login";
            ImGui::BeginDisabled(!allowLoginAttempt || isWorking);

            if (ImGui::Button(label)) {
                mCredentialsProvider = panel->getCredentialsProvider();

                mCallerIdentity.run([this] {
                    ClientConfigurationInitValues clientConfigInitValues;
                    clientConfigInitValues.shouldDisableIMDS = true;

                    STSClientConfiguration config{clientConfigInitValues};
                    config.region = mRegion.getSelectedRegionId();

                    STSClient stsClient{mCredentialsProvider, config};
                    return stsClient.GetCallerIdentity({});
                });
            }

            ImGui::EndDisabled();

            ImGui::EndTabItem();
        }

        if (disabled && ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip)) {
            std::string_view reason = panel->getDisabledReason();
            ImGui::SetTooltip("%.*s", static_cast<int>(reason.size()), reason.data());
        }

        ImGui::EndDisabled();
    }

    void loginFailedModal(const GetCallerIdentityOutcome& outcome) {
        ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_FirstUseEver);
        if (ImGui::BeginPopupModal(kLoginFailedModal)) {
            ImAws::ApiErrorTooltip(outcome.GetError());
            if (ImGui::Button("OK")) {
                mCallerIdentity.clear();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

public:
    CreateSessionWindow() = default;

    void init() {
        mPanels.emplace_back(sm::CreateSessionPanel_ForConfigFile());
        mPanels.emplace_back(sm::CreateSessionPanel_ForDefault());
    }

    bool render() {
        if (mCallerIdentity.isComplete()) {
            auto outcome = mCallerIdentity.getResult();
            if (outcome.IsSuccess()) {
                mLoggedIn = true;
                auto result = outcome.GetResult();
                mSessionTitle = std::format("Session - {}", result.GetArn());
            } else {
                ImGui::OpenPopup(kLoginFailedModal);
            }

            loginFailedModal(outcome);
        }

        if (ImGui::Begin("Create AWS Session")) {
            if (ImGui::BeginTabBar("Credential Panels")) {
                for (auto& panel : mPanels) {
                    drawSinglePanel(panel.get());
                }
                ImGui::EndTabBar();
            }
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
        return mCredentialsProvider;
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
        ImGui::TextUnformatted(PROJECT_NAME);
        ImGui::Separator();

        if (ImGui::BeginMenu("Sessions")) {
            if (gSessions.empty()) {
                ImGui::TextUnformatted("No active sessions");
            } else {
                for (auto& session : gSessions) {
                    session->drawMenu();
                }
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
        if (auto _ = ImAws::Begin("Version Info", &gShowAwsSdkInfoWindow)) {
            ImGui::SeparatorText("Application");
            ImGui::Text("Name: %s", PROJECT_NAME);
            ImGui::Text("Version: %s", PROJECT_VERSION);

            ImGui::SeparatorText("sqlite3");
            ImGui::Text("Version: %s", sqlite3_libversion());

            ImGui::SeparatorText("ImGui");
            ImGui::Text("ImGui Version: %s", IMGUI_VERSION);
            ImGui::Text("ImPlot Version: %s", IMPLOT_VERSION);
            ImGui::Text("ImPlot3D Version: %s", IMPLOT3D_VERSION);

            ImGui::SeparatorText("OpenSSL");
            ImGui::Text("Version: %s", OPENSSL_FULL_VERSION_STR);

            ImGui::SeparatorText("AWS C++ SDK");
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
        .title = PROJECT_NAME,
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
        gCreateSessionWindow.init();
        sm::Platform::run(loop);
    }

    Aws::ShutdownAPI(options);

    sm::Platform::finalize();

    return 0;
}
