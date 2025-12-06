src = files(
    'source/EC2InstanceConnectEndpointRules.cpp',
    'source/EC2InstanceConnectClient.cpp',
    'source/EC2InstanceConnectErrors.cpp',
    'source/EC2InstanceConnectRequest.cpp',
    'source/EC2InstanceConnectErrorMarshaller.cpp',
    'source/EC2InstanceConnectEndpointProvider.cpp',
    'source/model/SendSSHPublicKeyResult.cpp',
    'source/model/SendSSHPublicKeyRequest.cpp',
    'source/model/SendSerialConsoleSSHPublicKeyRequest.cpp',
    'source/model/SendSerialConsoleSSHPublicKeyResult.cpp',
)

inc = include_directories('include')

if host_machine.system() == 'windows' and get_option('default_library') == 'shared'
    cpp_args = ['-DAWS_EC2INSTANCECONNECT_API=1']
else
    cpp_args = []
endif

libaws_cpp_sdk_ec2_instance_connect = library(
    'aws-cpp-sdk-ec2-instance-connect',
    src,
    include_directories: inc,
    cpp_args: cpp_args,
    dependencies: [aws_cpp_sdk_core_dep],
    install: true,
    version: meson.project_version(),
)

aws_cpp_sdk_ec2_instance_connect_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_cpp_sdk_ec2_instance_connect,
    dependencies: [aws_cpp_sdk_core_dep],
)

meson.override_dependency('aws-cpp-sdk-ec2-instance-connect', aws_cpp_sdk_ec2_instance_connect_dep)

pkgs += {
    'aws-cpp-sdk-ec2-instance-connect': {
        'lib': libaws_cpp_sdk_ec2_instance_connect,
        'extra_cflags': public_cpp_args,
    }
}
