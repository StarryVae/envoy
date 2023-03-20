#pragma once

#include <string>

#include "envoy/http/filter.h"
#include "envoy/http/header_map.h"
#include "envoy/thread_local/thread_local.h"

#include "source/common/common/c_smart_ptr.h"
#include "source/common/common/lock_guard.h"
#include "source/common/common/thread.h"
#include "source/common/http/utility.h"
#include "source/common/singleton/const_singleton.h"
#include "source/extensions/filters/http/rider/common.h"
#include "source/extensions/filters/http/rider/context_interfaces.h"

namespace Envoy {
namespace Proxy {
namespace HttpFilters {
namespace Rider {

class LuaVirtualMachine;
class Filter;

enum class Version { v1, v2 };

/**
 * Plugin is first constructed when LDS init to load data and validate things.
 * Plugin is assigned to a Lua VM when filter config construct.
 */
class Plugin {
public:
  // std::string json;
  Plugin(const std::string& name, const std::string& vm_id, const std::string& code,
         const std::string& configuration)
      : name_(name), vm_id_(vm_id), code_(code), configuration_(configuration),
        key_(name + "||" + configuration) {}

  const std::string& name() const { return name_; }
  const std::string& key() const { return key_; }
  const std::string& code() const { return code_; }
  const std::string& configuration() const { return configuration_; }

private:
  const std::string name_;
  const std::string vm_id_;
  const std::string code_;
  const std::string configuration_;
  std::string log_prefix_;
  std::string key_;
};

using PluginSharedPtr = std::shared_ptr<Plugin>;

// Function names exported by plugin.
class PluginExportFunctionNameValues {
public:
  const std::string OnConfigure = "on_configure";
  const std::string OnRequest = "on_request";
  const std::string OnResponse = "on_response";
};
typedef ConstSingleton<PluginExportFunctionNameValues> PluginExportFunctionNames;

class PluginHandle : public ThreadLocal::ThreadLocalObject {
public:
  PluginHandle(std::shared_ptr<LuaVirtualMachine> vm, std::shared_ptr<Plugin> plugin,
               std::shared_ptr<ContextBase> context_base);

  ~PluginHandle();

  std::shared_ptr<LuaVirtualMachine> vm() { return vm_; }

  const Plugin* plugin() const { return plugin_.get(); }

  int& onConfigureRef() { return on_configure_ref_; }
  int& onRequestRef() { return on_request_ref_; }
  int& onResponseRef() { return on_response_ref_; }

  int& onRequestHeaderRef() { return on_request_header_ref_; }
  int& onRequestBodyRef() { return on_request_body_ref_; }
  int& onResponseHeaderRef() { return on_response_header_ref_; }
  int& onResponseBodyRef() { return on_response_body_ref_; }

  Version& version() { return version_; }

private:
  std::shared_ptr<LuaVirtualMachine> vm_;
  std::shared_ptr<Plugin> plugin_;
  std::shared_ptr<ContextBase> context_base_;

  int on_configure_ref_{LUA_NOREF};
  int on_request_ref_{LUA_NOREF};
  int on_response_ref_{LUA_NOREF};

