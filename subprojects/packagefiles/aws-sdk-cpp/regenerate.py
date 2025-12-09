from glob import glob

def generate_meson_for_client(name: str, export: str, path: str):
    fullname = 'aws-cpp-sdk-' + name
    underscored_name = fullname.replace('-', '_')
    prefix = f'../../aws-sdk-cpp-1.11.706/generated/src/aws-cpp-sdk-{name}/'
    sources = [f.removeprefix(prefix) for f in glob(f'{prefix}source/**/*.cpp', recursive=True)]
    with open(f'generated/src/aws-cpp-sdk-{name}/meson.build', 'w') as f:
        f.write('src = files(\n')
        for source in sources:
            f.write(f"    '{source}',\n")
        f.write(')\n')
        f.write(f'''
inc = include_directories('include')

if host_machine.system() == 'windows' and get_option('default_library') == 'shared'
    cpp_args = ['-DAWS_{export}_EXPORTS=1']
else
    cpp_args = []
endif

lib{underscored_name} = library(
    'aws-cpp-sdk-{name}',
    src,
    include_directories: inc,
    cpp_args: cpp_args + public_cpp_args,
    dependencies: [aws_cpp_sdk_core_dep],
    install: true,
    version: meson.project_version(),
)

{underscored_name}_dep = declare_dependency(
    include_directories: inc,
    link_with: lib{underscored_name},
    dependencies: [aws_cpp_sdk_core_dep],
    compile_args: public_cpp_args,
)

meson.override_dependency('aws-cpp-sdk-{name}', {underscored_name}_dep)

pkgs += {{
    'aws-cpp-sdk-{name}': {{
        'lib': lib{underscored_name},
        'extra_cflags': public_cpp_args,
    }}
}}
''')

if __name__ == '__main__':
    generate_meson_for_client('account', 'ACCOUNT', 'aws-cpp-sdk-account')
    generate_meson_for_client('acm', 'ACM', 'aws-cpp-sdk-acm')
    generate_meson_for_client('cognito-identity', 'COGNITOIDENTITY', 'aws-cpp-sdk-cognito-identity')
    generate_meson_for_client('dynamodb', 'DYNAMODB', 'aws-cpp-sdk-dynamodb')
    generate_meson_for_client('ec2', 'EC2', 'aws-cpp-sdk-ec2')
    generate_meson_for_client('ec2-instance-connect', 'EC2INSTANCECONNECT', 'aws-cpp-sdk-ec2-instance-connect')
    generate_meson_for_client('ecs', 'ECS', 'aws-cpp-sdk-ecs')
    generate_meson_for_client('elasticloadbalancing', 'ELASTICLOADBALANCING', 'aws-cpp-sdk-elasticloadbalancing')
    generate_meson_for_client('elasticloadbalancingv2', 'ELASTICLOADBALANCINGV2', 'aws-cpp-sdk-elasticloadbalancingv2')
    generate_meson_for_client('iam', 'IAM', 'aws-cpp-sdk-iam')
    generate_meson_for_client('kms', 'KMS', 'aws-cpp-sdk-kms')
    generate_meson_for_client('logs', 'CLOUDWATCHLOGS', 'aws-cpp-sdk-logs')
    generate_meson_for_client('monitoring', 'CLOUDWATCH', 'aws-cpp-sdk-monitoring')
    generate_meson_for_client('route53', 'ROUTE53', 'aws-cpp-sdk-route53')
    generate_meson_for_client('s3', 'S3', 'aws-cpp-sdk-s3')
    generate_meson_for_client('sts', 'STS', 'aws-cpp-sdk-sts')
