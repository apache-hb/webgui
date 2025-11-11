project('aws-c-s3', 'c', version: '0.13.3', license: 'Apache-2.0', meson_version: '>=0.63.0')

aws_c_common_dep = dependency('aws-c-common')
aws_c_auth_dep = dependency('aws-c-auth')
aws_checksums_dep = dependency('aws-checksums')
aws_c_http_dep = dependency('aws-c-http')

deps = [aws_c_common_dep, aws_c_auth_dep, aws_checksums_dep, aws_c_http_dep]

src = files(
    'source/s3.c'
)

inc = include_directories('include')

libaws_c_s3 = library(
    'aws-c-s3',
    src,
    include_directories: inc,
    install: true,
    dependencies: deps,
    version: meson.project_version(),
)

aws_c_s3_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_c_s3,
    dependencies: deps,
)

meson.override_dependency('aws-c-s3', aws_c_s3_dep)
