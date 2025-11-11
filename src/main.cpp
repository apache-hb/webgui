#include <thread>
#include <barrier>
#include <stdio.h>

#include "platform/platform.hpp"
#include "platform/aws.hpp"

#include <imgui.h>
#include <implot.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <aws/core/Region.h>
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

static bool gShowDemoWindow = true;
static bool gShowPlotDemoWindow = true;

static std::unique_ptr<std::jthread> gDescribeLogGroupsThread;
static std::atomic<bool> gDescribeLogGroupsOutcomePresent = false;
static Aws::CloudWatchLogs::Model::DescribeLogGroupsOutcome gDescribeLogGroupsOutcome;

static constexpr const char *kRegionNames[] = {
    Aws::Region::AF_SOUTH_1,
    Aws::Region::AP_EAST_1,
    Aws::Region::AP_EAST_2,
    Aws::Region::AP_NORTHEAST_1,
    Aws::Region::AP_NORTHEAST_2,
    Aws::Region::AP_NORTHEAST_3,
    Aws::Region::AP_SOUTH_1,
    Aws::Region::AP_SOUTH_2,
    Aws::Region::AP_SOUTHEAST_1,
    Aws::Region::AP_SOUTHEAST_2,
    Aws::Region::AP_SOUTHEAST_3,
    Aws::Region::AP_SOUTHEAST_4,
    Aws::Region::AP_SOUTHEAST_5,
    Aws::Region::AP_SOUTHEAST_6,
    Aws::Region::AP_SOUTHEAST_7,
    Aws::Region::AWS_CN_GLOBAL,
    Aws::Region::AWS_GLOBAL,
    Aws::Region::AWS_ISO_B_GLOBAL,
    Aws::Region::AWS_ISO_E_GLOBAL,
    Aws::Region::AWS_ISO_F_GLOBAL,
    Aws::Region::AWS_ISO_GLOBAL,
    Aws::Region::AWS_US_GOV_GLOBAL,
    Aws::Region::CA_CENTRAL_1,
    Aws::Region::CA_WEST_1,
    Aws::Region::CN_NORTH_1,
    Aws::Region::CN_NORTHWEST_1,
    Aws::Region::EU_CENTRAL_1,
    Aws::Region::EU_CENTRAL_2,
    Aws::Region::EU_ISOE_WEST_1,
    Aws::Region::EU_NORTH_1,
    Aws::Region::EU_SOUTH_1,
    Aws::Region::EU_SOUTH_2,
    Aws::Region::EU_WEST_1,
    Aws::Region::EU_WEST_2,
    Aws::Region::EU_WEST_3,
    Aws::Region::EUSC_DE_EAST_1,
    Aws::Region::IL_CENTRAL_1,
    Aws::Region::ME_CENTRAL_1,
    Aws::Region::ME_SOUTH_1,
    Aws::Region::MX_CENTRAL_1,
    Aws::Region::SA_EAST_1,
    Aws::Region::US_EAST_1,
    Aws::Region::US_EAST_2,
    Aws::Region::US_GOV_EAST_1,
    Aws::Region::US_GOV_WEST_1,
    Aws::Region::US_ISO_EAST_1,
    Aws::Region::US_ISO_WEST_1,
    Aws::Region::US_ISOB_EAST_1,
    Aws::Region::US_ISOB_WEST_1,
    Aws::Region::US_ISOF_EAST_1,
    Aws::Region::US_ISOF_SOUTH_1,
    Aws::Region::US_WEST_1,
    Aws::Region::US_WEST_2,
};

class AwsRegion;

namespace ImAws {
    void RegionCombo(const char *label, AwsRegion *region);
}

class AwsRegion {
    static constexpr size_t kDefaultRegionIndex = std::distance(std::begin(kRegionNames), std::find(std::begin(kRegionNames), std::end(kRegionNames), Aws::Region::US_EAST_1));

    size_t index{kDefaultRegionIndex};

    friend void ImAws::RegionCombo(const char *label, AwsRegion *region);

public:
    std::string getSelectedRegionId() const {
        return kRegionNames[index];
    }
};

