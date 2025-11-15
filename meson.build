project('webgui', 'cpp',
    default_options: ['cpp_std=c++26'],
)

cpp = meson.get_compiler('cpp')

imgui_dep = dependency('imgui')
implot_dep = dependency('implot')
implot3d_dep = dependency('implot3d')

imgui_backend_glfw_dep = dependency('imgui-backend-glfw')
imgui_backend_opengl3_dep = dependency('imgui-backend-opengl3')

aws_cpp_sdk_core = dependency('aws-cpp-sdk-core')
aws_cpp_sdk_logs = dependency('aws-cpp-sdk-logs')
aws_cpp_sdk_iam = dependency('aws-cpp-sdk-iam')
aws_cpp_sdk_sts = dependency('aws-cpp-sdk-sts')
aws_cpp_sdk_ecs = dependency('aws-cpp-sdk-ecs')
libcrypto = dependency('libcrypto')
concurrentqueue = dependency('concurrentqueue')
sqlite3_dep = dependency('sqlite3')

inc = include_directories('src')

src = files(
    'src/main.cpp',
    'src/gui/imaws.cpp',
    'src/gui/aws/session.cpp',
    'src/gui/aws/window.cpp',
    'src/platform/aws.cpp',
)

deps = [
    imgui_dep,
    implot_dep,
    implot3d_dep,
    imgui_backend_glfw_dep,
    imgui_backend_opengl3_dep,
    aws_cpp_sdk_core,
    aws_cpp_sdk_logs,
    aws_cpp_sdk_iam,
    aws_cpp_sdk_sts,
    aws_cpp_sdk_ecs,
    libcrypto,
    concurrentqueue,
    sqlite3_dep,
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

cpp_args = cpp.get_supported_arguments([
    '-Wno-c99-designator'
])

root = meson.project_source_root()

executable('gui', src,
    link_args: [
        '-lGL',
        '-sFETCH',
        '-sEXPORTED_RUNTIME_METHODS=[ccall]',
        '--use-port=contrib.glfw3',
        '-lidbfs.js',
        '--pre-js', join_paths(root, 'assets', 'pre.js'),
        '--preload-file', join_paths(root, 'assets', 'fonts') + '@/fonts',
    ],
    include_directories: inc,
    cpp_args: cpp_args + [ '--use-port=contrib.glfw3' ],
    dependencies: deps,
    install: true,
    install_dir: get_option('datadir'),
)

install_data('assets/index.html', install_dir: get_option('datadir'))
install_data(meson.project_build_root() / 'gui.data', install_dir: get_option('datadir'))
