project('aws-crt-cpp', 'cpp', version: '0.35.2', license: 'Apache-2.0', meson_version: '>=0.63.0')

pkg = import('pkgconfig')

aws_c_common_dep = dependency('aws-c-common')

subdir('include')
subdir('include/aws/crt')

inc = include_directories('include')

src = files(
    'source/Allocator.cpp',
    'source/Api.cpp',
    'source/DateTime.cpp',
)

libaws_cpp_crt = library(
    'aws-crt-cpp',
    src,
    include_directories: inc,
    dependencies: [aws_c_common_dep],
    install: true,
)

aws_cpp_crt_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_cpp_crt,
    dependencies: [aws_c_common_dep]
)

meson.override_dependency('aws-crt-cpp', aws_cpp_crt_dep)

pkg.generate(
    libaws_cpp_crt,
    description: 'C++ wrapper around the aws-c-* libraries. Provides Cross-Platform Transport Protocols and SSL/TLS implementations for C++.',
)
