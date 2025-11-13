#pragma once

#include <cstddef>
#include <string>

#include <imgui.h>

#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/core/client/AWSError.h>

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

// -- Implementation --

namespace ImAws {
    namespace detail {
        void ApiErrorTooltipImpl(
            const char *xAmzRequestId,
            int httpStatusCode,
            int errorType,
            const char *exceptionName,
            const char *message,
            const Aws::Http::HeaderValueCollection& headers,
            const Aws::String& xmlPayload,
            const Aws::String& jsonPayload
        );
    }

    template<typename T>
    void ApiErrorTooltip(const Aws::Client::AWSError<T>& error) {
        //
        // Slight bodge to gain access to protected members
        //
        using ErrorType = Aws::Client::AWSError<T>;
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

        Aws::String xmlPayload;
        Aws::String jsonPayload;
        switch (wrapperError.GetErrorPayloadType()) {
        case Aws::Client::ErrorPayloadType::XML:
            xmlPayload = wrapperError.GetXmlPayload().ConvertToString();
            break;
        case Aws::Client::ErrorPayloadType::JSON:
            jsonPayload = wrapperError.GetJsonPayload().View().WriteReadable();
            break;
        default:
            break;
        }

        detail::ApiErrorTooltipImpl(
            wrapperError.GetRequestId().c_str(),
            static_cast<int>(wrapperError.GetResponseCode()),
            static_cast<int>(wrapperError.GetErrorType()),
            wrapperError.GetExceptionName().c_str(),
            wrapperError.GetMessage().c_str(),
            wrapperError.GetResponseHeaders(),
            xmlPayload,
            jsonPayload
        );
    }
}
