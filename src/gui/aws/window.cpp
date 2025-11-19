#include "window.hpp"
#include "gui/aws/session.hpp"
#include "gui/imaws.hpp"

#include <imgui.h>

ImAws::IWindow::IWindow(Session *session, std::string title)
    : mSession(session)
    , mTitle(std::move(title))
    , mIsOpen(true)
{ }

std::shared_ptr<Aws::Auth::AWSCredentialsProvider> ImAws::IWindow::getSessionCredentialsProvider() const {
    return mSession->getCredentialsProvider();
}

std::string ImAws::IWindow::getSessionRegion() const {
    return mSession->getSelectedRegion();
}

void ImAws::IWindow::setTitle(std::string title) {
    mTitle = std::move(title);
}

bool ImAws::IWindow::drawWindow() {
    if (mIsOpen) {
        if (auto _ = ImAws::Begin(mTitle.c_str(), &mIsOpen)) {
            draw();
        }
    }

    return mIsOpen;
}

bool ImAws::WindowMenuItem(IWindow& window) {
    return ImGui::MenuItem(window.mTitle.c_str(), nullptr, &window.mIsOpen);
}
