project('opentelemetry-cpp', 'cpp',
    version: '1.24.0',
    license: 'Apache-2.0',
)

cpp = meson.get_compiler('cpp')
pkg = import('pkgconfig')

prometheus_dep = dependency('prometheus-cpp-pull')
rapidyaml_dep = dependency('rapidyaml')

src = files(
    'sdk/src/logs/logger_config.cc',
    'sdk/src/logs/multi_log_record_processor_factory.cc',
    'sdk/src/logs/provider.cc',
    'sdk/src/logs/logger_provider_factory.cc',
    'sdk/src/logs/logger_context_factory.cc',
    'sdk/src/logs/batch_log_record_processor_factory.cc',
    'sdk/src/logs/event_logger.cc',
    'sdk/src/logs/multi_recordable.cc',
    'sdk/src/logs/logger.cc',
    'sdk/src/logs/batch_log_record_processor.cc',
    'sdk/src/logs/event_logger_provider_factory.cc',
    'sdk/src/logs/exporter.cc',
    'sdk/src/logs/simple_log_record_processor_factory.cc',
    'sdk/src/logs/logger_context.cc',
    'sdk/src/logs/multi_log_record_processor.cc',
    'sdk/src/logs/logger_provider.cc',
    'sdk/src/logs/readable_log_record.cc',
    'sdk/src/logs/simple_log_record_processor.cc',
    'sdk/src/logs/read_write_log_record.cc',
    'sdk/src/logs/batch_log_record_processor_options.cc',
    'sdk/src/logs/event_logger_provider.cc',
    'sdk/src/version/version.cc',
    'sdk/src/metrics/meter_config.cc',
    'sdk/src/metrics/view/view_registry_factory.cc',
    'sdk/src/metrics/view/instrument_selector_factory.cc',
    'sdk/src/metrics/view/view_factory.cc',
    'sdk/src/metrics/view/meter_selector_factory.cc',
    'sdk/src/metrics/meter_context_factory.cc',
    'sdk/src/metrics/multi_observer_result.cc',
    'sdk/src/metrics/provider.cc',
    'sdk/src/metrics/meter_context.cc',
    'sdk/src/metrics/meter.cc',
    'sdk/src/metrics/instrument_metadata_validator.cc',
    'sdk/src/metrics/metric_reader.cc',
    'sdk/src/metrics/meter_provider.cc',
    'sdk/src/metrics/data/circular_buffer.cc',
    'sdk/src/metrics/meter_provider_factory.cc',
    'sdk/src/metrics/aggregation/base2_exponential_histogram_indexer.cc',
    'sdk/src/metrics/aggregation/base2_exponential_histogram_aggregation.cc',
    'sdk/src/metrics/aggregation/lastvalue_aggregation.cc',
    'sdk/src/metrics/aggregation/histogram_aggregation.cc',
    'sdk/src/metrics/aggregation/sum_aggregation.cc',
    'sdk/src/metrics/exemplar/reservoir.cc',
    'sdk/src/metrics/state/filtered_ordered_attribute_map.cc',
    'sdk/src/metrics/state/sync_metric_storage.cc',
    'sdk/src/metrics/state/metric_collector.cc',
    'sdk/src/metrics/state/observable_registry.cc',
    'sdk/src/metrics/state/temporal_metric_storage.cc',
    'sdk/src/metrics/async_instruments.cc',
    'sdk/src/metrics/sync_instruments.cc',
    'sdk/src/metrics/export/periodic_exporting_metric_reader.cc',
    'sdk/src/metrics/export/periodic_exporting_metric_reader_options.cc',
    'sdk/src/metrics/export/periodic_exporting_metric_reader_factory.cc',
    'sdk/src/resource/resource_detector.cc',
    'sdk/src/resource/resource.cc',
    'sdk/src/configuration/sdk_builder.cc',
    'sdk/src/configuration/configuration_parser.cc',
    'sdk/src/configuration/document_node.cc',
    'sdk/src/configuration/yaml_configuration_parser.cc',
    'sdk/src/configuration/ryml_document_node.cc',
    'sdk/src/configuration/registry.cc',
    'sdk/src/configuration/ryml_document.cc',
    'sdk/src/configuration/configured_sdk.cc',
    'sdk/src/trace/random_id_generator.cc',
    'sdk/src/trace/batch_span_processor.cc',
    'sdk/src/trace/simple_processor_factory.cc',
    'sdk/src/trace/batch_span_processor_options.cc',
    'sdk/src/trace/provider.cc',
    'sdk/src/trace/batch_span_processor_factory.cc',
    'sdk/src/trace/random_id_generator_factory.cc',
    'sdk/src/trace/span.cc',
    'sdk/src/trace/tracer_config.cc',
    'sdk/src/trace/tracer_provider_factory.cc',
    'sdk/src/trace/tracer_context_factory.cc',
    'sdk/src/trace/samplers/always_off_factory.cc',
    'sdk/src/trace/samplers/trace_id_ratio.cc',
    'sdk/src/trace/samplers/always_on_factory.cc',
    'sdk/src/trace/samplers/trace_id_ratio_factory.cc',
    'sdk/src/trace/samplers/parent.cc',
    'sdk/src/trace/samplers/parent_factory.cc',
    'sdk/src/trace/exporter.cc',
    'sdk/src/trace/tracer.cc',
    'sdk/src/trace/tracer_provider.cc',
    'sdk/src/trace/tracer_context.cc',
    'sdk/src/common/base64.cc',
    'sdk/src/common/env_variables.cc',
    'sdk/src/common/global_log_handler.cc',
    'sdk/src/common/disabled.cc',
    'sdk/src/common/random.cc',
)

