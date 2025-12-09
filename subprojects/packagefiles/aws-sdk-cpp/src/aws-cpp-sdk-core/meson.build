subdir('include/aws/core')

inc = include_directories('include')

src = files(
    'source/Globals.cpp',
    'source/Aws.cpp',
    'source/AmazonSerializableWebServiceRequest.cpp',
    'source/Version.cpp',
    'source/AmazonStreamingWebServiceRequest.cpp',
    'source/Region.cpp',
    'source/AmazonWebServiceRequest.cpp',
    'source/utils/StringUtils.cpp',
    'source/utils/FileSystemUtils.cpp',
    'source/utils/Document.cpp',
    'source/utils/UUID.cpp',
    'source/utils/TempFile.cpp',
    'source/utils/DNS.cpp',
    'source/utils/DateTimeCommon.cpp',
    'source/utils/HashingUtils.cpp',
    'source/utils/Directory.cpp',
    'source/utils/Array.cpp',
    'source/utils/EnumParseOverflowContainer.cpp',
    'source/utils/GetTheLights.cpp',
    'source/utils/RAIICounter.cpp',
    'source/utils/ARN.cpp',
    'source/utils/json/JsonSerializer.cpp',
    'source/utils/event/EventEncoderStream.cpp',
    'source/utils/event/EventStreamDecoder.cpp',
    'source/utils/event/EventDecoderStream.cpp',
    'source/utils/event/EventHeader.cpp',
    'source/utils/event/EventStreamEncoder.cpp',
    'source/utils/event/EventMessage.cpp',
    'source/utils/event/EventStreamBuf.cpp',
    'source/utils/event/EventStreamErrors.cpp',
    'source/utils/memory/AWSMemory.cpp',
    'source/utils/memory/stl/SimpleStringStream.cpp',
    'source/utils/xml/XmlSerializer.cpp',
    'source/utils/base64/Base64.cpp',
    'source/utils/threading/ReaderWriterLock.cpp',
    'source/utils/threading/PooledThreadExecutor.cpp',
    'source/utils/threading/DefaultExecutor.cpp',
    'source/utils/threading/Semaphore.cpp',
    'source/utils/threading/SameThreadExecutor.cpp',
    'source/utils/threading/ThreadTask.cpp',
    'source/utils/component-registry/ComponentRegistry.cpp',
    'source/utils/cbor/CborValue.cpp',
    'source/utils/logging/CRTLogging.cpp',
    'source/utils/logging/DefaultCRTLogSystem.cpp',
    'source/utils/logging/LogLevel.cpp',
    'source/utils/logging/DefaultLogSystem.cpp',
    'source/utils/logging/ConsoleLogSystem.cpp',
    'source/utils/logging/CRTLogSystemInterface.cpp',
    'source/utils/logging/AWSLogging.cpp',
    'source/utils/logging/FormattedLogSystem.cpp',
    'source/utils/stream/SimpleStreamBuf.cpp',
    'source/utils/stream/ConcurrentStreamBuf.cpp',
    'source/utils/stream/PreallocatedStreamBuf.cpp',
    'source/utils/stream/ResponseStream.cpp',
    'source/utils/crypto/PrecalculatedHash.cpp',
    'source/utils/crypto/MD5.cpp',
    'source/utils/crypto/ContentCryptoMaterial.cpp',
    'source/utils/crypto/KeyWrapAlgorithm.cpp',
    'source/utils/crypto/Sha1.cpp',
    'source/utils/crypto/CRC32.cpp',
    'source/utils/crypto/CryptoBuf.cpp',
    'source/utils/crypto/CryptoStream.cpp',
    'source/utils/crypto/EncryptionMaterials.cpp',
    'source/utils/crypto/Sha256HMAC.cpp',
    'source/utils/crypto/Sha256.cpp',
    'source/utils/crypto/ContentCryptoScheme.cpp',
    'source/utils/crypto/CRC64.cpp',
    'source/utils/crypto/Cipher.cpp',
    'source/utils/crypto/crt/CRTHash.cpp',
    'source/utils/crypto/crt/CRTSymmetricCipher.cpp',
    'source/utils/crypto/crt/CRTSecureRandomBytes.cpp',
    'source/utils/crypto/crt/CRTHMAC.cpp',
    'source/utils/crypto/factory/Factories.cpp',
    'source/client/CoreErrors.cpp',
    'source/client/RequestCompression.cpp',
    'source/client/UserAgent.cpp',
    'source/client/AWSErrorMarshaller.cpp',
    'source/client/AWSClient.cpp',
    'source/client/DefaultRetryStrategy.cpp',
    'source/client/GenericClientConfiguration.cpp',
    'source/client/SpecifiedRetryableErrorsRetryStrategy.cpp',
    'source/client/AdaptiveRetryStrategy.cpp',
    'source/client/AWSJsonClient.cpp',
    'source/client/RetryStrategy.cpp',
    'source/client/ClientConfiguration.cpp',
    'source/client/AWSXmlClient.cpp',
    'source/client/AsyncCallerContext.cpp',
    'source/client/AWSUrlPresigner.cpp',
    'source/client/AWSRpcV2CborClient.cpp',
    'source/internal/AWSHttpResourceClient.cpp',
    'source/monitoring/DefaultMonitoring.cpp',
    'source/monitoring/HttpClientMetrics.cpp',
    'source/monitoring/MonitoringManager.cpp',
    'source/smithy/tracing/TracingUtils.cpp',
    'source/smithy/client/AwsSmithyClientBase.cpp',
    'source/smithy/identity/AuthSchemeOption.cpp',
    'source/external/cjson/cJSON.cpp',
    'source/external/tinyxml2/tinyxml2.cpp',
    'source/endpoint/EndpointProviderBase.cpp',
    'source/endpoint/DefaultEndpointProvider.cpp',
    'source/endpoint/ClientContextParameters.cpp',
    'source/endpoint/AWSPartitions.cpp',
    'source/endpoint/BuiltInParameters.cpp',
    'source/endpoint/AWSEndpoint.cpp',
    'source/endpoint/internal/AWSEndpointAttribute.cpp',
    'source/auth/AWSCredentialsProvider.cpp',
    'source/auth/AWSCredentialsProviderChain.cpp',
    'source/auth/CrtCredentialsProvider.cpp',
    'source/auth/GeneralHTTPCredentialsProvider.cpp',
    'source/auth/LoginCredentialsProvider.cpp',
    'source/auth/SSOCredentialsProvider.cpp',
    'source/auth/STSCredentialsProvider.cpp',
    'source/auth/bearer-token-provider/DefaultBearerTokenProviderChain.cpp',
    'source/auth/bearer-token-provider/SSOBearerTokenProvider.cpp',
    'source/auth/signer/AWSAuthBearerSigner.cpp',
    'source/auth/signer/AWSAuthV4Signer.cpp',
    'source/auth/signer/AWSAuthEventStreamV4Signer.cpp',
    'source/auth/signer/AWSNullSigner.cpp',
    'source/auth/signer/AWSAuthSignerHelper.cpp',
    'source/auth/signer/AWSAuthSignerCommon.cpp',
    'source/auth/signer-provider/BearerTokenAuthSignerProvider.cpp',
    'source/auth/signer-provider/AWSAuthSignerProviderBase.cpp',
    'source/auth/signer-provider/DefaultAuthSignerProvider.cpp',
    'source/http/HttpClientFactory.cpp',
    'source/http/HttpTypes.cpp',
    'source/http/URI.cpp',
    'source/http/Scheme.cpp',
    'source/http/HttpClient.cpp',
    'source/http/HttpResponse.cpp',
    'source/http/HttpRequest.cpp',
    'source/http/crt/CRTHttpClient.cpp',
    'source/http/standard/StandardHttpResponse.cpp',
    'source/http/standard/StandardHttpRequest.cpp',
    'source/config/EC2InstanceProfileConfigLoader.cpp',
    'source/config/EndpointResolver.cpp',
    'source/config/ConfigAndCredentialsCacheManager.cpp',
    'source/config/AWSProfileConfigLoaderBase.cpp',
    'source/config/AWSConfigFileProfileConfigLoader.cpp',
    'source/config/defaults/ClientConfigurationDefaults.cpp'
)

