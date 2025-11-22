project('aws-c-mqtt', 'c', version: '0.13.3', license: 'Apache-2.0', meson_version: '>=0.63.0')

pkg = import('pkgconfig')

aws_c_common_dep = dependency('aws-c-common')
aws_c_http_dep = dependency('aws-c-http')

src = files(
    'source/mqtt.c'
)

inc = include_directories('include')

c_args = ['-DAWS_MQTT_EXPORTS=1']
public_c_args = []
if get_option('default_library') == 'shared'
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
