project('aws-c-mqtt', 'c', version: '0.13.3', license: 'Apache-2.0', meson_version: '>=0.63.0')

pkg = import('pkgconfig')

aws_c_common_dep = dependency('aws-c-common')
aws_c_http_dep = dependency('aws-c-http')

src = files(
    'source/client.c',
    'source/client_channel_handler.c',
    'source/client_impl_shared.c',
    'source/fixed_header.c',
    'source/mqtt.c',
    'source/mqtt311_decoder.c',
    'source/mqtt311_listener.c',
    'source/mqtt_subscription_set.c',
    'source/packets.c',
    'source/shared.c',
    'source/topic_tree.c',
    'source/request-response/protocol_adapter.c',
    'source/request-response/request_response_client.c',
    'source/request-response/request_response_subscription_set.c',
    'source/request-response/subscription_manager.c',
    'source/v5/mqtt5_callbacks.c',
    'source/v5/mqtt5_client.c',
    'source/v5/mqtt5_decoder.c',
    'source/v5/mqtt5_encoder.c',
    'source/v5/mqtt5_listener.c',
    'source/v5/mqtt5_options_storage.c',
    'source/v5/mqtt5_to_mqtt3_adapter.c',
    'source/v5/mqtt5_topic_alias.c',
    'source/v5/mqtt5_types.c',
    'source/v5/mqtt5_utils.c',
    'source/v5/rate_limiters.c',
)

inc = include_directories('include')

c_args = ['-DAWS_MQTT_EXPORTS=1']
public_c_args = []
if (get_option('default_library') == 'shared' and host_machine.system() == 'windows') or host_machine.system() != 'windows'
  public_c_args += ['-DAWS_MQTT_USE_IMPORT_EXPORT=1']
endif

libaws_c_mqtt = library(
    'aws-c-mqtt',
    src,
    c_args: c_args + public_c_args,
    include_directories: inc,
    dependencies: [aws_c_common_dep, aws_c_http_dep],
    install: true,
    version: meson.project_version(),
    gnu_symbol_visibility: 'hidden',
)

aws_c_mqtt_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_c_mqtt,
    dependencies: [aws_c_common_dep, aws_c_http_dep],
    compile_args: public_c_args,
)

meson.override_dependency('aws-c-mqtt', aws_c_mqtt_dep)

pkg.generate(
    libaws_c_mqtt,
    extra_cflags: public_c_args,
    description: 'C99 implementation of the MQTT 3.1.1 specification.',
)
