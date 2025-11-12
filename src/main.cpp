#include <thread>
#include <chrono>
#include <stdio.h>

#include "aws/core/client/AWSError.h"
#include "imgui_internal.h"
#include "platform/platform.hpp"
#include "platform/aws.hpp"
#include "util/defer.hpp"

#include <concurrentqueue.h>

#include <imgui.h>
#include <implot.h>
#include <implot3d.h>
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
static bool gShowPlot3dDemoWindow = true;

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

    void ArnTooltip(const Aws::String& arn) {
        ImGui::TextUnformatted(arn.c_str());

        if (ImGui::BeginItemTooltip()) {
            defer { ImGui::EndTooltip(); };
            ImGui::TextUnformatted("Amazon Resource Name (ARN)");

            size_t arnEndIndex = arn.find(':', 0);
            if (arnEndIndex == Aws::String::npos) {
                ImGui::TextUnformatted("Invalid ARN");
                return;
            }

            size_t partitionEndIndex = arn.find(':', arnEndIndex + 1);
            if (partitionEndIndex == Aws::String::npos) {
                ImGui::TextUnformatted("Invalid ARN");
                return;
            }

            size_t serviceEndIndex = arn.find(':', partitionEndIndex + 1);
            if (serviceEndIndex == Aws::String::npos) {
                ImGui::TextUnformatted("Invalid ARN");
                return;
            }

            size_t regionEndIndex = arn.find(':', serviceEndIndex + 1);
            if (regionEndIndex == Aws::String::npos) {
                ImGui::TextUnformatted("Invalid ARN");
                return;
            }

            size_t accountIdEndIndex = arn.find(':', regionEndIndex + 1);
            if (accountIdEndIndex == Aws::String::npos) {
                ImGui::TextUnformatted("Invalid ARN");
                return;
            }

            bool hasResoureType = false;

            size_t resourceTypeEndIndex = arn.find_first_of("/:", accountIdEndIndex + 1);
            if (resourceTypeEndIndex == Aws::String::npos) {
                resourceTypeEndIndex = arn.length();

                //
                // Assume no resource type
                //
            } else {
                //
                // Resource type present
                //

                size_t nextCharIndex = resourceTypeEndIndex + 1;
                if (nextCharIndex >= arn.length()) {
                    ImGui::TextUnformatted("Invalid ARN");
                    return;
                }

                hasResoureType = true;
            }

            std::string_view arnView{arn};
            std::string_view partition = arnView.substr(arnEndIndex + 1, partitionEndIndex - arnEndIndex - 1);
            std::string_view service = arnView.substr(partitionEndIndex + 1, serviceEndIndex - partitionEndIndex - 1);
            std::string_view region = arnView.substr(serviceEndIndex + 1, regionEndIndex - serviceEndIndex - 1);
            std::string_view accountId = arnView.substr(regionEndIndex + 1, accountIdEndIndex - regionEndIndex - 1);

            ImGui::BulletText("Partition: %.*s", (int)partition.length(), partition.data());
            ImGui::BulletText("Service: %.*s", (int)service.length(), service.data());

            if (region.empty()) {
                ImGui::BulletText("Region: global");
            } else {
                ImGui::BulletText("Region: %.*s", (int)region.length(), region.data());
            }

            if (accountId.empty()) {
                ImGui::BulletText("Account ID: unassociated");
            } else {
                ImGui::BulletText("Account ID: %.*s", (int)accountId.length(), accountId.data());
            }

            if (hasResoureType) {
                ImGui::BulletText("Resource Type: %.*s", static_cast<int>(resourceTypeEndIndex - accountIdEndIndex - 1), arn.c_str() + accountIdEndIndex + 1);
            }
            ImGui::BulletText("Resource ID: %s", arn.c_str() + resourceTypeEndIndex + 1);
        }
    }

    template<typename T>
    void ApiErrorTooltip(const Aws::Client::AWSError<T>& error) {

        // Slight bodge to gain access to protected members
        using ErrorType = Aws::Client::AWSError<T>;
        class WrapperError : public ErrorType {
        public:
            WrapperError(const ErrorType& rhs)
                : ErrorType(rhs)
            {}

            using ErrorType::GetErrorPayloadType;
            using ErrorType::GetXmlPayload;
            using ErrorType::GetJsonPayload;
        };

        WrapperError wrapperError{error};

        ImVec4 errorColour{1.0f, 0.0f, 0.0f, 1.0f};

        float width = ImGui::GetCurrentWindow()->Size.x - ImGui::GetStyle().WindowPadding.x * 2;
        ImGui::PushTextWrapPos(width);
        ImGui::TextColored(errorColour, "%s: %s", error.GetExceptionName().c_str(), error.GetMessage().c_str());
        ImGui::PopTextWrapPos();

        if (ImGui::BeginItemTooltip()) {
            ImGui::Text("X-Amz-Request-ID: %s", error.GetRequestId().c_str());
            ImGui::Text("HTTP Status Code: %d", static_cast<int>(error.GetResponseCode()));
            ImGui::Text("Error Type: %d", static_cast<int>(error.GetErrorType()));

            if (ImGui::BeginTable("Response Headers", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Header");
                ImGui::TableSetupColumn("Value");
                ImGui::TableHeadersRow();
                for (const auto& header : error.GetResponseHeaders()) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(header.first.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(header.second.c_str());
                }
                ImGui::EndTable();
            }

            auto payloadType = wrapperError.GetErrorPayloadType();
            if (payloadType == Aws::Client::ErrorPayloadType::NOT_SET) {
                ImGui::TextUnformatted("No error payload");
            } else if (payloadType == Aws::Client::ErrorPayloadType::XML) {
                ImGui::TextUnformatted("XML Payload");
                auto doc = wrapperError.GetXmlPayload();
                auto xmlString = doc.ConvertToString();
                ImGui::TextWrapped("%s", xmlString.c_str());
            } else if (payloadType == Aws::Client::ErrorPayloadType::JSON) {
                ImGui::TextUnformatted("JSON Payload");
                auto json = wrapperError.GetJsonPayload();
                auto jsonString = json.View().WriteReadable();
                ImGui::TextWrapped("%s", jsonString.c_str());
            }

            ImGui::EndTooltip();
        }
    }
}

