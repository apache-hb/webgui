#include "create_session_panel.hpp"

#include "gui/imaws.hpp"
#include "util/async.hpp"

#include <imgui.h>

#include <aws/sts/STSClient.h>
#include <aws/sts/model/GetCallerIdentityResult.h>

using Aws::STS::Model::GetCallerIdentityOutcome;

namespace {
    class CreateSessionPanel_Default final : public sm::ICreateSessionPanel {
        ImAws::AwsRegion mRegion;
        ImAws::AwsCredentialState mCredentials;
        sm::AsyncAction<GetCallerIdentityOutcome> mCallerIdentity;
        std::string mSessionTitle;

    public:
        CreateSessionPanel_Default() { }

        std::string_view getName() const override {
            return "Default";
        }

        std::shared_ptr<Aws::Auth::AWSCredentialsProvider> getCredentialsProvider() const override {
            return mCredentials.createProvider();
        }

        bool render() override {
            ImAws::InputCredentials(&mCredentials);
            return true;
        }
    };
}

std::unique_ptr<sm::ICreateSessionPanel> sm::CreateSessionPanel_ForDefault() {
    return std::make_unique<CreateSessionPanel_Default>();
}
