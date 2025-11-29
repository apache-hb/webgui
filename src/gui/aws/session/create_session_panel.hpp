#pragma once

#include <aws/core/auth/AWSCredentialsProvider.h>

#include <memory>
#include <string_view>

namespace sm {
    class ICreateSessionPanel {
    public:
        virtual ~ICreateSessionPanel() = default;

        virtual std::string_view getName() const = 0;

        virtual bool isDisabled() const { return false; }
        virtual std::string_view getDisabledReason() const { return ""; }

        virtual std::shared_ptr<Aws::Auth::AWSCredentialsProvider> getCredentialsProvider() const = 0;

        virtual std::vector<std::shared_ptr<Aws::Auth::AWSCredentialsProvider>> getAllCredentialsProviders() const {
            auto provider = getCredentialsProvider();
            if (provider != nullptr) {
                return { provider };
            } else {
                return {};
            }
        }

        virtual bool render() = 0;
    };

    std::unique_ptr<ICreateSessionPanel> CreateSessionPanel_ForConfigFile();
    std::unique_ptr<ICreateSessionPanel> CreateSessionPanel_ForDefault();
}
