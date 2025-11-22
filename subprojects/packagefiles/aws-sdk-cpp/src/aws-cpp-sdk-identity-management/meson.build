src = files(
    'source/auth/CognitoCachingCredentialsProvider.cpp',
    'source/auth/PersistentCognitoIdentityProvider_JsonFileImpl.cpp',
    'source/auth/STSAssumeRoleCredentialsProvider.cpp',
    'source/auth/STSProfileCredentialsProvider.cpp',
)

inc = include_directories('include')

cpp_args = ['-DAWS_IDENTITY_MANAGEMENT_EXPORTS=1']
public_cpp_args = []
if get_option('default_library') == 'shared'
    public_cpp_args += ['-DUSE_IMPORT_EXPORT=1']
endif

libaws_cpp_sdk_identity_management = library(
    'aws-cpp-sdk-identity-management',
    src,
    include_directories: inc,
    cpp_args: cpp_args + public_cpp_args,
    dependencies: [aws_cpp_sdk_core_dep, aws_cpp_sdk_cognito_identity_dep, aws_cpp_sdk_sts_dep],
    install: true,
    version: meson.project_version(),
    gnu_symbol_visibility: 'hidden',
)

aws_cpp_sdk_identity_management_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_cpp_sdk_identity_management,
    dependencies: [aws_cpp_sdk_core_dep, aws_cpp_sdk_cognito_identity_dep, aws_cpp_sdk_sts_dep],
    compile_args: public_cpp_args,
)

meson.override_dependency('aws-cpp-sdk-identity-management', aws_cpp_sdk_identity_management_dep)

pkg.generate(
    libaws_cpp_sdk_identity_management,
    extra_cflags: public_cpp_args,
    description: 'High-level C++ SDK for identity management against AWS',
)
