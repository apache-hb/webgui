project('prometheus-cpp', 'cpp',
    version: '1.3.0',
    license: 'MIT',
)

pkg = import('pkgconfig')

civetweb_dep = dependency('civetweb')

core_src = files(
    'core/src/detail/ckms_quantiles.cc',
    'core/src/detail/utils.cc',
    'core/src/detail/hash.h',
    'core/src/detail/builder.cc',
    'core/src/detail/time_window_quantiles.cc',
    'core/src/gauge.cc',
    'core/src/check_names.cc',
    'core/src/histogram.cc',
    'core/src/serializer.cc',
    'core/src/info.cc',
    'core/src/family.cc',
    'core/src/text_serializer.cc',
    'core/src/summary.cc',
    'core/src/registry.cc',
    'core/src/counter.cc',
)

inc = include_directories('core/include', 'util/include')

libprometheus_cpp_core = library(
    'prometheus-cpp-core',
    core_src,
    include_directories: inc,
    install: true,
    version: meson.project_version(),
)

prometheus_cpp_core_dep = declare_dependency(
    include_directories: inc,
    link_with: libprometheus_cpp_core,
)

meson.override_dependency('prometheus-cpp-core', prometheus_cpp_core_dep)

pull_src = files(
    'pull/src/basic_auth.cc',
    'pull/src/endpoint.cc',
    'pull/src/exposer.cc',
    'pull/src/handler.cc',
    'pull/src/metrics_collector.cc',
)

inc = include_directories('pull/include', 'util/include')

libprometheus_cpp_pull = library(
    'prometheus-cpp-pull',
    pull_src,
    include_directories: inc,
    dependencies: [civetweb_dep, prometheus_cpp_core_dep],
    install: true,
    version: meson.project_version(),
)

prometheus_cpp_pull_dep = declare_dependency(
    include_directories: inc,
    link_with: libprometheus_cpp_pull,
    dependencies: [prometheus_cpp_core_dep],
)

meson.override_dependency('prometheus-cpp-pull', prometheus_cpp_pull_dep)

curl = dependency('libcurl')

push_src = files(
    'push/src/gateway.cc',
    'push/src/detail/curl_wrapper.cc',
    'push/src/detail/label_encoder.cc',
)

inc = include_directories('push/include', 'util/include')

libprometheus_cpp_push = library(
    'prometheus-cpp-push',
    push_src,
    include_directories: inc,
    dependencies: [prometheus_cpp_core_dep, curl],
    install: true,
    version: meson.project_version(),
)

prometheus_cpp_push_dep = declare_dependency(
    include_directories: inc,
    link_with: libprometheus_cpp_push,
    dependencies: [prometheus_cpp_core_dep],
)

meson.override_dependency('prometheus-cpp-push', prometheus_cpp_push_dep)

pkg.generate(libprometheus_cpp_core)
pkg.generate(libprometheus_cpp_pull)
pkg.generate(libprometheus_cpp_push)
