project('rapidyaml', 'cpp',
    version: '0.10.0',
    license: 'MIT',
)

pkg = import('pkgconfig')

c4core_dep = dependency('c4core')

inc = include_directories('src')

src = files(
    'src/c4/yml/common.cpp',
    'src/c4/yml/node.cpp',
    'src/c4/yml/parse.cpp',
    'src/c4/yml/preprocess.cpp',
    'src/c4/yml/reference_resolver.cpp',
    'src/c4/yml/tag.cpp',
    'src/c4/yml/tree.cpp',
    'src/c4/yml/version.cpp',
)

librapidyaml = library(
    'rapidyaml',
    src,
    include_directories: inc,
    dependencies: c4core_dep,
    install: true,
    version: meson.project_version(),
)

rapidyaml_dep = declare_dependency(
    link_with: librapidyaml,
    include_directories: inc,
    dependencies: c4core_dep,
)

meson.override_dependency('rapidyaml', rapidyaml_dep)

pkg.generate(librapidyaml)
