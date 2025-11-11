project('webgui', 'cpp',
    default_options: ['cpp_std=c++20'],
)

subdir('imgui')

src = files('src/main.cpp')

aws_cpp_sdk_core = dependency('aws-cpp-sdk-core')
aws_cpp_sdk_logs = dependency('aws-cpp-sdk-logs')
libcrypto = dependency('libcrypto')

executable('gui', src,
    link_args: ['-lGL', '-sFETCH'],
    dependencies: [imgui_dep, backend_dep, aws_cpp_sdk_core, aws_cpp_sdk_logs, libcrypto],
    install: true,
    install_dir: get_option('datadir'),
)

install_data('assets/index.html', install_dir: get_option('datadir'))
