src = files(
    'source/CognitoIdentityErrors.cpp',
    'source/CognitoIdentityRequest.cpp',
    'source/CognitoIdentityErrorMarshaller.cpp',
    'source/CognitoIdentityClient.cpp',
    'source/CognitoIdentityEndpointRules.cpp',
    'source/CognitoIdentityEndpointProvider.cpp',
    'source/model/AmbiguousRoleResolutionType.cpp',
    'source/model/GetIdRequest.cpp',
    'source/model/ErrorCode.cpp',
    'source/model/ListIdentityPoolsResult.cpp',
    'source/model/ListIdentitiesResult.cpp',
    'source/model/DescribeIdentityRequest.cpp',
    'source/model/GetOpenIdTokenResult.cpp',
    'source/model/GetOpenIdTokenForDeveloperIdentityRequest.cpp',
    'source/model/GetIdentityPoolRolesResult.cpp',
    'source/model/LookupDeveloperIdentityResult.cpp',
    'source/model/GetIdentityPoolRolesRequest.cpp',
    'source/model/UnlinkIdentityRequest.cpp',
    'source/model/GetCredentialsForIdentityRequest.cpp',
    'source/model/GetPrincipalTagAttributeMapRequest.cpp',
    'source/model/MergeDeveloperIdentitiesRequest.cpp',
    'source/model/IdentityDescription.cpp',
    'source/model/RoleMappingType.cpp',
    'source/model/DescribeIdentityPoolRequest.cpp',
    'source/model/TagResourceRequest.cpp',
    'source/model/GetCredentialsForIdentityResult.cpp',
    'source/model/UntagResourceResult.cpp',
    'source/model/GetOpenIdTokenForDeveloperIdentityResult.cpp',
    'source/model/MappingRuleMatchType.cpp',
    'source/model/ListIdentitiesRequest.cpp',
    'source/model/ListTagsForResourceResult.cpp',
    'source/model/SetIdentityPoolRolesRequest.cpp',
    'source/model/CreateIdentityPoolResult.cpp',
    'source/model/MappingRule.cpp',
    'source/model/ListIdentityPoolsRequest.cpp',
    'source/model/Credentials.cpp',
    'source/model/UnprocessedIdentityId.cpp',
    'source/model/DescribeIdentityResult.cpp',
    'source/model/DeleteIdentityPoolRequest.cpp',
    'source/model/SetPrincipalTagAttributeMapRequest.cpp',
    'source/model/GetIdResult.cpp',
    'source/model/CognitoIdentityProvider.cpp',
    'source/model/UnlinkDeveloperIdentityRequest.cpp',
    'source/model/GetOpenIdTokenRequest.cpp',
    'source/model/UntagResourceRequest.cpp',
    'source/model/DeleteIdentitiesResult.cpp',
    'source/model/GetPrincipalTagAttributeMapResult.cpp',
    'source/model/IdentityPoolShortDescription.cpp',
    'source/model/ListTagsForResourceRequest.cpp',
    'source/model/RoleMapping.cpp',
    'source/model/MergeDeveloperIdentitiesResult.cpp',
    'source/model/UpdateIdentityPoolRequest.cpp',
    'source/model/RulesConfigurationType.cpp',
    'source/model/LookupDeveloperIdentityRequest.cpp',
    'source/model/SetPrincipalTagAttributeMapResult.cpp',
    'source/model/DescribeIdentityPoolResult.cpp',
    'source/model/CreateIdentityPoolRequest.cpp',
    'source/model/TagResourceResult.cpp',
    'source/model/UpdateIdentityPoolResult.cpp',
    'source/model/DeleteIdentitiesRequest.cpp',
)

inc = include_directories('include')

if host_machine.system() == 'windows' and get_option('default_library') == 'shared'
    cpp_args = ['-DAWS_COGNITOIDENTITY_EXPORTS=1']
else
    cpp_args = []
endif

libaws_cpp_sdk_cognito_identity = library(
    'aws-cpp-sdk-cognito-identity',
    src,
    include_directories: inc,
    cpp_args: cpp_args + public_cpp_args,
    dependencies: [aws_cpp_sdk_core_dep],
    install: true,
    version: meson.project_version(),
)

aws_cpp_sdk_cognito_identity_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_cpp_sdk_cognito_identity,
    dependencies: [aws_cpp_sdk_core_dep],
    compile_args: public_cpp_args,
)

meson.override_dependency('aws-cpp-sdk-cognito-identity', aws_cpp_sdk_cognito_identity_dep)

pkgs += {
    'aws-cpp-sdk-cognito-identity': {
        'lib': libaws_cpp_sdk_cognito_identity,
        'extra_cflags': public_cpp_args,
    }
}
