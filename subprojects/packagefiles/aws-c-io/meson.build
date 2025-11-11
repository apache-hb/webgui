project('aws-c-io',
'c', version: '0.23.3', license: 'Apache-2.0', meson_version: '>=0.63.0')

aws_c_common_dep = dependency('aws-c-common')
aws_c_cal_dep = dependency('aws-c-cal')
upoll_dep = dependency('upoll', required: host_machine.system() == 'emscripten')

src = files(
    'source/socket.c',
    'source/retry_strategy.c',
    'source/pkcs11_lib.c',
    'source/socket_channel_handler.c',
    'source/statistics.c',
    'source/tls_channel_handler_shared.c',
    'source/pkcs11_tls_op_handler.c',
    'source/standard_retry_strategy.c',
    'source/tls_channel_handler.c',
    'source/pem.c',
    'source/async_stream.c',
    'source/message_pool.c',
    'source/alpn_handler.c',
    'source/io.c',
    'source/host_resolver.c',
    'source/future.c',
    'source/stream.c',
    'source/event_loop.c',
    'source/tracing.c',
    'source/no_retry_strategy.c',
    'source/socket_shared.c',
    'source/exponential_backoff_retry_strategy.c',
    'source/channel.c',
    'source/channel_bootstrap.c',
)

linux_src = files(
    'source/linux/epoll_event_loop.c',
)

bsd_src = files(
    'source/bsd/kqueue_event_loop.c',
)

posix_src = files(
    'source/posix/pipe.c',
    'source/posix/socket.c',
    'source/posix/shared_library.c',
    'source/posix/host_resolver.c',
)

darwin_src = files(
    'source/darwin/darwin_pki_utils.c',
    'source/darwin/secure_transport_tls_channel_handler.c',
    'source/darwin/nw_socket.c',
    'source/darwin/dispatch_queue_event_loop.c',
)

s2n_src = files(
    'source/s2n/s2n_tls_channel_handler.c',
)

windows_src = files(
    'source/windows/shared_library.c',
    'source/windows/windows_pki_utils.c',
    'source/windows/winsock_init.c',
    'source/windows/secure_channel_tls_handler.c',
    'source/windows/host_resolver.c',
    'source/windows/iocp/pipe.c',
    'source/windows/iocp/socket.c',
    'source/windows/iocp/iocp_event_loop.c'
)

emscripten_src = files(
    'source/emscripten/epoll.c'
)

c_args = []
public_inc = include_directories('include')
inc = [public_inc]

if host_machine.system() == 'linux'
    src += linux_src + posix_src
    c_args += [ '-DAWS_ENABLE_EPOLL' ]
elif host_machine.system() in ['freebsd', 'openbsd', 'netbsd']
    src += bsd_src + posix_src
    c_args += [ '-DAWS_ENABLE_KQUEUE' ]
elif host_machine.system() == 'darwin'
    src += darwin_src + posix_src
    c_args += [ '-DAWS_ENABLE_DISPATCH_QUEUE' ]
elif host_machine.system() == 'windows'
    src += windows_src
    c_args += [ '-DAWS_ENABLE_IO_COMPLETION_PORTS' ]
elif host_machine.system() == 'emscripten'
    src += posix_src + emscripten_src + linux_src
    c_args += [ '-DAWS_ENABLE_EPOLL' ]
    inc += include_directories('source/emscripten')
endif

libaws_c_io = library(
    'aws-c-io',
    src,
    c_args: c_args,
    include_directories: inc,
    install: true,
    dependencies: [aws_c_common_dep, aws_c_cal_dep, upoll_dep],
    version: meson.project_version(),
)

aws_c_io_dep = declare_dependency(
    include_directories: public_inc,
    link_with: libaws_c_io,
    dependencies: [aws_c_common_dep, aws_c_cal_dep],
)

meson.override_dependency('aws-c-io', aws_c_io_dep)
