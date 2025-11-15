#include "window.hpp"

#include <imgui.h>

ImAws::IWindow::IWindow(Session *session, std::string title)
    : mSession(session)
    , mTitle(std::move(title))
    , mIsOpen(true)
{ }

void ImAws::IWindow::setTitle(std::string title) {
    mTitle = std::move(title);
}

bool ImAws::IWindow::drawWindow() {
    if (mIsOpen) {
        if (ImGui::Begin(mTitle.c_str(), &mIsOpen)) {
            draw();
        }

        ImGui::End();
    }

    return mIsOpen;
}

bool ImAws::WindowMenuItem(IWindow& window) {
    return ImGui::MenuItem(window.mTitle.c_str(), nullptr, &window.mIsOpen);
}
