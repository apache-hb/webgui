project('aws-c-http', 'c', version: '0.13.3', license: 'Apache-2.0', meson_version: '>=0.63.0')

pkg = import('pkgconfig')

aws_c_common_dep = dependency('aws-c-common')
aws_c_compression_dep = dependency('aws-c-compression')
aws_c_io_dep = dependency('aws-c-io')

deps = [aws_c_common_dep, aws_c_compression_dep, aws_c_io_dep]

src = files(
    'source/hpack_decoder.c',
    'source/proxy_strategy.c',
    'source/hpack.c',
    'source/h1_stream.c',
    'source/statistics.c',
    'source/proxy_connection.c',
    'source/hpack_encoder.c',
    'source/websocket_encoder.c',
    'source/h2_frames.c',
    'source/request_response.c',
    'source/random_access_set.c',
    'source/websocket.c',
    'source/http.c',
    'source/no_proxy.c',
    'source/websocket_bootstrap.c',
    'source/hpack_huffman_static.c',
    'source/h2_decoder.c',
    'source/h1_encoder.c',
    'source/h1_decoder.c',
    'source/websocket_decoder.c',
    'source/connection_manager.c',
    'source/connection.c',
    'source/h2_stream.c',
    'source/http2_stream_manager.c',
    'source/connection_monitor.c',
    'source/h2_connection.c',
    'source/h1_connection.c',
    'source/strutil.c'
)

inc = include_directories('include')

c_args = ['-DAWS_HTTP_EXPORTS=1']
public_c_args = []
if (get_option('default_library') == 'shared' and host_machine.system() == 'windows') or host_machine.system() != 'windows'
  public_c_args += ['-DAWS_HTTP_USE_IMPORT_EXPORT=1']
endif

libaws_c_http = library(
    'aws-c-http',
    src,
    c_args: c_args + public_c_args,
    include_directories: inc,
    dependencies: deps,
    install: true,
    version: meson.project_version(),
    gnu_symbol_visibility: 'hidden',
)

aws_c_http_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_c_http,
    dependencies: deps,
    compile_args: public_c_args,
)

meson.override_dependency('aws-c-http', aws_c_http_dep)

pkg.generate(
    libaws_c_http,
    extra_cflags: public_c_args,
    description: 'C99 implementation of the HTTP/1.1 and HTTP/2 specifications',
)
