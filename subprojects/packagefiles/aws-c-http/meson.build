project('aws-c-http', 'c', version: '0.13.3', license: 'Apache-2.0', meson_version: '>=0.63.0')

aws_c_common_dep = dependency('aws-c-common')
aws_c_compression_dep = dependency('aws-c-compression')
aws_c_io_dep = dependency('aws-c-io')

deps = [aws_c_common_dep, aws_c_compression_dep, aws_c_io_dep]

src = files(
    'source/http.c'
)

inc = include_directories('include')

libaws_c_http = library(
    'aws-c-http',
    src,
    include_directories: inc,
    install: true,
    dependencies: deps,
    version: meson.project_version(),
)

aws_c_http_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_c_http,
    dependencies: deps,
)

meson.override_dependency('aws-c-http', aws_c_http_dep)
