project('c4core', 'cpp',
    version: '0.2.7',
    license: 'MIT',
)

pkg = import('pkgconfig')

debugbreak_dep = dependency('debugbreak')

inc = include_directories('src')

src = files(
    'src/c4/base64.cpp',
    'src/c4/char_traits.cpp',
    'src/c4/error.cpp',
    'src/c4/format.cpp',
    'src/c4/language.cpp',
    'src/c4/memory_resource.cpp',
    'src/c4/memory_util.cpp',
    'src/c4/utf.cpp',
    'src/c4/version.cpp',
)

libc4core = library(
    'c4core',
    src,
    include_directories: inc,
    dependencies: debugbreak_dep,
    install: true,
    version: meson.project_version(),
)

c4core_dep = declare_dependency(
    link_with: libc4core,
    include_directories: inc,
    dependencies: debugbreak_dep,
)

meson.override_dependency('c4core', c4core_dep)

pkg.generate(libc4core)
