#pragma once

#include <vector>
#include <memory>

#include "util/async.hpp"

#include "gui/imaws.hpp"

#include <aws/sts/STSServiceClientModel.h>

namespace ImAws {
    class IWindow;

    class Session {
        std::vector<std::unique_ptr<IWindow>> mWindows;

        AwsRegion mRegion;
        AwsCredentialState mCredentials;
        bool mLoggedIn;
        std::string mSessionTitle;
        sm::AsyncAction<Aws::STS::Model::GetCallerIdentityOutcome> mCallerIdentity;

        friend void SessionMenu(Session& session);

    public:
        Session(int id);

        void draw();
    };

    void SessionMenu(Session& session);
}
