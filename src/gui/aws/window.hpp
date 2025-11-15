#pragma once

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

    public:
        virtual ~IWindow() = default;

        IWindow(Session *session, std::string title);

        virtual void draw() = 0;

        bool drawWindow();
    };

    bool WindowMenuItem(IWindow& window);
}
