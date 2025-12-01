#pragma once

#include "gui/imaws.hpp"
#include <memory>
#include <vector>

#include <aws/core/client/AWSError.h>
#include <aws/core/utils/xml/XmlSerializer.h>

#include <imgui.h>

namespace sm {
    namespace detail {
        class IGenericError {
        public:
            virtual ~IGenericError() = default;

            virtual const std::string& getExceptionName() const = 0;
            virtual const std::string& getMessage() const = 0;
            virtual const std::string& getRequestId() const = 0;
            virtual const std::string& getRemoteHostIpAddress() const = 0;
            virtual int getResponseCode() const = 0;
            virtual int getErrorType() const = 0;
            virtual const Aws::Http::HeaderValueCollection& getHttpHeaders() const = 0;
            virtual const ImAws::PayloadBody& getPayload() const = 0;
        };

        class AwsError final : public IGenericError {
            Aws::String mExceptionName;
            Aws::String mMessage;
            Aws::String mRequestId;
            Aws::String mRemoteHostIpAddress;
            int mResponseCode;
            int mErrorType;
            Aws::Http::HeaderValueCollection mHttpHeaders;
            ImAws::PayloadBody mPayload;

        public:
            template<typename E>
            AwsError(Aws::Client::AWSError<E> error)
                : mExceptionName(error.GetExceptionName())
                , mMessage(error.GetMessage())
                , mRequestId(error.GetRequestId())
                , mRemoteHostIpAddress(error.GetRemoteHostIpAddress())
                , mResponseCode(static_cast<int>(error.GetResponseCode()))
                , mErrorType(static_cast<int>(error.GetErrorType()))
                , mHttpHeaders(error.GetResponseHeaders())
                , mPayload(ImAws::detail::extractPayload(error))
            { }

            const std::string& getExceptionName() const override {
                return mExceptionName;
            }

            const std::string& getMessage() const override {
                return mMessage;
            }

            const std::string& getRequestId() const override {
                return mRequestId;
            }

            const std::string& getRemoteHostIpAddress() const override {
                return mRemoteHostIpAddress;
            }

            int getResponseCode() const override {
                return mResponseCode;
            }

            int getErrorType() const override {
                return mErrorType;
            }

            const Aws::Http::HeaderValueCollection& getHttpHeaders() const override {
                return mHttpHeaders;
            }

            const ImAws::PayloadBody& getPayload() const override {
                return mPayload;
            }
        };
    }

    class ErrorPanel {
        std::vector<std::unique_ptr<detail::IGenericError>> mErrors;
        size_t mSelectedErrorIndex = 0;

        void addGenericError(std::unique_ptr<detail::IGenericError> error) {
            mErrors.emplace_back(std::move(error));
        }

    public:
        ErrorPanel() = default;

        void draw() {
            if (mErrors.empty()) {
                return;
            }

            ImGui::SameLine();
            bool clear = false;
            if (ImGui::Button("Clear")) {
                clear = true;
            }

            ImGui::SameLine();

            ImGuiComboFlags flags =  ImGuiComboFlags_NoPreview;
            if (ImGui::BeginCombo("##Errors", nullptr, flags)) {
                for (size_t i = 0; i < mErrors.size(); ++i) {
                    const auto& error = mErrors[i];
                    auto err = std::format("{}#{}", error->getExceptionName(), i);
                    if (ImGui::Selectable(err.c_str(), mSelectedErrorIndex == i)) {
                        mSelectedErrorIndex = i;
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            const auto& selectedError = mErrors[mSelectedErrorIndex];

            ImAws::detail::ApiErrorTooltipImpl(
                /*xAmzRequestId=*/ selectedError->getRequestId().c_str(),
                /*httpStatusCode=*/ selectedError->getResponseCode(),
                /*errorType=*/ selectedError->getErrorType(),
                /*exceptionName=*/ selectedError->getExceptionName().c_str(),
                /*message=*/ selectedError->getMessage().c_str(),
                /*headers=*/ selectedError->getHttpHeaders(),
                /*payload=*/ selectedError->getPayload()
            );

            if (clear) {
                mErrors.clear();
                mSelectedErrorIndex = 0;
            }
        }

        template<typename E>
        void addError(Aws::Client::AWSError<E> error) {
            addGenericError(std::make_unique<detail::AwsError>(error));
        }
    };
}
