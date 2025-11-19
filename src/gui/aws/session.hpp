#pragma once

#include <vector>
#include <memory>

#include "aws/core/auth/AWSCredentialsProvider.h"
#include "aws/sts/model/GetCallerIdentityResult.h"

#include <aws/sts/STSServiceClientModel.h>

namespace ImAws {
    class IWindow;

    struct SessionInfo {
        std::string title;
        std::shared_ptr<Aws::Auth::AWSCredentialsProvider> provider;
        std::string region;
        Aws::STS::Model::GetCallerIdentityResult callerIdentity;
    };

    class Session {
        std::vector<std::unique_ptr<IWindow>> mWindows;
        SessionInfo mInfo;

        void drawSessionInfo();

    public:
        Session(SessionInfo info);

        std::shared_ptr<Aws::Auth::AWSCredentialsProvider> getCredentialsProvider() const {
            return mInfo.provider;
        }

        std::string getSelectedRegion() const {
            return mInfo.region;
        }

        void draw();

        void drawMenu();
    };
}
