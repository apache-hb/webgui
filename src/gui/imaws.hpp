#pragma once

#include <cstddef>
#include <string>

#include <imgui.h>

#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/core/client/AWSError.h>
#include <variant>

namespace ImAws {
    class AwsRegion;
    class AwsCredentialState;

    void RegionCombo(const char *label, AwsRegion *region, ImGuiComboFlags flags = ImGuiComboFlags_None);
    void ArnTooltip(const Aws::String& arn);
    void InputCredentials(AwsCredentialState *credentials);

    template<typename T>
    void ApiErrorTooltip(const Aws::Client::AWSError<T>& error);

    class AwsRegion {
        size_t mIndex;
        std::string mCustomRegion;

        friend void RegionCombo(const char *label, AwsRegion *region, ImGuiComboFlags flags);

    public:
        AwsRegion();

        std::string getSelectedRegionId() const;
    };

    class AwsCredentialState {
        enum CredentialType {
            eCredentialType_AccessKey,
            eCredentialType_SessionToken,
            eCredentialType_Count
        };

        int mType;
        std::string mAccessKeyId;
        std::string mSecretKey;
        std::string mSessionToken;

        friend void InputCredentials(AwsCredentialState *credentials);

    public:
        AwsCredentialState();

        std::shared_ptr<Aws::Auth::AWSCredentialsProvider> createProvider() const;
    };
}

namespace ImAws {
    class WindowHandle {
        bool mOpen;
    public:
        WindowHandle(bool open)
            : mOpen(open)
        { }

        operator bool() const {
            return mOpen;
        }

        ~WindowHandle() {
            ImGui::End();
        }
    };

    WindowHandle Begin(const char *name, bool *p_open = nullptr, ImGuiWindowFlags flags = 0);
}
// -- Implementation --

namespace ImAws {
    using PayloadBody = std::variant<std::monostate, Aws::Utils::Xml::XmlDocument, Aws::Utils::Json::JsonValue>;

    namespace detail {
        void ApiErrorTooltipImpl(
            std::string_view xAmzRequestId,
            int httpStatusCode,
            int errorType,
            std::string_view exceptionName,
            std::string_view message,
            const Aws::Http::HeaderValueCollection& headers,
            const PayloadBody& payload
        );

        template<typename E>
        ImAws::PayloadBody extractPayload(const Aws::Client::AWSError<E>& error) {
            //
            // Slight bodge to gain access to protected members
            //
            using ErrorType = Aws::Client::AWSError<E>;
            class WrapperError : public ErrorType {
            public:
                WrapperError(const ErrorType& rhs)
                    : ErrorType(rhs)
                { }

                using ErrorType::GetErrorPayloadType;
                using ErrorType::GetXmlPayload;
                using ErrorType::GetJsonPayload;
            };

            WrapperError wrapperError{error};

            switch (wrapperError.GetErrorPayloadType()) {
            case Aws::Client::ErrorPayloadType::XML:
                return wrapperError.GetXmlPayload();
            case Aws::Client::ErrorPayloadType::JSON:
                return wrapperError.GetJsonPayload();
            default:
                return std::monostate{};
            }
        }
    }

    template<typename T>
    void ApiErrorTooltip(const Aws::Client::AWSError<T>& error) {
        auto payload = detail::extractPayload(error);

        detail::ApiErrorTooltipImpl(
            error.GetRequestId().c_str(),
            static_cast<int>(error.GetResponseCode()),
            static_cast<int>(error.GetErrorType()),
            error.GetExceptionName().c_str(),
            error.GetMessage().c_str(),
            error.GetResponseHeaders(),
            payload
        );
    }
}
