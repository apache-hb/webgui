project('aws-c-event-stream', 'c', version: '0.5.7', license: 'Apache-2.0', meson_version: '>=0.63.0')

pkg = import('pkgconfig')

aws_c_common_dep = dependency('aws-c-common')
aws_c_io_dep = dependency('aws-c-io')
aws_checksums_dep = dependency('aws-checksums')

deps = [aws_c_common_dep, aws_c_io_dep, aws_checksums_dep]

src = files(
    'source/event_stream.c'
)

inc = include_directories('include')

c_args = ['-DAWS_EVENT_STREAM_EXPORTS=1']
public_c_args = []
if get_option('default_library') == 'shared'
  public_c_args += ['-DAWS_EVENT_STREAM_USE_IMPORT_EXPORT=1']
endif

libaws_c_event_stream = library(
    'aws-c-event-stream',
    src,
    c_args: c_args + public_c_args,
    include_directories: inc,
    dependencies: deps,
    install: true,
    version: meson.project_version(),
    gnu_symbol_visibility: 'hidden',
)

aws_c_event_stream_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_c_event_stream,
    dependencies: deps,
    compile_args: public_c_args,
)

meson.override_dependency('aws-c-event-stream', aws_c_event_stream_dep)

pkg.generate(
    libaws_c_event_stream,
    extra_cflags: public_c_args,
    description: 'C99 implementation of the vnd.amazon.eventstream content-type.',
)
