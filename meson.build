project('webgui', 'cpp',
    default_options: ['cpp_std=c++20'],
)

subdir('imgui')
subdir('implot')

aws_cpp_sdk_core = dependency('aws-cpp-sdk-core')
aws_cpp_sdk_logs = dependency('aws-cpp-sdk-logs')
aws_cpp_sdk_sts = dependency('aws-cpp-sdk-sts')
aws_cpp_sdk_ecs = dependency('aws-cpp-sdk-ecs')
libcrypto = dependency('libcrypto')
concurrentqueue = dependency('concurrentqueue')

inc = include_directories('src')

src = files(
    'src/main.cpp',
    'src/platform/aws.cpp'
)

deps = [
    imgui_dep,
    implot_dep,
    backend_dep,
    aws_cpp_sdk_core,
    aws_cpp_sdk_logs,
    aws_cpp_sdk_sts,
    aws_cpp_sdk_ecs,
    libcrypto,
    concurrentqueue
]

if host_machine.system() == 'windows'
    src += files('src/platform/windows/windows.cpp')
elif host_machine.system() == 'linux'
    src += files('src/platform/linux/linux.cpp')
elif host_machine.system() == 'emscripten'
    clipboard_dep = dependency('emscripten-browser-clipboard')
    deps += [clipboard_dep]
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
    link_args: ['-lGL', '-sFETCH', '-sEXPORTED_RUNTIME_METHODS=[ccall]', '-lidbfs.js', '-sASYNCIFY', '-sASYNCIFY_IMPORTS=[emscripten_sleep]'],
    include_directories: inc,
    dependencies: deps,
    install: true,
    install_dir: get_option('datadir'),
)

install_data('assets/index.html', install_dir: get_option('datadir'))
