#include "emscripten.hpp"

#include "util/defer.hpp"

#include <emscripten/fetch.h>
#include <emscripten/wget.h>

#include <aws/core/Aws.h>
#include <aws/core/http/HttpClient.h>
#include <aws/core/http/HttpClientFactory.h>
#include <aws/core/http/standard/StandardHttpRequest.h>
#include <aws/core/http/standard/StandardHttpResponse.h>

using namespace Aws::Http;
using namespace Aws::Client;

using Aws::Utils::RateLimits::RateLimiterInterface;

using sm::Platform_Emscripten;

namespace {
    bool equals_ignore_case(const char* a, const char* b) {
        while (*a && *b) {
            if (tolower(*a) != tolower(*b)) {
                return false;
            }
            ++a;
            ++b;
        }
        return *a == *b;
    }

    class EmsdkWgetHttpClient final : public HttpClient {
        ClientConfiguration mClientConfig;

        static void copy_content_body(std::vector<char>& dst, Aws::IOStream& body) {
            body.seekg(0, std::ios::end);
            size_t size = body.tellg();
            body.seekg(0, std::ios::beg);
            dst.resize(size);
            body.read(dst.data(), size);
        }

        static std::vector<char*> create_headers(const HeaderValueCollection& source) {
            std::vector<char*> headers;
            for (const auto& header : source) {
                // These cannot be set manually
                if (equals_ignore_case(header.first.c_str(), "Host") || equals_ignore_case(header.first.c_str(), "User-Agent") || equals_ignore_case(header.first.c_str(), "Content-Length")) {
                    continue;
                }
                headers.push_back(strdup(header.first.c_str()));
                headers.push_back(strdup(header.second.c_str()));
            }
            headers.push_back(nullptr);
            return headers;
        }

        static void free_headers(std::vector<char*> headers) {
            for (const auto& header : headers) {
                if (header != nullptr) {
                    free((void*)header);
                }
            }
        }

    public:
        EmsdkWgetHttpClient(const ClientConfiguration &clientConfiguration)
            : mClientConfig(clientConfiguration)
        { }

        std::shared_ptr<HttpResponse> MakeRequest(
            const std::shared_ptr<HttpRequest> &request,
            RateLimiterInterface *readLimiter = nullptr,
            RateLimiterInterface *writeLimiter = nullptr
        ) const override {
            std::vector<char> requestBody;

            if (auto body = request->GetContentBody()) {
                copy_content_body(requestBody, *body);
            }

            std::vector headers = create_headers(request->GetHeaders());
            defer { free_headers(headers); };

            emscripten_fetch_attr_t attr{};
            emscripten_fetch_attr_init(&attr);

            strncpy(
                attr.requestMethod,
                HttpMethodMapper::GetNameForHttpMethod(request->GetMethod()),
                sizeof(attr.requestMethod) - 1);

            attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
            attr.timeoutMSecs = static_cast<unsigned int>(mClientConfig.requestTimeoutMs);
            attr.requestData = requestBody.data();
            attr.requestDataSize = requestBody.size();
            attr.requestHeaders = headers.data();

            emscripten_fetch_t *fetch = emscripten_fetch(&attr, request->GetUri().GetURIString().c_str());

            defer { emscripten_fetch_close(fetch); };

            auto response = Aws::MakeShared<Standard::StandardHttpResponse>("EmsdkWgetHttpClientAllocationTag", request);
            response->SetResponseCode(static_cast<HttpResponseCode>(fetch->status));
            response->SetOriginatingRequest(request);

            if (fetch->status == 200) {
                size_t headerCount = emscripten_fetch_get_response_headers_length(fetch);
                std::unique_ptr<char[]> responseHeadersPacked{new char[headerCount + 1]};
                emscripten_fetch_get_response_headers(fetch, responseHeadersPacked.get(), headerCount + 1);

                char **responseHeaders = emscripten_fetch_unpack_response_headers(responseHeadersPacked.get());
                defer { emscripten_fetch_free_unpacked_response_headers(responseHeaders); };

                size_t i = 0;
                while (true) {
                    char *key = responseHeaders[i++];
                    if (!key) {
                        break;
                    }

                    char *value = responseHeaders[i++];
                    response->AddHeader(Aws::String(key), Aws::String(value));
                }

                response->GetResponseBody().write(fetch->data, fetch->numBytes);
            }

            return response;
        }
    };

    class EmsdkWgetClientFactory final : public HttpClientFactory {
    public:
        std::shared_ptr<HttpClient>
        CreateHttpClient(const ClientConfiguration &clientConfiguration) const override {
            return Aws::MakeShared<EmsdkWgetHttpClient>("EmsdkWgetHttpClientAllocationTag", clientConfiguration);
        }

        std::shared_ptr<HttpRequest>
        CreateHttpRequest(const Aws::String &uri, HttpMethod method, const Aws::IOStreamFactory &streamFactory) const override {
            auto request = Aws::MakeShared<Standard::StandardHttpRequest>("StandardHttpRequestAllocationTag", uri, method);
            request->SetResponseStreamFactory(streamFactory);
            return request;
        }

        std::shared_ptr<HttpRequest>
        CreateHttpRequest(const URI &uri, HttpMethod method, const Aws::IOStreamFactory &streamFactory) const override {
            auto request = Aws::MakeShared<Standard::StandardHttpRequest>("StandardHttpRequestAllocationTag", uri, method);
            request->SetResponseStreamFactory(streamFactory);
            return request;
        }
    };
}

void Platform_Emscripten::configure_aws_sdk_options(Aws::SDKOptions& options) {
    options.httpOptions.httpClientFactory_create_fn = [] {
        return Aws::MakeShared<EmsdkWgetClientFactory>("EmsdkWgetClientFactoryAllocationTag");
    };
}