optel_src = files(
    'source/smithy/tracing/impl/opentelemetry/MeterAdapters.cpp',
    'source/smithy/tracing/impl/opentelemetry/OtelMeterProvider.cpp',
    'source/smithy/tracing/impl/opentelemetry/TraceAdapters.cpp',
    'source/smithy/tracing/impl/opentelemetry/OtelTelemetryProvider.cpp',
    'source/smithy/tracing/impl/opentelemetry/OtelTracerProvider.cpp',
)

curl_http_src = files(
    'source/http/curl/CurlHttpClient.cpp',
    'source/http/curl/CurlHandleContainer.cpp',
)

windows_http_src = files(
    'source/http/windows/WinHttpConnectionPoolMgr.cpp',
    'source/http/windows/WinConnectionPoolMgr.cpp',
    'source/http/windows/WinINetSyncHttpClient.cpp',
    'source/http/windows/WinSyncHttpClient.cpp',
    'source/http/windows/WinINetConnectionPoolMgr.cpp',
    'source/http/windows/WinHttpSyncHttpClient.cpp',
)

windows_net_src = files(
    'source/net/windows/SimpleUDP.cpp',
    'source/net/windows/Net.cpp',
)

android_src = files(
    'source/platform/android/FileSystem.cpp',
    'source/platform/android/Platform.cpp',
    'source/platform/android/Environment.cpp',
    'source/platform/android/OSVersionInfo.cpp',
    'source/platform/android/Time.cpp',
    'source/platform/android/LogcatLogSystem.cpp',
    'source/platform/android/Security.cpp',
)

