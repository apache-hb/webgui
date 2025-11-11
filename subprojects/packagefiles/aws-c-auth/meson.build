project('aws-c-auth', 'c', version: '0.13.3', license: 'Apache-2.0', meson_version: '>=0.63.0')

aws_c_http_dep = dependency('aws-c-http')
aws_c_cal_dep = dependency('aws-c-cal')
aws_c_sdkutils_dep = dependency('aws-c-sdkutils')

deps = [aws_c_http_dep, aws_c_cal_dep, aws_c_sdkutils_dep]

src = files(
    'source/auth.c'
)

inc = include_directories('include')

libaws_c_auth = library(
    'aws-c-auth',
    src,
    include_directories: inc,
    install: true,
    dependencies: deps,
    version: meson.project_version(),
)

aws_c_auth_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_c_auth,
    dependencies: deps,
)

meson.override_dependency('aws-c-auth', aws_c_auth_dep)
