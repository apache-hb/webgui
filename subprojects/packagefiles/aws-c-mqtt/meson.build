project('aws-c-mqtt', 'c', version: '0.13.3', license: 'Apache-2.0', meson_version: '>=0.63.0')

aws_c_common_dep = dependency('aws-c-common')
aws_c_http_dep = dependency('aws-c-http')

src = files(
    'source/mqtt.c'
)

inc = include_directories('include')

libaws_c_mqtt = library(
    'aws-c-mqtt',
    src,
    include_directories: inc,
    install: true,
    dependencies: [aws_c_common_dep, aws_c_http_dep],
    version: meson.project_version(),
)

aws_c_mqtt_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_c_mqtt,
    dependencies: [aws_c_common_dep, aws_c_http_dep],
)

meson.override_dependency('aws-c-mqtt', aws_c_mqtt_dep)
