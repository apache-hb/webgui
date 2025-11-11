src = files(
    'implot_demo.cpp',
    'implot_items.cpp',
    'implot.cpp',
)

inc = include_directories('.')

libimplot = library(
    'implot',
    src,
    include_directories: inc,
    install: true,
    dependencies: [imgui_dep]
)

implot_dep = declare_dependency(
    include_directories: inc,
    link_with: libimplot,
    dependencies: [imgui_dep]
)
