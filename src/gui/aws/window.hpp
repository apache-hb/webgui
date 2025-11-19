#pragma once

#include "aws/core/auth/AWSCredentials.h"
#include "aws/core/auth/AWSCredentialsProvider.h"
#include <string>

namespace ImAws {
    class Session;

    class IWindow {
        Session *mSession;
        std::string mTitle;
        bool mIsOpen;

        friend bool WindowMenuItem(IWindow& window);

    protected:
        void setTitle(std::string title);

        std::shared_ptr<Aws::Auth::AWSCredentialsProvider> getSessionCredentialsProvider() const;
        std::string getSessionRegion() const;

    public:
        virtual ~IWindow() = default;

        IWindow(Session *session, std::string title);

        virtual void draw() = 0;

        bool drawWindow();
    };

    bool WindowMenuItem(IWindow& window);
}
