project('debugbreak', 'c',
    license: 'BSD-2-Clause',
)

inc = include_directories('.')

debugbreak_dep = declare_dependency(
    include_directories: inc,
)

meson.override_dependency('debugbreak', debugbreak_dep)