static std::string gAwsAccessKeyId = "AccessKeyId";
static std::string gAwsSecretKey = "";
static AwsRegion gAwsRegion;

namespace {
    Aws::CloudWatchLogs::CloudWatchLogsClient create_cwl_client() {
        Aws::Auth::AWSCredentials credentials{gAwsAccessKeyId, gAwsSecretKey};
        auto provider = Aws::MakeShared<Aws::Auth::SimpleAWSCredentialsProvider>("AwsAllocationTag", credentials);

        Aws::Client::ClientConfigurationInitValues clientConfigInitValues;
        clientConfigInitValues.shouldDisableIMDS = true;

        Aws::CloudWatchLogs::CloudWatchLogsClientConfiguration config{clientConfigInitValues};
        config.region = gAwsRegion.getSelectedRegionId();

        return Aws::CloudWatchLogs::CloudWatchLogsClient{provider, config};
    }

    std::string unix_epoch_ms_to_datetime_string(long long epochMs) {
        std::chrono::system_clock::time_point tp{std::chrono::milliseconds{epochMs}};
        return std::format("{0:%Y}-{0:%m}-{0:%d}:{0:%H}:{0:%M}:{0:%S}", tp);
    }
}

class LogGroupsPanel {
    using LogGroup = Aws::CloudWatchLogs::Model::LogGroup;
    using CloudWatchLogsError = Aws::CloudWatchLogs::CloudWatchLogsError;

    enum State {
        ePanelState_Idle,
        ePanelState_Fetching,
    };

