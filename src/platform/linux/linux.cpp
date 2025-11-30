#include "linux.hpp"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <implot.h>
#include <implot3d.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using sm::Platform_Linux;

namespace {
    GLFWwindow *gWindow{};
    std::array<float, 4> gClearColour{};
    std::string gClipboardContent;
    bool gShouldClose{false};

    int initGlfw(const char *title) {
        glfwSetErrorCallback([](int error, const char* description) {
            fprintf(stderr, "GLFW Error %d: %s\n", error, description);
        });

        if (!glfwInit()) {
            fprintf(stderr, "Failed to initialize GLFW\n");
            return 1;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        float scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
        if (scale <= 0.0f) {
            scale = 1.0f;
        }

        // Open a window and create its OpenGL context.
        // The window is created with minimal size,
        // which will be updated with an automatic resize.
        // You could get the primary viewport size here to create.
        gWindow = glfwCreateWindow((int)(1280 * scale), (int)(800 * scale), title, NULL, NULL);
        if (gWindow == NULL) {
            fprintf(stderr, "Failed to open GLFW window.\n");
            glfwTerminate();
            return -1;
        }

        glfwSetWindowCloseCallback(gWindow, [](GLFWwindow* window) {
            gShouldClose = true;
        });

        glfwMakeContextCurrent(gWindow); // Initialize GLEW

        return 0;
    }

    void initImGui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImPlot3D::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(gWindow, true);
        ImGui_ImplOpenGL3_Init("#version 130");

        // Setup style
        ImGui::StyleColorsDark();

        ImGuiIO &io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();

        style.ScaleAllSizes(ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()));

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Load Fonts
        io.Fonts->AddFontDefault();
    }
}

int Platform_Linux::setup(const PlatformCreateInfo& createInfo) {
    if (int res = initGlfw(createInfo.title.c_str())) {
        return res;
    }

    initImGui();

    gClearColour = createInfo.clear;

    return 0;
}

void Platform_Linux::finalize() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot3D::DestroyContext();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    if (gWindow != nullptr) {
        glfwDestroyWindow(gWindow);
    }

    glfwTerminate();
}

void Platform_Linux::run(void (*fn)()) {
    while (!gShouldClose) {
        fn();
    }
}

bool Platform_Linux::begin() {
    glfwPollEvents();
    if (glfwGetWindowAttrib(gWindow, GLFW_ICONIFIED) != 0) {
        ImGui_ImplGlfw_Sleep(10);
        return false;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    return true;
}

void Platform_Linux::end() {
    ImGuiIO &io = ImGui::GetIO();
    ImGui::Render();

    int width{};
    int height{};

    glfwMakeContextCurrent(gWindow);
    glfwGetFramebufferSize(gWindow, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(gClearColour[0], gClearColour[1], gClearColour[2], gClearColour[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(gWindow);
}

void Platform_Linux::configureAwsSdkOptions(Aws::SDKOptions& options) {

}
