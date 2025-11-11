#include "platform/generic.hpp"
#include <thread>
#include <barrier>
#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/fetch.h>
#include <emscripten/wget.h>
#endif

#include "platform/platform.hpp"
#include "platform/aws.hpp"

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentials.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/http/HttpClient.h>
#include <aws/core/http/HttpClientFactory.h>
#include <aws/core/http/standard/StandardHttpRequest.h>
#include <aws/core/http/standard/StandardHttpResponse.h>
#include <aws/logs/CloudWatchLogsClient.h>
#include <aws/logs/CloudWatchLogsServiceClientModel.h>
#include <aws/logs/model/DescribeLogGroupsRequest.h>

// Global variables - the window needs to be passed in to imgui
GLFWwindow *g_window;

// Global variables - these can be edited in the demo
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
bool show_demo_window = true;
bool show_another_window = false;

Aws::CloudWatchLogs::Model::DescribeLogGroupsOutcome g_describeLogGroupsOutcome;

void loop() {
  sm::Platform::begin();

  // 1. Show a simple window.
  // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically
  // appears in a window called "Debug".
  {
    static float f = 0.0f;
    static int counter = 0;
    ImGui::Text(
        "Hello, world!"); // Display some text (you can use a format string too)
    ImGui::SliderFloat("float", &f, 0.0f,
                       1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3(
        "clear color",
        (float *)&clear_color); // Edit 3 floats representing a color

    ImGui::Checkbox(
        "Demo Window",
        &show_demo_window); // Edit bools storing our windows open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    if (ImGui::Button("Button")) // Buttons return true when clicked (NB: most
                                 // widgets return true when edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    if (g_describeLogGroupsOutcome.IsSuccess()) {
      const auto &logGroups =
          g_describeLogGroupsOutcome.GetResult().GetLogGroups();
      ImGui::Text("Log Groups:");
      for (const auto &logGroup : logGroups) {
        ImGui::BulletText("%s", logGroup.GetLogGroupName().c_str());
      }
    } else {
      ImGui::Text("Failed to describe log groups: %s",
                  g_describeLogGroupsOutcome.GetError().GetMessage().c_str());
    }
  }

  // 2. Show another simple window. In most cases you will use an explicit
  // Begin/End pair to name your windows.
  if (show_another_window) {
    ImGui::Begin("Another Window", &show_another_window);
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Close Me"))
      show_another_window = false;
    ImGui::End();
  }

  // 3. Show the ImGui demo window. Most of the sample code is in
  // ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
  if (show_demo_window) {
    ImGui::SetNextWindowPos(
        ImVec2(650, 20),
        ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call
                                 // this because positions are saved in .ini
                                 // file anyway. Here we just want to make the
                                 // demo initial state a bit more friendly!
    ImGui::ShowDemoWindow(&show_demo_window);
  }

  ImPlot::ShowDemoWindow();

  sm::Platform::end();
}

class EmsdkWgetHttpClient final : public Aws::Http::HttpClient {
  Aws::Client::ClientConfiguration mClientConfig;

public:
  EmsdkWgetHttpClient(
      const Aws::Client::ClientConfiguration &clientConfiguration)
      : mClientConfig(clientConfiguration) {}

  std::shared_ptr<Aws::Http::HttpResponse> MakeRequest(
      const std::shared_ptr<Aws::Http::HttpRequest> &request,
      Aws::Utils::RateLimits::RateLimiterInterface *readLimiter = nullptr,
      Aws::Utils::RateLimits::RateLimiterInterface *writeLimiter =
          nullptr) const override {

    std::vector<char> requestBody;

    if (auto body = request->GetContentBody()) {
      body->seekg(0, std::ios::end);
      size_t size = body->tellg();
      body->seekg(0, std::ios::beg);
      requestBody.resize(size);
      body->read(requestBody.data(), size);
    }

    std::vector<const char *> headers;
    for (const auto &header : request->GetHeaders()) {
      std::cout << "Header: " << header.first << " = " << header.second
                << std::endl;
      headers.push_back(strdup(header.first.c_str()));
      headers.push_back(strdup(header.second.c_str()));
    }
    headers.push_back(nullptr);

    auto freeHeaders = [&headers]() {
      for (const auto &header : headers) {
        free((void *)header);
      }
    };

    for (const auto &header : headers) {
      std::cout << "Entry: " << (header ? header : "NULL") << std::endl;
    }

    emscripten_fetch_attr_t attr{};
    emscripten_fetch_attr_init(&attr);
    strncpy(
        attr.requestMethod,
        Aws::Http::HttpMethodMapper::GetNameForHttpMethod(request->GetMethod()),
        sizeof(attr.requestMethod) - 1);
    attr.attributes =
        EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
    attr.timeoutMSecs =
        static_cast<unsigned int>(mClientConfig.requestTimeoutMs);
    attr.requestData = requestBody.data();
    attr.requestDataSize = requestBody.size();
    attr.requestHeaders = headers.data();

    emscripten_fetch_t *fetch =
        emscripten_fetch(&attr, request->GetUri().GetURIString().c_str());

    printf("id: %u, data: %p, size: %d, status: %d\n", fetch->id, fetch->data, (int)fetch->numBytes, fetch->status);
    if (fetch->status == 200) {
      auto response =
          Aws::MakeShared<Aws::Http::Standard::StandardHttpResponse>(
              "EmsdkWgetHttpClientAllocationTag", request);

      response->SetResponseCode(
          static_cast<Aws::Http::HttpResponseCode>(fetch->status));

      size_t headerCount = emscripten_fetch_get_response_headers_length(fetch);
      std::unique_ptr<char[]> responseHeadersPacked{new char[headerCount + 1]};
      emscripten_fetch_get_response_headers(fetch, responseHeadersPacked.get(),
                                            headerCount + 1);
      char **responseHeaders =
          emscripten_fetch_unpack_response_headers(responseHeadersPacked.get());

      size_t i = 0;
      while (true) {
        char *key = responseHeaders[i++];
        if (!key) {
          break;
        }

        char *value = responseHeaders[i++];
        response->AddHeader(Aws::String(key), Aws::String(value));
      }

      emscripten_fetch_free_unpacked_response_headers(responseHeaders);

      response->SetOriginatingRequest(request);
      response->GetResponseBody().write(fetch->data, fetch->numBytes);

      emscripten_fetch_close(fetch);
      freeHeaders();
      return response;
    } else {
      printf("EmsdkWgetHttpClient request failed with status code %d\n",
             fetch->status);

      auto response =
          Aws::MakeShared<Aws::Http::Standard::StandardHttpResponse>(
              "EmsdkWgetHttpClientAllocationTag", request);

      response->SetResponseCode(
          static_cast<Aws::Http::HttpResponseCode>(fetch->status));
      response->SetOriginatingRequest(request);
      emscripten_fetch_close(fetch);
      freeHeaders();
      return response;
    }
  }
};

class EmsdkWgetClientFactory final : public Aws::Http::HttpClientFactory {

public:
  std::shared_ptr<Aws::Http::HttpClient>
  CreateHttpClient(const Aws::Client::ClientConfiguration &clientConfiguration)
      const override {
    return Aws::MakeShared<EmsdkWgetHttpClient>(
        "EmsdkWgetHttpClientAllocationTag", clientConfiguration);
  }

  std::shared_ptr<Aws::Http::HttpRequest>
  CreateHttpRequest(const Aws::String &uri, Aws::Http::HttpMethod method,
                    const Aws::IOStreamFactory &streamFactory) const override {
    auto request = Aws::MakeShared<Aws::Http::Standard::StandardHttpRequest>(
        "StandardHttpRequestAllocationTag", uri, method);
    request->SetResponseStreamFactory(streamFactory);
    return request;
  }

  std::shared_ptr<Aws::Http::HttpRequest>
  CreateHttpRequest(const Aws::Http::URI &uri, Aws::Http::HttpMethod method,
                    const Aws::IOStreamFactory &streamFactory) const override {
    auto request = Aws::MakeShared<Aws::Http::Standard::StandardHttpRequest>(
        "StandardHttpRequestAllocationTag", uri, method);
    request->SetResponseStreamFactory(streamFactory);
    return request;
  }
};

int main(int argc, char **argv) {
  sm::PlatformCreateInfo createInfo {
    .title = "AWS C++ WASM",
    .clear = {0.45f, 0.55f, 0.60f, 1.00f}
  };

  if (int err = sm::Platform::setup(createInfo)) {
    return err;
  }

  sm::aws_init_byo_crypto();

  Aws::SDKOptions options;
  sm::Platform::configure_aws_sdk_options(options);

  Aws::InitAPI(options);

  std::barrier barrier{2};

  std::thread worker([&barrier]() {
    Aws::Auth::AWSCredentials credentials{
        "Secret", "Secret"};

    auto provider = Aws::MakeShared<Aws::Auth::SimpleAWSCredentialsProvider>(
        "AwsAllocationTag", credentials);

    Aws::Client::ClientConfigurationInitValues clientConfigInitValues;
    clientConfigInitValues.shouldDisableIMDS = true;

    Aws::CloudWatchLogs::CloudWatchLogsClientConfiguration config{
        clientConfigInitValues};
    config.region = "us-east-1";
    Aws::CloudWatchLogs::CloudWatchLogsClient cwlClient{provider, config};

    Aws::CloudWatchLogs::Model::DescribeLogGroupsRequest request;
    request.SetLimit(5);
    g_describeLogGroupsOutcome = cwlClient.DescribeLogGroups(request);
    barrier.arrive_and_wait();
  });


  barrier.arrive_and_wait();

  sm::Platform::run(loop);

  sm::Platform::finalize();

  Aws::ShutdownAPI(options);

  return 0;
}