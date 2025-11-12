project('imgui', 'cpp')

src = files(
    'imgui_demo.cpp',
    'imgui_draw.cpp',
    'imgui_tables.cpp',
    'imgui_widgets.cpp',
    'imgui.cpp',
    'misc/cpp/imgui_stdlib.cpp',
)

inc = include_directories('backends', '.')

libimgui = library('imgui', src,
    include_directories: inc,
    install: true
)

imgui_dep = declare_dependency(
    include_directories: inc,
    link_with: libimgui
)

meson.override_dependency('imgui', imgui_dep)

if host_machine.system() == 'emscripten'
    src = files('backends/imgui_impl_glfw.cpp')
    libimgui_impl_glfw = library('imgui_impl_glfw', src,
        dependencies: imgui_dep,
        include_directories: inc
    )
    imgui_impl_glfw_dep = declare_dependency(
        include_directories: inc,
        link_with: libimgui_impl_glfw
    )
    meson.override_dependency('imgui-backend-glfw', imgui_impl_glfw_dep)

    src = files('backends/imgui_impl_opengl3.cpp')
    libimgui_impl_opengl3 = library('imgui_impl_opengl3', src,
        dependencies: imgui_dep,
        include_directories: inc
    )
    imgui_impl_opengl3_dep = declare_dependency(
        include_directories: inc,
        link_with: libimgui_impl_opengl3
    )
    meson.override_dependency('imgui-backend-opengl3', imgui_impl_opengl3_dep)
endif
