src = files(
    'source/STSEndpointRules.cpp',
    'source/STSErrors.cpp',
    'source/STSEndpointProvider.cpp',
    'source/STSClient.cpp',
    'source/STSRequest.cpp',
    'source/STSErrorMarshaller.cpp',
    'source/model/AssumeRoleResult.cpp',
    'source/model/PolicyDescriptorType.cpp',
    'source/model/GetCallerIdentityRequest.cpp',
    'source/model/GetDelegatedAccessTokenRequest.cpp',
    'source/model/AssumeRoleWithWebIdentityResult.cpp',
    'source/model/AssumeRoleWithSAMLRequest.cpp',
    'source/model/AssumedRoleUser.cpp',
    'source/model/ResponseMetadata.cpp',
    'source/model/ProvidedContext.cpp',
    'source/model/AssumeRoleWithSAMLResult.cpp',
    'source/model/GetCallerIdentityResult.cpp',
    'source/model/AssumeRootResult.cpp',
    'source/model/DecodeAuthorizationMessageResult.cpp',
    'source/model/GetFederationTokenResult.cpp',
    'source/model/AssumeRoleWithWebIdentityRequest.cpp',
    'source/model/GetFederationTokenRequest.cpp',
    'source/model/GetDelegatedAccessTokenResult.cpp',
    'source/model/GetAccessKeyInfoRequest.cpp',
    'source/model/Credentials.cpp',
    'source/model/FederatedUser.cpp',
    'source/model/DecodeAuthorizationMessageRequest.cpp',
    'source/model/AssumeRoleRequest.cpp',
    'source/model/GetSessionTokenRequest.cpp',
    'source/model/AssumeRootRequest.cpp',
    'source/model/GetSessionTokenResult.cpp',
    'source/model/Tag.cpp',
    'source/model/GetAccessKeyInfoResult.cpp'
)

inc = include_directories('include')

if host_machine.system() == 'windows' and get_option('default_library') == 'shared'
    cpp_args = ['-DAWS_STS_EXPORTS=1']
else
    cpp_args = []
endif

libaws_cpp_sdk_sts = library(
    'aws-cpp-sdk-sts',
    src,
    include_directories: inc,
    cpp_args: cpp_args + public_cpp_args,
    dependencies: [aws_cpp_sdk_core_dep],
    install: true,
    version: meson.project_version(),
)

aws_cpp_sdk_sts_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_cpp_sdk_sts,
    dependencies: [aws_cpp_sdk_core_dep],
    compile_args: public_cpp_args,
)

meson.override_dependency('aws-cpp-sdk-sts', aws_cpp_sdk_sts_dep)
