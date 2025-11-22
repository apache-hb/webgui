project('aws-crt-cpp', 'cpp', version: '0.35.2', license: 'Apache-2.0', meson_version: '>=0.63.0')

pkg = import('pkgconfig')
byo_crypto = get_option('byo_crypto')

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
    'source/DnsUtils.cpp',
    'source/StringUtils.cpp',
    'source/Api.cpp',
    'source/UUID.cpp',
    'source/ImdsClient.cpp',
    'source/Allocator.cpp',
    'source/DateTime.cpp',
    'source/Types.cpp',
    'source/JsonObject.cpp',
    'source/mqtt/MqttConnectionCore.cpp',
    'source/mqtt/MqttClient.cpp',
    'source/mqtt/Mqtt5Packets.cpp',
    'source/mqtt/MqttConnection.cpp',
    'source/mqtt/Mqtt5Client.cpp',
    'source/mqtt/Mqtt5ClientCore.cpp',
    'source/endpoints/RuleEngine.cpp',
    'source/io/Uri.cpp',
    'source/io/HostResolver.cpp',
    'source/io/Bootstrap.cpp',
    'source/io/ChannelHandler.cpp',
    'source/io/Stream.cpp',
    'source/io/TlsOptions.cpp',
    'source/io/SocketOptions.cpp',
    'source/io/Pkcs11.cpp',
    'source/io/EventLoopGroup.cpp',
    'source/iot/MqttRequestResponseClient.cpp',
    'source/iot/MqttCommon.cpp',
    'source/iot/MqttClient.cpp',
    'source/iot/Mqtt5Client.cpp',
    'source/auth/Sigv4Signing.cpp',
    'source/auth/Credentials.cpp',
    'source/checksum/CRC.cpp',
    'source/http/HttpProxyStrategy.cpp',
    'source/http/HttpConnection.cpp',
    'source/http/HttpConnectionManager.cpp',
    'source/http/HttpRequestResponse.cpp',
    'source/cbor/Cbor.cpp',
    'source/crypto/HMAC.cpp',
    'source/crypto/SymmetricCipher.cpp',
    'source/crypto/Hash.cpp',
    'source/crypto/HKDF.cpp',
    'source/crypto/SecureRandom.cpp'
)

cpp_args = []

if byo_crypto.allowed()
    cpp_args += [ '-DBYO_CRYPTO=1' ]
endif

cpp_args += ['-DAWS_CRT_CPP_EXPORTS=1']

public_cpp_args = []
if (get_option('default_library') == 'shared' and host_machine.system() == 'windows') or host_machine.system() != 'windows'
  public_cpp_args += ['-DAWS_CRT_CPP_USE_IMPORT_EXPORT=1']
endif

libaws_cpp_crt = library(
    'aws-crt-cpp',
    src,
    cpp_args: cpp_args + public_cpp_args,
    include_directories: inc,
    dependencies: deps,
    install: true,
    version: meson.project_version(),
    gnu_symbol_visibility: 'hidden',
)

aws_cpp_crt_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_cpp_crt,
    dependencies: deps,
    compile_args: public_cpp_args,
)

meson.override_dependency('aws-crt-cpp', aws_cpp_crt_dep)

pkg.generate(
    libaws_cpp_crt,
    extra_cflags: public_cpp_args,
    description: 'C++ wrapper around the aws-c-* libraries. Provides Cross-Platform Transport Protocols and SSL/TLS implementations for C++.',
)
