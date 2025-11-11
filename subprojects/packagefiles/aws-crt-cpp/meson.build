project('aws-crt-cpp', 'cpp', version: '0.35.2', license: 'Apache-2.0', meson_version: '>=0.63.0')

pkg = import('pkgconfig')

aws_c_common_dep = dependency('aws-c-common')
aws_c_io_dep = dependency('aws-c-io')
aws_c_mqtt_dep = dependency('aws-c-mqtt')
aws_c_cal_dep = dependency('aws-c-cal')
aws_c_auth_dep = dependency('aws-c-auth')
aws_checksums_dep = dependency('aws-checksums')
aws_c_event_stream_dep = dependency('aws-c-event-stream')
aws_c_s3_dep = dependency('aws-c-s3')

subdir('include')
subdir('include/aws/crt')

deps = [
    aws_c_common_dep,
    aws_c_io_dep,
    aws_c_mqtt_dep,
    aws_c_cal_dep,
    aws_c_auth_dep,
    aws_checksums_dep,
    aws_c_event_stream_dep,
    aws_c_s3_dep,
]

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
    dependencies: deps,
    install: true,
)

aws_cpp_crt_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_cpp_crt,
    dependencies: deps,
)

meson.override_dependency('aws-crt-cpp', aws_cpp_crt_dep)

pkg.generate(
    libaws_cpp_crt,
    description: 'C++ wrapper around the aws-c-* libraries. Provides Cross-Platform Transport Protocols and SSL/TLS implementations for C++.',
)
