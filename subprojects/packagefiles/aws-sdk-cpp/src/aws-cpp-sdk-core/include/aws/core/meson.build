sdkconfig_cdata = configuration_data()
sdkconfig_cdata.set('USE_AWS_MEMORY_MANAGEMENT', 0)

configure_file(
    input: 'SDKConfig.h.in',
    output: 'SDKConfig.h',
    configuration: sdkconfig_cdata,
    install: true,
    install_dir: get_option('includedir') / 'aws/core',
    format: 'cmake',
)
