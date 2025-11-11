src = files(
    'imgui_demo.cpp',
    'imgui_draw.cpp',
    'imgui_tables.cpp',
    'imgui_widgets.cpp',
    'imgui.cpp'
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

if host_machine.system() == 'emscripten'
    src = files('backends/imgui_impl_glfw.cpp', 'backends/imgui_impl_opengl3.cpp')
    libbackend = library('imgui_backend', src,
        dependencies: imgui_dep,
        include_directories: inc
    )

    backend_dep = declare_dependency(
        include_directories: inc,
        link_with: libbackend
    )
endif
