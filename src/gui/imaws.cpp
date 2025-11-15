#include "imaws.hpp"
#include "util/defer.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include <aws/core/Region.h>

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

static constexpr size_t kDefaultRegionIndex = std::distance(std::begin(kRegionNames), std::find(std::begin(kRegionNames), std::end(kRegionNames), Aws::Region::US_EAST_1));

void ImAws::RegionCombo(const char *label, AwsRegion *region, ImGuiComboFlags flags) {
    if (ImGui::BeginCombo(label, kRegionNames[region->mIndex], flags)) {
        for (size_t i = 0; i < std::size(kRegionNames); ++i) {
            const bool isSelected = (region->mIndex == i);
            if (ImGui::Selectable(kRegionNames[i], isSelected)) {
                region->mIndex = i;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

ImAws::AwsRegion::AwsRegion()
    : mIndex(kDefaultRegionIndex)
{ }

std::string ImAws::AwsRegion::getSelectedRegionId() const {
    return kRegionNames[mIndex];
}

void ImAws::ArnTooltip(const Aws::String& arn) {
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

namespace {
    std::string escapeText(std::string_view text) {
        std::string escaped;
        for (char c : text) {
            switch (c) {
            case '\n':
                escaped += "\\n";
                break;
            case '\r':
                escaped += "\\r";
                break;
            case '\t':
                escaped += "\\t";
                break;
            case '\"':
                escaped += "\\\"";
                break;
            case '\\':
                escaped += "\\\\";
                break;
            default:
                escaped += c;
                break;
            }
        }
        return escaped;
    }
}

void ImAws::detail::ApiErrorTooltipImpl(
    const char *xAmzRequestId,
    int httpStatusCode,
    int errorType,
    const char *exceptionName,
    const char *message,
    const Aws::Http::HeaderValueCollection& headers,
    const Aws::String& xmlPayload,
    const Aws::String& jsonPayload
) {
    ImVec4 errorColour{1.0f, 0.0f, 0.0f, 1.0f};

    float width = ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x * 2;
    ImGui::PushTextWrapPos(width);
    ImGui::TextColored(errorColour, "%s: %s", exceptionName, message);
    ImGui::PopTextWrapPos();

    if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {

        std::stringstream ss;
        ss << std::format("[{}]\n", exceptionName);
        ss << std::format("status = {}\n", httpStatusCode);
        ss << std::format("type = {}\n", errorType);
        ss << std::format("message = \"{}\"\n", escapeText(message));
        ss << "headers = {\n";
        for (auto it = headers.begin(); it != headers.end(); ++it) {
            const auto& header = *it;
            ss << std::format("  {} = \"{}\"", header.first, escapeText(header.second));
            if (std::next(it) != headers.end()) {
                ss << ",";
            }
            ss << "\n";
        }
        ss << "}\n";

        if (!xmlPayload.empty()) {
            ss << std::format("xml = \"{}\"\n", escapeText(xmlPayload));
        }

        if (!jsonPayload.empty()) {
            ss << std::format("json = \"{}\"\n", escapeText(jsonPayload));
        }

        ImGui::SetClipboardText(ss.str().c_str());
    }

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip)) {
        ImGui::SetNextWindowSize(ImVec2(width, 0), ImGuiCond_Appearing);
        if (ImGui::BeginTooltipEx(ImGuiTooltipFlags_None, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("X-Amz-RequestId: %s", xAmzRequestId);
            ImGui::Text("HTTP Status Code: %d", httpStatusCode);
            ImGui::Text("Error Type: %d", errorType);

            if (ImGui::BeginTable("Response Headers", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Header");
                ImGui::TableSetupColumn("Value");
                ImGui::TableHeadersRow();

                for (const auto& header : headers) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(header.first.c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(header.second.c_str());
                }

                ImGui::EndTable();
            }

            if (xmlPayload.empty() && jsonPayload.empty()) {
                ImGui::TextUnformatted("No error payload");
            } else if (!xmlPayload.empty()) {
                ImGui::SeparatorText("XML");
                ImGui::TextWrapped("%s", xmlPayload.c_str());
            } else if (!jsonPayload.empty()) {
                ImGui::SeparatorText("JSON");
                ImGui::TextWrapped("%s", jsonPayload.c_str());
            }

            ImGui::EndTooltip();
        }
    }
}


void ImAws::InputCredentials(AwsCredentialState *credentials) {
    static constexpr const char *kCredentialTypeNames[AwsCredentialState::eCredentialType_Count] = {
        [AwsCredentialState::eCredentialType_AccessKey] = "Access Key",
        [AwsCredentialState::eCredentialType_SessionToken] = "Session Token",
    };

    if (ImGui::BeginCombo("##Credentials", kCredentialTypeNames[credentials->mType], ImGuiComboFlags_WidthFitPreview)) {
        for (int i = 0; i < AwsCredentialState::eCredentialType_Count; ++i) {
            const bool isSelected = (credentials->mType == i);

            if (ImGui::Selectable(kCredentialTypeNames[i], isSelected)) {
                credentials->mType = i;
            }

            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    ImGui::Button("Show Secrets");
    bool isActive = ImGui::IsItemActive();
    ImGuiInputTextFlags flags = isActive ? ImGuiInputTextFlags_None : ImGuiInputTextFlags_Password;

    ImGui::InputTextWithHint("Access Key ID", "access key", &credentials->mAccessKeyId);
    ImGui::InputTextWithHint("Secret Key", "secret key", &credentials->mSecretKey, flags);

    switch (credentials->mType) {
    case AwsCredentialState::eCredentialType_AccessKey:
        credentials->mSessionToken.clear();
        break;
    case AwsCredentialState::eCredentialType_SessionToken:
        ImGui::InputTextWithHint("Session Token", "session token", &credentials->mSessionToken, flags);
        break;
    default:
        break;
    }
}

ImAws::AwsCredentialState::AwsCredentialState()
    : mType(eCredentialType_AccessKey)
{ }

std::shared_ptr<Aws::Auth::AWSCredentialsProvider> ImAws::AwsCredentialState::createProvider() const {
    return Aws::MakeShared<Aws::Auth::SimpleAWSCredentialsProvider>("ImAws::AwsCredentialState",
        mAccessKeyId.c_str(),
        mSecretKey.c_str(),
        mSessionToken.c_str()
    );
}