namespace ImAws {
    void RegionCombo(const char *label, AwsRegion *region) {
        if (ImGui::BeginCombo(label, kRegionNames[region->index])) {
            for (size_t i = 0; i < sizeof(kRegionNames) / sizeof(kRegionNames[0]); ++i) {
                const bool isSelected = (region->index == i);
                if (ImGui::Selectable(kRegionNames[i], isSelected)) {
                    region->index = i;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
}

static std::string gAwsAccessKeyId = "AccessKeyId";
static std::string gAwsSecretKey = "";
static AwsRegion gAwsRegion;

void loop() {
    sm::Platform::begin();

    ImGui::DockSpaceOverViewport();

    if (ImGui::Begin("Aws Credentials")) {
        ImGui::InputText("Access Key ID", &gAwsAccessKeyId);
        ImGui::InputText("Secret Access Key", &gAwsSecretKey, ImGuiInputTextFlags_Password);
        ImAws::RegionCombo("Region", &gAwsRegion);

        if (ImGui::Button("Describe Log Groups")) {
            // Trigger describe log groups
            gDescribeLogGroupsThread = std::make_unique<std::jthread>([
                accessKeyId = gAwsAccessKeyId,
                secretKey = gAwsSecretKey,
                region = gAwsRegion.getSelectedRegionId()
            ] {
                Aws::Auth::AWSCredentials credentials{accessKeyId, secretKey};

                auto provider = Aws::MakeShared<Aws::Auth::SimpleAWSCredentialsProvider>("AwsAllocationTag", credentials);

                Aws::Client::ClientConfigurationInitValues clientConfigInitValues;
                clientConfigInitValues.shouldDisableIMDS = true;

                Aws::CloudWatchLogs::CloudWatchLogsClientConfiguration config{clientConfigInitValues};
                config.region = region;

                Aws::CloudWatchLogs::CloudWatchLogsClient cwlClient{provider, config};

                Aws::CloudWatchLogs::Model::DescribeLogGroupsRequest request;
                request.SetLimit(5);
                gDescribeLogGroupsOutcome = cwlClient.DescribeLogGroups(request);
                gDescribeLogGroupsOutcomePresent = true;
            });
        }

        ImGui::End();
    }

    if (gDescribeLogGroupsOutcomePresent) {
        gDescribeLogGroupsThread.reset();

        if (ImGui::Begin("Describe Log Groups Outcome")) {
            if (gDescribeLogGroupsOutcome.IsSuccess()) {
                const auto& logGroups = gDescribeLogGroupsOutcome.GetResult().GetLogGroups();
                ImGui::Text("Log Groups:");
                for (const auto &logGroup : logGroups) {
                    ImGui::BulletText("%s", logGroup.GetLogGroupName().c_str());
                }
            } else {
                const auto& err = gDescribeLogGroupsOutcome.GetError();
                ImGui::Text("Exception: %s", err.GetExceptionName().c_str());
                ImGui::Text("Request ID: %s", err.GetRequestId().c_str());
                ImGui::Text("Error Message: %s", err.GetMessage().c_str());
            }
            ImGui::End();
        }
    }

    if (ImGui::Begin("ImGui Info")) {
        ImGui::Checkbox("Demo Window", &gShowDemoWindow);
        ImGui::Checkbox("Plot Demo Window", &gShowPlotDemoWindow);
        ImGui::End();
    }

    if (gShowDemoWindow) {
        ImGui::ShowDemoWindow(&gShowDemoWindow);
    }

    if (gShowPlotDemoWindow) {
        ImPlot::ShowDemoWindow(&gShowPlotDemoWindow);
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

    sm::aws_init_byo_crypto();

    Aws::SDKOptions options;
    sm::Platform::configure_aws_sdk_options(options);

    Aws::InitAPI(options);

    std::barrier barrier{2};

    std::thread worker([&barrier]() {
        Aws::Auth::AWSCredentials credentials{
            "Secret", "Secret"};

        auto provider = Aws::MakeShared<Aws::Auth::SimpleAWSCredentialsProvider>(
            "AwsAllocationTag", credentials);

        Aws::Client::ClientConfigurationInitValues clientConfigInitValues;
        clientConfigInitValues.shouldDisableIMDS = true;

        Aws::CloudWatchLogs::CloudWatchLogsClientConfiguration config{
            clientConfigInitValues};
        config.region = "us-east-1";
        Aws::CloudWatchLogs::CloudWatchLogsClient cwlClient{provider, config};

        Aws::CloudWatchLogs::Model::DescribeLogGroupsRequest request;
        request.SetLimit(5);
        gDescribeLogGroupsOutcome = cwlClient.DescribeLogGroups(request);
        barrier.arrive_and_wait();
    });

    barrier.arrive_and_wait();

    sm::Platform::run(loop);

    sm::Platform::finalize();

    Aws::ShutdownAPI(options);

    return 0;
}