project('aws-checksums', 'c', version: '0.2.7', license: 'Apache-2.0', meson_version: '>=0.63.0')

pkg = import('pkgconfig')

cc = meson.get_compiler('c')
aws_c_common_dep = dependency('aws-c-common')

deps = [aws_c_common_dep]

src = files(
    'source/checksums.c',
    'source/crc_sw.c',
    'source/crc32.c',
    'source/crc64.c',
    'source/crc64_sw.c',
)

x86_src = files(
    'source/intel/asm/crc32c_sse42_asm.c',
    'source/intel/intrin/crc64nvme_clmul.c',
    'source/intel/intrin/crc32c_sse42_avx512.c',
    'source/intel/intrin/crc64nvme_avx512.c',
)

arm_src = files(
    'source/arm/crc32c_arm.c',
    'source/arm/crc64_arm.c'
)

c_args = cc.get_supported_arguments('-mpclmul', '-mavx512f', '-mavx512vl', '-mvpclmulqdq', '-mavx2', '/arch:AVX512')

if host_machine.cpu_family() in ['x86', 'x86_64']
    src += x86_src
elif host_machine.cpu_family() in ['aarch64', 'arm']
    src += arm_src
endif

inc = include_directories('include')

c_args += ['-DAWS_CHECKSUMS_EXPORTS=1']
public_c_args = []
if get_option('default_library') == 'shared'
  public_c_args += ['-DAWS_CHECKSUMS_USE_IMPORT_EXPORT=1']
endif

libaws_c_sdkutils = library(
    'aws-checksums',
    src,
    c_args: c_args + public_c_args,
    include_directories: inc,
    dependencies: deps,
    install: true,
    version: meson.project_version(),
    gnu_symbol_visibility: 'hidden',
)

aws_c_sdkutils_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_c_sdkutils,
    dependencies: deps,
    compile_args: public_c_args,
)

meson.override_dependency('aws-checksums', aws_c_sdkutils_dep)

pkg.generate(
    libaws_c_sdkutils,
    extra_cflags: public_c_args,
    description: 'Cross-Platform HW accelerated CRC32c and CRC32 with fallback to efficient SW implementations. C interface with language bindings for each of our SDKs',
)