if host_machine.system() == 'windows'
    src += files('sdk/src/common/platform/fork_windows.cc')
elif host_machine.system() == 'linux'
    src += files('sdk/src/common/platform/fork_unix.cc')
endif

inc = include_directories('api/include', 'sdk/include')
private_inc = include_directories('sdk')

cpp_args = cpp.get_supported_arguments(
    '-Wno-deprecated-declarations'
)

libopentelemetry_cpp = library(
    'opentelemetry-cpp',
    src,
    cpp_args: cpp_args,
    include_directories: [inc, private_inc],
    dependencies: [prometheus_dep, rapidyaml_dep],
    install: true,
    version: meson.project_version(),
)

opentelemetry_cpp_dep = declare_dependency(
    include_directories: inc,
    link_with: libopentelemetry_cpp,
)

meson.override_dependency('opentelemetry-cpp', opentelemetry_cpp_dep)

pkg.generate(libopentelemetry_cpp)

# memory exporter

inc = include_directories('exporters/memory/include')

src = files(
    'exporters/memory/src/in_memory_metric_data.cc',
    'exporters/memory/src/in_memory_metric_exporter_factory.cc',
    'exporters/memory/src/in_memory_span_exporter_factory.cc',
)

libopentelemetry_memory_exporter = library(
    'opentelemetry_memory_exporter',
    src,
    cpp_args: cpp_args,
    include_directories: [inc, private_inc],
    dependencies: [opentelemetry_cpp_dep],
    install: true,
    version: meson.project_version(),
)

opentelemetry_memory_exporter_dep = declare_dependency(
    include_directories: inc,
    link_with: libopentelemetry_memory_exporter,
)

meson.override_dependency('opentelemetry-cpp-memory-exporter', opentelemetry_memory_exporter_dep)

pkg.generate(libopentelemetry_memory_exporter)

# ostream exporter

inc = include_directories('exporters/ostream/include')

src = files(
    'exporters/ostream/src/span_exporter.cc',
    'exporters/ostream/src/log_record_exporter_factory.cc',
    'exporters/ostream/src/console_log_record_builder.cc',
    'exporters/ostream/src/console_push_metric_builder.cc',
    'exporters/ostream/src/log_record_exporter.cc',
    'exporters/ostream/src/metric_exporter_factory.cc',
    'exporters/ostream/src/metric_exporter.cc',
    'exporters/ostream/src/console_span_builder.cc',
    'exporters/ostream/src/span_exporter_factory.cc',
)

libopentelemetry_ostream_exporter = library(
    'opentelemetry_ostream_exporter',
    src,
    cpp_args: cpp_args,
    include_directories: [inc, private_inc],
    dependencies: [opentelemetry_cpp_dep],
    install: true,
    version: meson.project_version(),
)

opentelemetry_ostream_exporter_dep = declare_dependency(
    include_directories: inc,
    link_with: libopentelemetry_ostream_exporter,
)

meson.override_dependency('opentelemetry-cpp-ostream-exporter', opentelemetry_ostream_exporter_dep)

pkg.generate(libopentelemetry_ostream_exporter)

# prometheus exporter

inc = include_directories('exporters/prometheus/include')

src = files(
    'exporters/prometheus/src/prometheus_pull_builder.cc',
    'exporters/prometheus/src/exporter_options.cc',
    'exporters/prometheus/src/exporter_factory.cc',
    'exporters/prometheus/src/exporter_utils.cc',
    'exporters/prometheus/src/collector.cc',
    'exporters/prometheus/src/exporter.cc',
)

libopentelemetry_prometheus_exporter = library(
    'opentelemetry_prometheus_exporter',
    src,
    cpp_args: cpp_args,
    include_directories: [inc, private_inc],
    dependencies: [opentelemetry_cpp_dep, prometheus_dep],
    install: true,
    version: meson.project_version(),
)

opentelemetry_prometheus_exporter_dep = declare_dependency(
    include_directories: inc,
    link_with: libopentelemetry_prometheus_exporter,
)

meson.override_dependency('opentelemetry-cpp-prometheus-exporter', opentelemetry_prometheus_exporter_dep)

pkg.generate(libopentelemetry_prometheus_exporter)
