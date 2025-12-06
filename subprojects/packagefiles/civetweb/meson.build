project('civetweb', 'c', 'cpp',
    version: '1.16',
    license: 'MIT',
)

pkg = import('pkgconfig')

src = files(
    'src/civetweb.c',
    'src/CivetServer.cpp',
)

inc = include_directories('include')

libcivetweb = library(
    'civetweb',
    src,
    c_args: [ '-DNO_SSL=1' ],
    include_directories: inc,
    install: true,
    version: meson.project_version(),
)

civetweb_dep = declare_dependency(
    include_directories: inc,
    link_with: libcivetweb,
)

meson.override_dependency('civetweb', civetweb_dep)

pkg.generate(libcivetweb)
