// NOLINT(namespace-envoy)
#include <string>
#include <string_view>
#include <unordered_map>


#include "proxy_wasm_intrinsics.h"


class ExampleRootContext : public RootContext {
public:
  explicit ExampleRootContext(uint32_t id, std::string_view root_id) : RootContext(id, root_id) {}


  bool onStart(size_t) override;
  bool onConfigure(size_t) override;
  void onTick() override;
};


class ExampleContext : public Context {
public:
  explicit ExampleContext(uint32_t id, RootContext* root) : Context(id, root) {}


  void onCreate() override;
  FilterHeadersStatus onRequestHeaders(uint32_t headers, bool end_of_stream) override;
  FilterDataStatus onRequestBody(size_t body_buffer_length, bool end_of_stream) override;
  FilterHeadersStatus onResponseHeaders(uint32_t headers, bool end_of_stream) override;
  FilterDataStatus onResponseBody(size_t body_buffer_length, bool end_of_stream) override;
  void onDone() override;
  void onLog() override;
  void onDelete() override;
};
static RegisterContextFactory register_ExampleContext(CONTEXT_FACTORY(ExampleContext),
                                                      ROOT_FACTORY(ExampleRootContext),
                                                      "my_root_id");


bool ExampleRootContext::onStart(size_t) { return true; }


bool ExampleRootContext::onConfigure(size_t) { return true; }


void ExampleRootContext::onTick() {}


void ExampleContext::onCreate() {}


FilterHeadersStatus ExampleContext::onRequestHeaders(uint32_t, bool) {
  std::string fake_header_key_prefix = "fake_key_0";


  // Add 20 headers to request headers
  for (size_t i = 0; i < 20; i++) {
    addRequestHeader(fake_header_key_prefix, "hahahaha");
  }


  // Check 20 times headers.
  for (size_t i = 0; i < 30; i++) {
    auto header = getRequestHeader("fake_key_0");
    if (std::string(header->view()) != "hahahaha") {
      LOG_ERROR("header mistach!");
    }
  }

  return FilterHeadersStatus::Continue;
}


FilterHeadersStatus ExampleContext::onResponseHeaders(uint32_t, bool) {
  return FilterHeadersStatus::Continue;
}


FilterDataStatus ExampleContext::onRequestBody(size_t, bool /* end_of_stream */) {


  return FilterDataStatus::Continue;
}


FilterDataStatus ExampleContext::onResponseBody(size_t body_buffer_length,
                                                bool /* end_of_stream */) {
  for (int i = 0; i < 30; i++) {
    auto body = getBufferBytes(WasmBufferType::HttpResponseBody, 0, body_buffer_length);
  }
  return FilterDataStatus::Continue;
}


void ExampleContext::onDone() {}


void ExampleContext::onLog() {}


void ExampleContext::onDelete() {}