  int on_request_header_ref_{LUA_NOREF};
  int on_request_body_ref_{LUA_NOREF};
  int on_response_header_ref_{LUA_NOREF};
  int on_response_body_ref_{LUA_NOREF};
  Version version_;
};

using PluginHandleSharedPtr = std::shared_ptr<PluginHandle>;

/***
 * ContextBase contains a collection of interfaces for CallIn(host->lua) and CallOut(lua->host).
 * When a plugin is loaded, a new context will be assigned to it. Filter implements context
 * interfaces to handle requests.
 */
class ContextBase : public RootInterface,
                    public GeneralInterface,
                    public StreamInterface,
                    public StreamInfoInterface,
                    public HeaderInterface,
                    public FilterMetadataInterface,
                    public BodyInterface,
                    public Logger::Loggable<Logger::Id::lua> {
public:
  ContextBase() {}

  // Root Context
  ContextBase(std::shared_ptr<LuaVirtualMachine> vm, PluginSharedPtr plugin);

  // Stream Context
  ContextBase(Filter* filter, std::shared_ptr<LuaVirtualMachine> vm, PluginSharedPtr plugin);

  // RootInterface
  void onConfigure(PluginHandle& plugin_handle) override;
  int getConfiguration(envoy_lua_ffi_str_t* buffer) override;

  // GeneralInterface
  void log(spdlog::level::level_enum level, const char* message) override;
  uint32_t getLogLevel() override { return static_cast<uint32_t>(ENVOY_LOGGER().level()); }
  uint64_t getCurrentTimeMilliseconds() override { PANIC_DUE_TO_CORRUPT_ENUM; }

  // StreamInterface
  void fileLog(const char*, int) override { PANIC_DUE_TO_CORRUPT_ENUM; }
  int getOrCreateSharedTable() override { PANIC_DUE_TO_CORRUPT_ENUM; }
  int getRouteConfiguration(envoy_lua_ffi_str_t*) override { PANIC_DUE_TO_CORRUPT_ENUM; }
  uint64_t getRouteConfigHash() override { PANIC_DUE_TO_CORRUPT_ENUM; }
  int luaBody(lua_State*, StreamDirection) override { PANIC_DUE_TO_CORRUPT_ENUM; }
  int luaHttpCall(lua_State*, StreamDirection) override { PANIC_DUE_TO_CORRUPT_ENUM; }
  int luaRespond(lua_State*, StreamDirection) override { PANIC_DUE_TO_CORRUPT_ENUM; }

  // StreamInfoInterface
  const char* upstreamHost() override { PANIC_DUE_TO_CORRUPT_ENUM; }
  const char* upstreamCluster() override { PANIC_DUE_TO_CORRUPT_ENUM; }
  const char* downstreamLocalAddress() override { PANIC_DUE_TO_CORRUPT_ENUM; }
  const char* downstreamRemoteAddress() override { PANIC_DUE_TO_CORRUPT_ENUM; }
  int64_t startTime() override { PANIC_DUE_TO_CORRUPT_ENUM; }

  // HeaderInterface
  int getHeaderMapSize(LuaStreamOpSourceType) override { PANIC_DUE_TO_CORRUPT_ENUM; }
  int getHeaderMap(LuaStreamOpSourceType, envoy_lua_ffi_string_pairs*) override {
    PANIC_DUE_TO_CORRUPT_ENUM;
  }
  int getHeaderMapValue(LuaStreamOpSourceType, absl::string_view, envoy_lua_ffi_str_t*) override {
    PANIC_DUE_TO_CORRUPT_ENUM;
  }
  int getHeaderMapValueSize(LuaStreamOpSourceType, absl::string_view) override {
    PANIC_DUE_TO_CORRUPT_ENUM;
  }
  int getHeaderMapValueIndex(LuaStreamOpSourceType, absl::string_view, envoy_lua_ffi_str_t*,
                             int) override {
    PANIC_DUE_TO_CORRUPT_ENUM;
  }
  int setHeaderMap(LuaStreamOpSourceType, envoy_lua_ffi_string_pairs*) override {
    PANIC_DUE_TO_CORRUPT_ENUM;
  }
  int setHeaderMapValue(LuaStreamOpSourceType, absl::string_view, absl::string_view) override {
    PANIC_DUE_TO_CORRUPT_ENUM;
  }
  int removeHeaderMapValue(LuaStreamOpSourceType, absl::string_view) override {
    PANIC_DUE_TO_CORRUPT_ENUM;
  }

  int getQueryParameters(envoy_lua_ffi_string_pairs*) override { PANIC_DUE_TO_CORRUPT_ENUM; }

  // FilterMetadataInterface
  int getMetadataValue(envoy_lua_ffi_str_t*, envoy_lua_ffi_str_t*, envoy_lua_ffi_str_t*) override {
    PANIC_DUE_TO_CORRUPT_ENUM;
  }

  int getDynamicMetadataValue(envoy_lua_ffi_str_t*, envoy_lua_ffi_str_t*,
                              envoy_lua_ffi_str_t*) override {
    PANIC_DUE_TO_CORRUPT_ENUM;
  }

  // BodyInterface
  int getBody(LuaStreamOpSourceType, envoy_lua_ffi_str_t*) override {
    PANIC_DUE_TO_CORRUPT_ENUM;
  }

private:
  std::shared_ptr<LuaVirtualMachine> vm_;
  std::shared_ptr<Plugin> plugin_;
};

} // namespace Rider
} // namespace HttpFilters
} // namespace Proxy
} // namespace Envoy