windows_src = files(
    'source/platform/windows/FileSystem.cpp',
    'source/platform/windows/Environment.cpp',
    'source/platform/windows/OSVersionInfo.cpp',
    'source/platform/windows/Time.cpp',
    'source/platform/windows/Security.cpp',
)

linux_src = files(
    'source/platform/linux-shared/FileSystem.cpp',
    'source/platform/linux-shared/Environment.cpp',
    'source/platform/linux-shared/OSVersionInfo.cpp',
    'source/platform/linux-shared/Time.cpp',
    'source/platform/linux-shared/Security.cpp',
)

linux_net_src = files(
    'source/net/linux-shared/SimpleUDP.cpp',
    'source/net/linux-shared/Net.cpp',
)

default_net_src = files(
    'source/net/SimpleUDP.cpp',
    'source/net/Net.cpp',
)

deps = [
    aws_crt_cpp_dep,
    dependency('threads'),
    cpp.find_library('dl', required: false),
]

optel_dep = dependency('opentelemetry-cpp', required: false)

if optel_dep.found()
    ostream_exporter_dep = dependency('opentelemetry-cpp-ostream-exporter')
    src += optel_src
    deps += [optel_dep, ostream_exporter_dep]
endif

curl = dependency('libcurl', required: host_machine.system() in ['linux', 'android'])

if host_machine.system() == 'windows' and get_option('default_library') == 'shared'
    cpp_args = ['-DAWS_CORE_EXPORTS=1', '-DSMITHY_EXPORTS=1']
else
    cpp_args = []
endif

if host_machine.system() == 'windows'
    src += windows_src + windows_http_src + windows_net_src
    deps += [
        cpp.find_library('wininet', required: false),
        cpp.find_library('winhttp', required: false),
        cpp.find_library('ws2_32', required: false),
        cpp.find_library('userenv', required: false),
        cpp.find_library('version', required: false),
    ]
    cpp_args += [
        '-DENABLE_WINDOWS_CLIENT=1',
        '-DAWS_SDK_USE_CRT_HTTP=1',
    ]
elif host_machine.system() == 'android'
    src += android_src + curl_http_src
    deps += [curl]
    cpp_args += ['-DENABLE_CURL_CLIENT=1']
elif host_machine.system() == 'linux'
    src += linux_src + curl_http_src + linux_net_src
    deps += [curl]
    cpp_args += ['-DENABLE_CURL_CLIENT=1']
elif host_machine.system() == 'emscripten'
    src += linux_src + default_net_src
else
    src += default_net_src
endif

libaws_cpp_sdk_core = library(
    'aws-cpp-sdk-core',
    src,
    cpp_args: cpp_args + public_cpp_args,
    include_directories: inc,
    dependencies: deps,
    install: true,
    version: meson.project_version(),
)

aws_cpp_sdk_core_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_cpp_sdk_core,
    dependencies: [aws_crt_cpp_dep],
    compile_args: public_cpp_args,
)

meson.override_dependency('aws-cpp-sdk-core', aws_cpp_sdk_core_dep)

pkgs += {
    'aws-cpp-sdk-core': {
        'lib': libaws_cpp_sdk_core,
        'extra_cflags': public_cpp_args,
        'description': 'Core http and utility library for the AWS C++ SDK',
    }
}
