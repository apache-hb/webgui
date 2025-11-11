project('aws-c-event-stream', 'c', version: '0.13.3', license: 'Apache-2.0', meson_version: '>=0.63.0')

aws_c_common_dep = dependency('aws-c-common')
aws_c_io_dep = dependency('aws-c-io')
aws_checksums_dep = dependency('aws-checksums')

deps = [aws_c_common_dep, aws_c_io_dep, aws_checksums_dep]

src = files(
    'source/event_stream.c'
)

inc = include_directories('include')

libaws_c_event_stream = library(
    'aws-c-event-stream',
    src,
    include_directories: inc,
    install: true,
    dependencies: deps,
    version: meson.project_version(),
)

aws_c_event_stream_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_c_event_stream,
    dependencies: deps,
)

meson.override_dependency('aws-c-event-stream', aws_c_event_stream_dep)
