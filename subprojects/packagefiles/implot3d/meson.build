project('implot3d', 'cpp')

src = files(
    'implot3d_demo.cpp',
    'implot3d_items.cpp',
    'implot3d_meshes.cpp',
    'implot3d.cpp',
)

imgui_dep = dependency('imgui')

inc = include_directories('.')

libimplot3d = static_library(
    'implot3d',
    src,
    include_directories: inc,
    install: true,
    dependencies: [imgui_dep]
)

implot3d_dep = declare_dependency(
    include_directories: inc,
    link_with: libimplot3d,
    dependencies: [imgui_dep]
)

meson.override_dependency('implot3d', implot3d_dep)
