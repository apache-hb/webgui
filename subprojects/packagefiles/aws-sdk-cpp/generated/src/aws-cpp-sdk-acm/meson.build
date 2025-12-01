src = files(
    'source/ACMRequest.cpp',
    'source/ACMClient.cpp',
    'source/ACMEndpointRules.cpp',
    'source/ACMErrorMarshaller.cpp',
    'source/ACMErrors.cpp',
    'source/ACMEndpointProvider.cpp',
    'source/model/CertificateOptions.cpp',
    'source/model/ExtendedKeyUsageName.cpp',
    'source/model/RevocationReason.cpp',
    'source/model/RenewalSummary.cpp',
    'source/model/GetCertificateRequest.cpp',
    'source/model/CertificateDetail.cpp',
    'source/model/GetAccountConfigurationResult.cpp',
    'source/model/KeyAlgorithm.cpp',
    'source/model/ResourceRecord.cpp',
    'source/model/RemoveTagsFromCertificateRequest.cpp',
    'source/model/CertificateExport.cpp',
    'source/model/DomainStatus.cpp',
    'source/model/RequestCertificateResult.cpp',
    'source/model/AddTagsToCertificateRequest.cpp',
    'source/model/SortBy.cpp',
    'source/model/RenewalEligibility.cpp',
    'source/model/DescribeCertificateResult.cpp',
    'source/model/ExportCertificateRequest.cpp',
    'source/model/FailureReason.cpp',
    'source/model/DeleteCertificateRequest.cpp',
    'source/model/RevokeCertificateResult.cpp',
    'source/model/ListCertificatesResult.cpp',
    'source/model/DescribeCertificateRequest.cpp',
    'source/model/SortOrder.cpp',
    'source/model/ExtendedKeyUsage.cpp',
    'source/model/RenewCertificateRequest.cpp',
    'source/model/CertificateTransparencyLoggingPreference.cpp',
    'source/model/CertificateSummary.cpp',
    'source/model/ListTagsForCertificateResult.cpp',
    'source/model/ValidationMethod.cpp',
    'source/model/CertificateManagedBy.cpp',
    'source/model/PutAccountConfigurationRequest.cpp',
    'source/model/RevokeCertificateRequest.cpp',
    'source/model/GetAccountConfigurationRequest.cpp',
    'source/model/CertificateStatus.cpp',
    'source/model/RecordType.cpp',
    'source/model/ListCertificatesRequest.cpp',
    'source/model/GetCertificateResult.cpp',
    'source/model/ExpiryEventsConfiguration.cpp',
    'source/model/RenewalStatus.cpp',
    'source/model/HttpRedirect.cpp',
    'source/model/KeyUsage.cpp',
    'source/model/DomainValidationOption.cpp',
    'source/model/ResendValidationEmailRequest.cpp',
    'source/model/DomainValidation.cpp',
    'source/model/UpdateCertificateOptionsRequest.cpp',
    'source/model/ImportCertificateRequest.cpp',
    'source/model/CertificateType.cpp',
    'source/model/ListTagsForCertificateRequest.cpp',
    'source/model/Filters.cpp',
    'source/model/KeyUsageName.cpp',
    'source/model/ImportCertificateResult.cpp',
    'source/model/RequestCertificateRequest.cpp',
    'source/model/Tag.cpp',
    'source/model/ExportCertificateResult.cpp'
)

inc = include_directories('include')

if host_machine.system() == 'windows' and get_option('default_library') == 'shared'
    cpp_args = ['-DAWS_ACM_EXPORTS=1']
else
    cpp_args = []
endif

libaws_cpp_sdk_acm = library(
    'aws-cpp-sdk-acm',
    src,
    include_directories: inc,
    cpp_args: cpp_args,
    dependencies: [aws_cpp_sdk_core_dep],
    install: true,
    version: meson.project_version(),
)

aws_cpp_sdk_acm_dep = declare_dependency(
    include_directories: inc,
    link_with: libaws_cpp_sdk_acm,
    dependencies: [aws_cpp_sdk_core_dep],
)

meson.override_dependency('aws-cpp-sdk-acm', aws_cpp_sdk_acm_dep)

pkgs += {
    'aws-cpp-sdk-acm': {
        'lib': libaws_cpp_sdk_acm,
        'extra_cflags': public_cpp_args,
    }
}
