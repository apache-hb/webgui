#include "session.hpp"
#include "gui/aws/windows/cwl.hpp"
#include "gui/aws/windows/iam.hpp"
#include "gui/aws/windows/monitoring.hpp"
#include "window.hpp"

#include "gui/imaws.hpp"

#include <imgui.h>

#include <aws/sts/STSClient.h>

ImAws::Session::Session(SessionInfo info)
    : mInfo(std::move(info))
{ }

void ImAws::Session::drawSessionInfo() {
    if (auto _ = ImAws::Begin(mInfo.title.c_str())) {
        ImGui::Text("Account: %s", mInfo.callerIdentity.GetAccount().c_str());
        ImGui::Text("UserId: %s", mInfo.callerIdentity.GetUserId().c_str());
        ImGui::Text("ARN: %s", mInfo.callerIdentity.GetArn().c_str());

        if (ImGui::Button("CloudWatch Logs")) {
            mWindows.push_back(std::make_unique<ImAws::CloudWatchLogsPanel>(this, "CloudWatch Logs"));
        }

        if (ImGui::Button("CloudWatch Monitoring")) {
            mWindows.push_back(std::make_unique<ImAws::MonitoringPanel>(this, "CloudWatch Monitoring"));
        }

        if (ImGui::Button("IAM")) {
            mWindows.push_back(std::make_unique<ImAws::IamPanel>(this, "IAM"));
        }
    }
}

void ImAws::Session::draw() {
    drawSessionInfo();

    for (auto& window : mWindows) {
        window->drawWindow();
    }
}

void ImAws::Session::drawMenu() {
    ImGui::SeparatorText(mInfo.title.c_str());
    for (auto& window : mWindows) {
        ImAws::WindowMenuItem(*window);
    }
}
