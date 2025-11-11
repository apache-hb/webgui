project('aws-checksums',
'c', version: '0.13.3', license: 'Apache-2.0', meson_version: '>=0.63.0')

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

if host_machine.cpu_family() in ['x86', 'x86_64']
    src += x86_src
elif host_machine.cpu_family() in ['aarch64', 'arm']
    src += arm_src
endif

inc = include_directories('include')

libaws_c_sdkutils = library(
    'aws-checksums',
    src,
    include_directories: inc,
    install: true,
    dependencies: deps,
    version: meson.project_version(),
)

aws_c_sdkutils_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_c_sdkutils,
    dependencies: deps,
)

meson.override_dependency('aws-checksums', aws_c_sdkutils_dep)
