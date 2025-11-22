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

imgui_impl_glfw_dep = disabler()
imgui_impl_opengl3_dep = disabler()
imgui_impl_win32_dep = disabler()
imgui_impl_dx12_dep = disabler()
imgui_impl_osx_dep = disabler()
imgui_impl_metal_dep = disabler()

if host_machine.system() in ['linux', 'emscripten']
    glfw_dep = dependency('glfw3', required: host_machine.system() == 'linux')

    src = files('backends/imgui_impl_glfw.cpp')
    libimgui_impl_glfw = library('imgui_impl_glfw', src,
        dependencies: [imgui_dep, glfw_dep],
        include_directories: inc,
        build_by_default: false
    )
    imgui_impl_glfw_dep = declare_dependency(
        include_directories: inc,
        link_with: libimgui_impl_glfw,
        dependencies: [glfw_dep],
    )

    src = files('backends/imgui_impl_opengl3.cpp')
    libimgui_impl_opengl3 = library('imgui_impl_opengl3', src,
        dependencies: imgui_dep,
        include_directories: inc,
        build_by_default: false
    )
    imgui_impl_opengl3_dep = declare_dependency(
        include_directories: inc,
        link_with: libimgui_impl_opengl3
    )
elif host_machine.system() == 'windows'
    src = files('backends/imgui_impl_win32.cpp')
    libimgui_impl_win32 = library('imgui_impl_win32', src,
        dependencies: imgui_dep,
        include_directories: inc,
        build_by_default: false
    )
    imgui_impl_win32_dep = declare_dependency(
        include_directories: inc,
        link_with: libimgui_impl_win32
    )

    src = files('backends/imgui_impl_dx12.cpp')
    libimgui_impl_dx12 = library('imgui_impl_dx12', src,
        dependencies: imgui_dep,
        include_directories: inc,
        build_by_default: false
    )
    imgui_impl_dx12_dep = declare_dependency(
        include_directories: inc,
        link_with: libimgui_impl_dx12
    )
elif host_machine.system() == 'darwin'
    src = files('backends/imgui_impl_osx.mm')
    libimgui_impl_osx = library('imgui_impl_osx', src,
        dependencies: imgui_dep,
        include_directories: inc,
        build_by_default: false
    )
    imgui_impl_osx_dep = declare_dependency(
        include_directories: inc,
        link_with: libimgui_impl_osx
    )

    src = files('backends/imgui_impl_metal.mm')
    libimgui_impl_metal = library('imgui_impl_metal', src,
        dependencies: imgui_dep,
        include_directories: inc,
        build_by_default: false
    )
    imgui_impl_metal_dep = declare_dependency(
        include_directories: inc,
        link_with: libimgui_impl_metal
    )
endif

meson.override_dependency('imgui-backend-glfw', imgui_impl_glfw_dep)
meson.override_dependency('imgui-backend-opengl3', imgui_impl_opengl3_dep)
meson.override_dependency('imgui-backend-win32', imgui_impl_win32_dep)
meson.override_dependency('imgui-backend-dx12', imgui_impl_dx12_dep)
meson.override_dependency('imgui-backend-osx', imgui_impl_osx_dep)
meson.override_dependency('imgui-backend-metal', imgui_impl_metal_dep)
