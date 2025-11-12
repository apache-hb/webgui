#include "emscripten.hpp"
#include "implot3d.h"

#include <emscripten.h>

#include <stdio.h>

#define GLFW_INCLUDE_ES3
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <implot.h>
#include <implot3d.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <emscripten_browser_clipboard.h>

using sm::Platform_Emscripten;

namespace clipboard = emscripten_browser_clipboard;

namespace {
    GLFWwindow *gWindow{};
    std::array<float, 4> gClearColour{};
    std::string gClipboardContent;

    int init_glfw(const char *title) {
        if (!glfwInit()) {
            fprintf(stderr, "Failed to initialize GLFW\n");
            return 1;
        }

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

        // Open a window and create its OpenGL context.
        // The window is created with minimal size,
        // which will be updated with an automatic resize.
        // You could get the primary viewport size here to create.
        gWindow = glfwCreateWindow(1, 1, title, NULL, NULL);
        if (gWindow == NULL) {
            fprintf(stderr, "Failed to open GLFW window.\n");
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(gWindow); // Initialize GLEW

        return 0;
    }

    void init_imgui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImPlot3D::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(gWindow, true);
        ImGui_ImplGlfw_InstallEmscriptenCallbacks(gWindow, "#canvas");
        ImGui_ImplOpenGL3_Init("#version 300 es");

        // Setup style
        bool isDarkMode = EM_ASM_INT({
            if (window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches) {
                return 1;
            }
            return 0;
        });

        if (isDarkMode) {
            ImGui::StyleColorsDark();
        } else {
            ImGui::StyleColorsLight();
        }

        ImGuiIO &io = ImGui::GetIO();

        io.IniFilename = "/storage/imgui.ini";
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        // Load Fonts
        io.Fonts->AddFontDefault();
    }

    const char *get_clipboard_imgui_adapter(ImGuiContext *ctx) {
        return gClipboardContent.c_str();
    }

    void set_clipboard_imgui_adapter(ImGuiContext *ctx, const char *text) {
        gClipboardContent = text;
        clipboard::copy(gClipboardContent);
    }

    void init_clipboard() {
        clipboard::paste([](std::string&& pasted, [[maybe_unused]] void *user) {
            gClipboardContent = std::move(pasted);
        }, nullptr);

        ImGuiPlatformIO& pio = ImGui::GetPlatformIO();
        pio.Platform_GetClipboardTextFn = get_clipboard_imgui_adapter;
        pio.Platform_SetClipboardTextFn = set_clipboard_imgui_adapter;
    }

    EM_JS(bool, has_storage_mounted, (), {
        return !!Module.storage;
    });
}

int Platform_Emscripten::setup(const PlatformCreateInfo& createInfo) {
    EM_ASM(
        FS.mkdir('/storage');
        FS.mount(IDBFS, {autoPersist: true}, '/storage');

        FS.syncfs(true, function (err) {
            Module.storage = true;
        });
    );

    if (int res = init_glfw(createInfo.title.c_str())) {
        return res;
    }

    // We don't want to wait forever, so limit to 5 attempts
    size_t limit = 5;
    while (!has_storage_mounted() && limit-- > 0) {
        printf("Waiting for filesystem to be mounted...\n");
        // Wait for the filesystem to be mounted
        emscripten_sleep(10);
    }

    init_imgui();
    init_clipboard();

    gClearColour = createInfo.clear;

    return 0;
}

void Platform_Emscripten::finalize() {
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

void Platform_Emscripten::run(void (*fn)()) {
    emscripten_set_main_loop(fn, 0, 1);
}

void Platform_Emscripten::begin() {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Platform_Emscripten::end() {
    ImGui::Render();

    int width{};
    int height{};

    glfwMakeContextCurrent(gWindow);
    glfwGetFramebufferSize(gWindow, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(gClearColour[0], gClearColour[1], gClearColour[2], gClearColour[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwMakeContextCurrent(gWindow);
}
