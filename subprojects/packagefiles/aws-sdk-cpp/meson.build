project('aws-sdk-cpp', 'cpp',
    version: '0.13.3',
    license: 'Apache-2.0',
    meson_version: '>=0.63.0',
    default_options: [
        'cpp_eh=none',
        'cpp_rtti=false',
    ]
)

pkg = import('pkgconfig')
cpp = meson.get_compiler('cpp')

aws_crt_cpp_dep = dependency('aws-crt-cpp')

public_cpp_args = []
if (get_option('default_library') == 'shared' and host_machine.system() == 'windows') or host_machine.system() != 'windows'
    public_cpp_args += ['-DUSE_IMPORT_EXPORT=1']
endif

pkgs = {}

subdir('src/aws-cpp-sdk-core')

subdir('generated/src/aws-cpp-sdk-account')
subdir('generated/src/aws-cpp-sdk-acm')
subdir('generated/src/aws-cpp-sdk-cognito-identity')
subdir('generated/src/aws-cpp-sdk-dynamodb')
subdir('generated/src/aws-cpp-sdk-ec2')
subdir('generated/src/aws-cpp-sdk-ec2-instance-connect')
subdir('generated/src/aws-cpp-sdk-ecs')
subdir('generated/src/aws-cpp-sdk-elasticloadbalancing')
subdir('generated/src/aws-cpp-sdk-elasticloadbalancingv2')
subdir('generated/src/aws-cpp-sdk-iam')
subdir('generated/src/aws-cpp-sdk-kms')
subdir('generated/src/aws-cpp-sdk-logs')
subdir('generated/src/aws-cpp-sdk-monitoring')
subdir('generated/src/aws-cpp-sdk-route53')
subdir('generated/src/aws-cpp-sdk-s3')
subdir('generated/src/aws-cpp-sdk-sts')

subdir('src/aws-cpp-sdk-identity-management')

foreach name, config : pkgs
    pkg.generate(
        config['lib'],
        extra_cflags: config['extra_cflags'],
        description: config.get('description', 'AWS C++ SDK for ' + name),
    )
endforeach