    struct LogGroupEntry {
        uint32_t generation;
        LogGroup logGroup;
    };

    std::atomic<State> mState{ePanelState_Idle};
    uint32_t mGeneration{0};

    std::vector<LogGroup> mLogGroups;
    std::unique_ptr<std::jthread> mWorkerThread;
    moodycamel::ConcurrentQueue<LogGroupEntry> mLogGroupQueue;

    std::atomic<bool> mHasError{false};
    std::optional<CloudWatchLogsError> mLastError;

    ImGuiTableFlags mTableFlags{ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV};

    void fetch_all_log_groups(std::stop_token stop) {
        mState = ePanelState_Fetching;

        auto generation = ++mGeneration;

        auto cwlClient = create_cwl_client();

        Aws::CloudWatchLogs::Model::DescribeLogGroupsRequest request;
        request.SetLimit(50);

        Aws::String nextToken;
        do {
            if (!nextToken.empty()) {
                request.SetNextToken(nextToken);
            }

            auto outcome = cwlClient.DescribeLogGroups(request);
            if (!outcome.IsSuccess()) {
                mLastError = outcome.GetError();
                mHasError = true;
                break;
            }

            const auto& result = outcome.GetResult();
            for (const auto& logGroup : result.GetLogGroups()) {
                mLogGroupQueue.enqueue({generation, logGroup});
            }

            nextToken = result.GetNextToken();
        } while (!nextToken.empty() && !stop.stop_requested());

        mState = ePanelState_Idle;
    }

public:
    void render() {
        bool isFetching = mState.load() == ePanelState_Fetching;
        ImGui::BeginDisabled(isFetching);
        if (ImGui::Button(isFetching ? "Working..." : "Fetch")) {
            mLogGroups.clear();
            mHasError.store(false);
            mLastError.reset();
            mWorkerThread.reset(new std::jthread([this](std::stop_token stop) {
                fetch_all_log_groups(stop);
            }));
        }
        ImGui::EndDisabled();

        if (mHasError.load()) {
            ImGui::SameLine();
            ImAws::ApiErrorTooltip(mLastError.value());
        }

        LogGroupEntry logGroup;
        if (mLogGroupQueue.try_dequeue(logGroup)) {
            if (logGroup.generation == mGeneration) {
                mLogGroups.push_back(logGroup.logGroup);
            }
        }

        if (ImGui::BeginTable("Log Groups", 3, mTableFlags)) {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("ARN", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Creation Time", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();
            for (const auto& group : mLogGroups) {
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

static LogGroupsPanel gLogGroupsPanel;

void loop() {
    sm::Platform::begin();

    ImGui::DockSpaceOverViewport();

    if (ImGui::BeginMainMenuBar()) {
        ImGui::TextUnformatted("AWS C++ WASM Example");
        ImGui::Separator();

        if (ImGui::BeginMenu("Windows")) {
            ImGui::MenuItem("ImGui Demo Window", nullptr, &gShowDemoWindow);
            ImGui::MenuItem("ImPlot Demo Window", nullptr, &gShowPlotDemoWindow);
            ImGui::MenuItem("ImPlot3D Demo Window", nullptr, &gShowPlot3dDemoWindow);
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
        ImGui::InputText("Access Key ID", &gAwsAccessKeyId);
        ImGui::InputText("Secret Access Key", &gAwsSecretKey, ImGuiInputTextFlags_Password);
        ImAws::RegionCombo("Region", &gAwsRegion);
    }

    ImGui::End();

    if (ImGui::Begin("CloudWatch Log Groups")) {
        gLogGroupsPanel.render();
    }
    ImGui::End();

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

    sm::aws_init_byo_crypto();

    Aws::SDKOptions options;
    sm::Platform::configure_aws_sdk_options(options);

    Aws::InitAPI(options);

    sm::Platform::run(loop);

    sm::Platform::finalize();

    Aws::ShutdownAPI(options);

    return 0;
}
