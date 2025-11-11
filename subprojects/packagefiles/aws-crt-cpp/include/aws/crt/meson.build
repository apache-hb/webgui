cfg_data = configuration_data()

version = meson.project_version()
parts = version.split('.')

cfg_data.set('FULL_VERSION', version)
cfg_data.set('PROJECT_VERSION_MAJOR', parts[0])
cfg_data.set('PROJECT_VERSION_MINOR', parts[1])
cfg_data.set('PROJECT_VERSION_PATCH', parts[2])
cfg_data.set('GIT_HASH', '35dad3a8131dc9d761a2ccfc24808308d5ce4680')

configure_file(
    input: 'Config.h.in',
    output: 'Config.h',
    configuration: cfg_data,
    install: true,
    install_dir: get_option('includedir') / 'aws/crt',
    format: 'cmake',
)
