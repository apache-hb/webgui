src = files(
    'source/AccountClient.cpp',
    'source/AccountErrors.cpp',
    'source/AccountEndpointRules.cpp',
    'source/AccountErrorMarshaller.cpp',
    'source/AccountEndpointProvider.cpp',
    'source/AccountRequest.cpp',
    'source/model/ConflictException.cpp',
    'source/model/PutAccountNameRequest.cpp',
    'source/model/GetAlternateContactResult.cpp',
    'source/model/ValidationExceptionField.cpp',
    'source/model/ResourceNotFoundException.cpp',
    'source/model/StartPrimaryEmailUpdateRequest.cpp',
    'source/model/GetAlternateContactRequest.cpp',
    'source/model/AccessDeniedException.cpp',
    'source/model/GetPrimaryEmailResult.cpp',
    'source/model/DeleteAlternateContactRequest.cpp',
    'source/model/GetAccountInformationRequest.cpp',
    'source/model/TooManyRequestsException.cpp',
    'source/model/PrimaryEmailUpdateStatus.cpp',
    'source/model/GetRegionOptStatusRequest.cpp',
    'source/model/PutContactInformationRequest.cpp',
    'source/model/PutAlternateContactRequest.cpp',
    'source/model/GetPrimaryEmailRequest.cpp',
    'source/model/RegionOptStatus.cpp',
    'source/model/ListRegionsResult.cpp',
    'source/model/ValidationExceptionReason.cpp',
    'source/model/AlternateContact.cpp',
    'source/model/ContactInformation.cpp',
    'source/model/GetContactInformationResult.cpp',
    'source/model/StartPrimaryEmailUpdateResult.cpp',
    'source/model/EnableRegionRequest.cpp',
    'source/model/AcceptPrimaryEmailUpdateRequest.cpp',
    'source/model/AlternateContactType.cpp',
    'source/model/Region.cpp',
    'source/model/GetAccountInformationResult.cpp',
    'source/model/AcceptPrimaryEmailUpdateResult.cpp',
    'source/model/ListRegionsRequest.cpp',
    'source/model/DisableRegionRequest.cpp',
    'source/model/ValidationException.cpp',
    'source/model/InternalServerException.cpp',
    'source/model/GetRegionOptStatusResult.cpp',
    'source/model/GetContactInformationRequest.cpp'
)

inc = include_directories('include')

libaws_cpp_sdk_account = library(
    'aws-cpp-sdk-account',
    src,
    include_directories: inc,
    cpp_args: ['-DAWS_KMS_EXPORTS=1'],
    dependencies: [aws_cpp_sdk_core_dep],
    install: true,
    version: meson.project_version(),
    gnu_symbol_visibility: 'hidden',
)

aws_cpp_sdk_account_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_cpp_sdk_account,
    dependencies: [aws_cpp_sdk_core_dep],
)

meson.override_dependency('aws-cpp-sdk-account', aws_cpp_sdk_account_dep)
