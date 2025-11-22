project('aws-c-s3', 'c', version: '0.10.1', license: 'Apache-2.0', meson_version: '>=0.63.0')

pkg = import('pkgconfig')

aws_c_common_dep = dependency('aws-c-common')
aws_c_auth_dep = dependency('aws-c-auth')
aws_checksums_dep = dependency('aws-checksums')
aws_c_http_dep = dependency('aws-c-http')

deps = [aws_c_common_dep, aws_c_auth_dep, aws_checksums_dep, aws_c_http_dep]

src = files(
    'source/s3_request.c',
    'source/s3_paginator.c',
    'source/s3.c',
    'source/s3_chunk_stream.c',
    'source/s3_auto_ranged_get.c',
    'source/s3_client.c',
    'source/s3_checksums.c',
    'source/s3_copy_object.c',
    'source/s3_part_streaming_input_stream.c',
    'source/s3_request_messages.c',
    'source/s3_parallel_input_stream.c',
    'source/s3_util.c',
    'source/s3_platform_info.c',
    'source/s3_auto_ranged_put.c',
    'source/s3_default_meta_request.c',
    'source/s3_list_parts.c',
    'source/s3_list_objects.c',
    'source/s3express_credentials_provider.c',
    'source/s3_buffer_pool.c',
    'source/s3_meta_request.c',
    'source/s3_checksum_stream.c',
    'source/s3_endpoint.c',
    'source/s3_default_buffer_pool.c',
    'source/s3_checksum_context.c',
    'source/s3_endpoint_resolver/s3_endpoint_resolver.c',
    'source/s3_endpoint_resolver/aws_s3_endpoint_rule_set.c',
    'source/s3_endpoint_resolver/aws_s3_endpoint_resolver_partition.c'
)

inc = include_directories('include')

c_args = ['-DAWS_S3_EXPORTS=1']
public_c_args = []
if get_option('default_library') == 'shared'
  public_c_args += ['-DAWS_S3_USE_IMPORT_EXPORT=1']
endif

libaws_c_s3 = library(
    'aws-c-s3',
    src,
    c_args: c_args + public_c_args,
    include_directories: inc,
    dependencies: deps,
    install: true,
    version: meson.project_version(),
    gnu_symbol_visibility: 'hidden',
)

aws_c_s3_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_c_s3,
    dependencies: deps,
    compile_args: public_c_args,
)

meson.override_dependency('aws-c-s3', aws_c_s3_dep)

pkg.generate(
    libaws_c_s3,
    extra_cflags: public_c_args,
    description: 'C99 library implementation for communicating with the S3 service, designed for maximizing throughput on high bandwidth EC2 instances.',
)
