project('webgui', 'cpp',
    default_options: ['cpp_std=c++20'],
)

subdir('imgui')
subdir('implot')

aws_cpp_sdk_core = dependency('aws-cpp-sdk-core')
aws_cpp_sdk_logs = dependency('aws-cpp-sdk-logs')
libcrypto = dependency('libcrypto')

inc = include_directories('src')

src = files(
    'src/main.cpp',
    'src/platform/aws.cpp'
)

if host_machine.system() == 'windows'
    src += files('src/platform/windows/windows.cpp')
elif host_machine.system() == 'linux'
    src += files('src/platform/linux/linux.cpp')
elif host_machine.system() == 'emscripten'
    src += files(
        'src/platform/emscripten/emscripten.cpp',
        'src/platform/emscripten/emsdk_wget_http.cpp',
    )
elif host_machine.system() == 'darwin'
    src += files('src/platform/darwin/darwin.cpp')
else
    error('Unsupported platform: ' + host_machine.system())
endif

executable('gui', src,
    link_args: ['-lGL', '-sFETCH'],
    include_directories: inc,
    dependencies: [imgui_dep, implot_dep, backend_dep, aws_cpp_sdk_core, aws_cpp_sdk_logs, libcrypto],
    install: true,
    install_dir: get_option('datadir'),
)

install_data('assets/index.html', install_dir: get_option('datadir'))
