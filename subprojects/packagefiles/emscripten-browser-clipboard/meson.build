project('emscripten-browser-clipboard', 'cpp')

inc = include_directories('.')

emscripten_browser_clipboard_dep = declare_dependency(
    include_directories: inc,
)

meson.override_dependency('emscripten-browser-clipboard', emscripten_browser_clipboard_dep)
