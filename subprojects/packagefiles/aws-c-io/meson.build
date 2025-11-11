project('aws-c-io', 'c', version: '0.23.3', license: 'Apache-2.0', meson_version: '>=0.63.0')

aws_c_common_dep = dependency('aws-c-common')

src = files(
    'source/socket.c'
)

inc = include_directories('include')

libaws_c_io = library(
    'aws-c-io',
    src,
    include_directories: inc,
    install: true,
    dependencies: [aws_c_common_dep],
    version: meson.project_version(),
)

aws_c_io_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_c_io,
    dependencies: [aws_c_common_dep],
)

meson.override_dependency('aws-c-io', aws_c_io_dep)
