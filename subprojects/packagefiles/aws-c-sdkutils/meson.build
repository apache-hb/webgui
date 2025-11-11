project('aws-c-sdkutils', 'c', version: '0.13.3', license: 'Apache-2.0', meson_version: '>=0.63.0')

aws_c_common_dep = dependency('aws-c-common')

deps = [aws_c_common_dep]

src = files(
    'source/sdkutils.c'
)

inc = include_directories('include')

libaws_c_sdkutils = library(
    'aws-c-sdkutils',
    src,
    include_directories: inc,
    install: true,
    dependencies: deps,
    version: meson.project_version(),
)

aws_c_sdkutils_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_c_sdkutils,
    dependencies: deps,
)

meson.override_dependency('aws-c-sdkutils', aws_c_sdkutils_dep)
