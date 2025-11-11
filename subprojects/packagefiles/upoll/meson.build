project('upoll', 'c')

src = files(
    'src/upoll.c'
)

inc = include_directories('src')

libupoll = library(
    'upoll',
    src,
    include_directories: inc,
    install: true,
)

upoll_dep = declare_dependency(
    include_directories: inc,
    link_with: libupoll,
)

meson.override_dependency('upoll', upoll_dep)
