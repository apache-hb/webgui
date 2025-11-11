project('webgui', 'cpp')

subdir('imgui')

src = files('src/main.cpp')

aws_cpp_sdk_core = dependency('aws-cpp-sdk-core')
aws_cpp_sdk_logs = dependency('aws-cpp-sdk-logs')

executable('gui', src,
    link_args: ['-lGL'],
    dependencies: [imgui_dep, backend_dep, aws_cpp_sdk_core, aws_cpp_sdk_logs],
    install: true,
    install_dir: get_option('datadir'),
)

install_data('assets/index.html', install_dir: get_option('datadir'))
