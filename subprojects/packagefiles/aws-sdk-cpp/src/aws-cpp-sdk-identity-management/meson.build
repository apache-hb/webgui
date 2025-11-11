src = files(
    'source/auth/CognitoCachingCredentialsProvider.cpp',
    'source/auth/PersistentCognitoIdentityProvider_JsonFileImpl.cpp',
    'source/auth/STSAssumeRoleCredentialsProvider.cpp',
    'source/auth/STSProfileCredentialsProvider.cpp',
)

inc = include_directories('include')

libaws_cpp_sdk_identity_management = library(
    'aws-cpp-sdk-identity-management',
    src,
    include_directories: inc,
    install: true,
    cpp_args: ['-DAWS_IDENTITY_MANAGEMENT_EXPORTS=1'],
    dependencies: [aws_cpp_sdk_core_dep, aws_cpp_sdk_cognito_identity_dep, aws_cpp_sdk_sts_dep],
    version: meson.project_version(),
)

aws_cpp_sdk_identity_management_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_cpp_sdk_identity_management,
    dependencies: [aws_cpp_sdk_core_dep, aws_cpp_sdk_cognito_identity_dep, aws_cpp_sdk_sts_dep],
)

meson.override_dependency('aws-cpp-sdk-identity-management', aws_cpp_sdk_identity_management_dep)
