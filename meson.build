project('webgui', 'cpp')

subdir('imgui')

src = files('src/main.cpp')

aws_c_common_dep = dependency('aws-c-common')

executable('gui', src,
    link_args: ['-lGL'],
    dependencies: [imgui_dep, backend_dep, aws_c_common_dep],
    install: true,
    install_dir: get_option('datadir'),
)

install_data('assets/index.html', install_dir: get_option('datadir'))
