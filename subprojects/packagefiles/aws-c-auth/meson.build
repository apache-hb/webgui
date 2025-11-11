project('aws-c-auth', 'c', version: '0.13.3', license: 'Apache-2.0', meson_version: '>=0.63.0')

aws_c_http_dep = dependency('aws-c-http')
aws_c_cal_dep = dependency('aws-c-cal')
aws_c_sdkutils_dep = dependency('aws-c-sdkutils')

deps = [aws_c_http_dep, aws_c_cal_dep, aws_c_sdkutils_dep]

src = files(
    'source/sigv4_http_request.c',
    'source/signable.c',
    'source/credentials_provider_sso.c',
    'source/credentials_provider_x509.c',
    'source/credentials_provider_sts.c',
    'source/credentials_provider_sts_web_identity.c',
    'source/credentials.c',
    'source/credentials_provider_imds.c',
    'source/credentials_provider_anonymous.c',
    'source/aws_signing.c',
    'source/signing.c',
    'source/credentials_provider_default_chain.c',
    'source/signing_config.c',
    'source/credentials_provider_process.c',
    'source/auth.c',
    'source/key_derivation.c',
    'source/credentials_provider_cached.c',
    'source/token_provider_sso_profile.c',
    'source/signing_result.c',
    'source/aws_profile.c',
    'source/credentials_utils.c',
    'source/credentials_provider_environment.c',
    'source/credentials_provider_delegate.c',
    'source/credentials_provider_chain.c',
    'source/credentials_provider_ecs.c',
    'source/signable_http_request.c',
    'source/credentials_provider_profile.c',
    'source/aws_imds_client.c',
    'source/signable_chunk.c',
    'source/sso_token_utils.c',
    'source/credentials_provider_static.c',
    'source/signable_trailer.c',
    'source/credentials_provider_cognito.c',
    'source/token_provider_sso_session.c'
)
inc = include_directories('include')

libaws_c_auth = library(
    'aws-c-auth',
    src,
    include_directories: inc,
    install: true,
    dependencies: deps,
    version: meson.project_version(),
)

aws_c_auth_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_c_auth,
    dependencies: deps,
)

meson.override_dependency('aws-c-auth', aws_c_auth_dep)
