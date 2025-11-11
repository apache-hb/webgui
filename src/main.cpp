#include "aws/cal/hash.h"
#include "aws/logs/model/DescribeLogGroupsRequest.h"
#include "openssl/hmac.h"
#include <condition_variable>
#include <thread>
#include <barrier>
#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/fetch.h>
#include <emscripten/wget.h>
#endif

#define GLFW_INCLUDE_ES3
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#include <openssl/crypto.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <aws/cal/cal.h>
#include <aws/cal/hmac.h>

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentials.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/http/HttpClient.h>
#include <aws/core/http/HttpClientFactory.h>
#include <aws/core/http/standard/StandardHttpRequest.h>
#include <aws/core/http/standard/StandardHttpResponse.h>
#include <aws/logs/CloudWatchLogsClient.h>
#include <aws/logs/CloudWatchLogsServiceClientModel.h>

// Global variables - the window needs to be passed in to imgui
GLFWwindow *g_window;

// Global variables - these can be edited in the demo
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
bool show_demo_window = true;
bool show_another_window = false;

Aws::CloudWatchLogs::Model::DescribeLogGroupsOutcome g_describeLogGroupsOutcome;

void loop() {
  glfwPollEvents();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

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

  ImGui::Render();

  int display_w, display_h;
  glfwMakeContextCurrent(g_window);
  glfwGetFramebufferSize(g_window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwMakeContextCurrent(g_window);
}

int init_gl() {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return 1;
  }

  glfwWindowHint(GLFW_OPENGL_PROFILE,
                 GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

  // Open a window and create its OpenGL context.
  // The window is created with minimal size,
  // which will be updated with an automatic resize.
  // You could get the primary viewport size here to create.
  g_window = glfwCreateWindow(1, 1, "WebGui Demo", NULL, NULL);
  if (g_window == NULL) {
    fprintf(stderr, "Failed to open GLFW window.\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(g_window); // Initialize GLEW

  return 0;
}

int init_imgui() {
  // Setup Dear ImGui binding
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(g_window, true);
  ImGui_ImplGlfw_InstallEmscriptenCallbacks(g_window, "#canvas");
  ImGui_ImplOpenGL3_Init("#version 300 es");

  // Setup style
  ImGui::StyleColorsDark();

  ImGuiIO &io = ImGui::GetIO();

  // Disable loading of imgui.ini file
  io.IniFilename = nullptr;

  // Load Fonts
  io.Fonts->AddFontDefault();

  return 0;
}

int init() {
  init_gl();
  init_imgui();
  return 0;
}

void quit() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(g_window);
  glfwTerminate();
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

extern "C" struct aws_hmac *
aws_sha256_hmac_default_new(struct aws_allocator *allocator,
                            const struct aws_byte_cursor *secret);

struct MySha256HmacImpl {};

static void s_destroy(struct aws_hmac *hmac) {
  if (hmac == NULL) {
    return;
  }

  HMAC_CTX *ctx = (HMAC_CTX *)hmac->impl;
  if (ctx != NULL) {
    HMAC_CTX_free(ctx);
  }

  aws_mem_release(hmac->allocator, hmac);
}

static int s_update(struct aws_hmac *hmac,
                    const struct aws_byte_cursor *to_hmac) {
  if (!hmac->good) {
    return aws_raise_error(AWS_ERROR_INVALID_STATE);
  }

  HMAC_CTX *ctx = (HMAC_CTX *)hmac->impl;

  if (AWS_LIKELY(HMAC_Update(ctx, to_hmac->ptr, to_hmac->len))) {
    return AWS_OP_SUCCESS;
  }

  hmac->good = false;
  return aws_raise_error(AWS_ERROR_INVALID_ARGUMENT);
}

static int s_finalize(struct aws_hmac *hmac, struct aws_byte_buf *output) {
  if (!hmac->good) {
    return aws_raise_error(AWS_ERROR_INVALID_STATE);
  }

  HMAC_CTX *ctx = (HMAC_CTX *)hmac->impl;

  size_t buffer_len = output->capacity - output->len;

  if (buffer_len < hmac->digest_size) {
    return aws_raise_error(AWS_ERROR_SHORT_BUFFER);
  }

  if (AWS_LIKELY(HMAC_Final(ctx, output->buffer + output->len,
                            (unsigned int *)&buffer_len))) {
    hmac->good = false;
    output->len += hmac->digest_size;
    return AWS_OP_SUCCESS;
  }

  hmac->good = false;
  return aws_raise_error(AWS_ERROR_INVALID_ARGUMENT);
}

static struct aws_hmac_vtable s_sha256_hmac_vtable = {
    .destroy = s_destroy,
    .update = s_update,
    .finalize = s_finalize,
    .alg_name = "SHA256 HMAC",
    .provider = "OpenSSL Compatible libcrypto",
};

struct aws_hmac *my_sha256_hmac_new(struct aws_allocator *allocator,
                                    const struct aws_byte_cursor *secret) {

  struct aws_hmac *hmac =
      (aws_hmac *)aws_mem_acquire(allocator, sizeof(struct aws_hmac));

  hmac->allocator = allocator;
  hmac->vtable = &s_sha256_hmac_vtable;
  hmac->digest_size = AWS_SHA256_HMAC_LEN;
  HMAC_CTX *ctx = NULL;
  ctx = HMAC_CTX_new();

  if (!ctx) {
    aws_raise_error(AWS_ERROR_CAL_CRYPTO_OPERATION_FAILED);
    aws_mem_release(allocator, hmac);
    return NULL;
  }

  hmac->impl = ctx;
  hmac->good = true;

  if (!HMAC_Init_ex(ctx, secret->ptr, secret->len, EVP_sha256(), NULL)) {
    s_destroy(hmac);
    aws_raise_error(AWS_ERROR_INVALID_ARGUMENT);
    return NULL;
  }

  return hmac;
}

static void s_destroy_digest(struct aws_hash *hash) {
  if (hash == NULL) {
    return;
  }

  EVP_MD_CTX *ctx = (EVP_MD_CTX *)hash->impl;
  if (ctx != NULL) {
    EVP_MD_CTX_free(ctx);
  }

  aws_mem_release(hash->allocator, hash);
}

static int s_update_digest(struct aws_hash *hash,
                           const struct aws_byte_cursor *to_hash) {
  if (!hash->good) {
    return aws_raise_error(AWS_ERROR_INVALID_STATE);
  }

  EVP_MD_CTX *ctx = (EVP_MD_CTX *)hash->impl;

  if (AWS_LIKELY(EVP_DigestUpdate(ctx, to_hash->ptr, to_hash->len))) {
    return AWS_OP_SUCCESS;
  }

  hash->good = false;
  return aws_raise_error(AWS_ERROR_INVALID_ARGUMENT);
}

static int s_finalize_digest(struct aws_hash *hash,
                             struct aws_byte_buf *output) {
  if (!hash->good) {
    return aws_raise_error(AWS_ERROR_INVALID_STATE);
  }

  EVP_MD_CTX *ctx = (EVP_MD_CTX *)hash->impl;

  size_t buffer_len = output->capacity - output->len;

  if (buffer_len < hash->digest_size) {
    return aws_raise_error(AWS_ERROR_SHORT_BUFFER);
  }

  if (AWS_LIKELY(EVP_DigestFinal_ex(ctx, output->buffer + output->len,
                                    (unsigned int *)&buffer_len))) {
    output->len += hash->digest_size;
    hash->good = false;
    return AWS_OP_SUCCESS;
  }

  hash->good = false;
  return aws_raise_error(AWS_ERROR_INVALID_ARGUMENT);
}

static struct aws_hash_vtable s_md5_vtable = {
    .destroy = s_destroy_digest,
    .update = s_update_digest,
    .finalize = s_finalize_digest,
    .alg_name = "MD5",
    .provider = "OpenSSL Compatible libcrypto",
};

static struct aws_hash_vtable s_sha512_vtable = {
    .destroy = s_destroy_digest,
    .update = s_update_digest,
    .finalize = s_finalize_digest,
    .alg_name = "SHA512",
    .provider = "OpenSSL Compatible libcrypto",
};

static struct aws_hash_vtable s_sha256_vtable = {
    .destroy = s_destroy_digest,
    .update = s_update_digest,
    .finalize = s_finalize_digest,
    .alg_name = "SHA256",
    .provider = "OpenSSL Compatible libcrypto",
};

static struct aws_hash_vtable s_sha1_vtable = {
    .destroy = s_destroy_digest,
    .update = s_update_digest,
    .finalize = s_finalize_digest,
    .alg_name = "SHA1",
    .provider = "OpenSSL Compatible libcrypto",
};

struct aws_hash *my_md5_new(struct aws_allocator *allocator) {
  struct aws_hash *hash =
      (aws_hash *)aws_mem_acquire(allocator, sizeof(struct aws_hash));

  hash->allocator = allocator;
  hash->vtable = &s_md5_vtable;
  hash->digest_size = AWS_MD5_LEN;
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  hash->impl = ctx;
  hash->good = true;

  AWS_FATAL_ASSERT(hash->impl);

  if (!EVP_DigestInit_ex(ctx, EVP_md5(), NULL)) {
    s_destroy_digest(hash);
    aws_raise_error(AWS_ERROR_UNKNOWN);
    return NULL;
  }

  return hash;
}

struct aws_hash *my_sha512_default_new(struct aws_allocator *allocator) {
  struct aws_hash *hash =
      (aws_hash *)aws_mem_acquire(allocator, sizeof(struct aws_hash));

  hash->allocator = allocator;
  hash->vtable = &s_sha512_vtable;
  hash->digest_size = AWS_SHA512_LEN;
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  hash->impl = ctx;
  hash->good = true;

  AWS_FATAL_ASSERT(hash->impl);

  if (!EVP_DigestInit_ex(ctx, EVP_sha512(), NULL)) {
    s_destroy_digest(hash);
    aws_raise_error(AWS_ERROR_UNKNOWN);
    return NULL;
  }

  return hash;
}

struct aws_hash *my_sha256_default_new(struct aws_allocator *allocator) {
  struct aws_hash *hash =
      (aws_hash *)aws_mem_acquire(allocator, sizeof(struct aws_hash));

  hash->allocator = allocator;
  hash->vtable = &s_sha256_vtable;
  hash->digest_size = AWS_SHA256_LEN;
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  hash->impl = ctx;
  hash->good = true;

  AWS_FATAL_ASSERT(hash->impl);

  if (!EVP_DigestInit_ex(ctx, EVP_sha256(), NULL)) {
    s_destroy_digest(hash);
    aws_raise_error(AWS_ERROR_UNKNOWN);
    return NULL;
  }

  return hash;
}

struct aws_hash *my_sha1_default_new(struct aws_allocator *allocator) {
  struct aws_hash *hash =
      (aws_hash *)aws_mem_acquire(allocator, sizeof(struct aws_hash));

  hash->allocator = allocator;
  hash->vtable = &s_sha1_vtable;
  hash->digest_size = AWS_SHA1_LEN;
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  hash->impl = ctx;
  hash->good = true;

  AWS_FATAL_ASSERT(hash->impl);

  if (!EVP_DigestInit_ex(ctx, EVP_sha1(), NULL)) {
    s_destroy_digest(hash);
    aws_raise_error(AWS_ERROR_UNKNOWN);
    return NULL;
  }

  return hash;
}

int main(int argc, char **argv) {
  if (init() != 0)
    return 1;

  OPENSSL_init_crypto(
      OPENSSL_INIT_ADD_ALL_CIPHERS | OPENSSL_INIT_ADD_ALL_DIGESTS, NULL);
  auto alloc = aws_default_allocator();
  aws_set_sha256_hmac_new_fn(my_sha256_hmac_new);
  aws_cal_library_init(alloc);
  aws_set_md5_new_fn(my_md5_new);
  aws_set_sha256_new_fn(my_sha256_default_new);
  aws_set_sha512_new_fn(my_sha512_default_new);
  aws_set_sha1_new_fn(my_sha1_default_new);

  Aws::SDKOptions options;
  options.httpOptions.httpClientFactory_create_fn = []() {
    return Aws::MakeShared<EmsdkWgetClientFactory>(
        "EmsdkWgetClientFactoryAllocationTag");
  };

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

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(loop, 0, 1);
#endif

  quit();

  Aws::ShutdownAPI(options);

  return 0;
}