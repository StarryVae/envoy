load(
    "//bazel:envoy_build_system.bzl",
    "envoy_cc_extension",
    "envoy_cc_library",
    "envoy_extension_package",
)

licenses(["notice"])  # Apache 2

envoy_extension_package()

envoy_cc_library(
    name = "filter_lib",
    srcs = [
        "common.cc",
        "context.cc",
        "ffi.cc",
        "filter.cc",
        "vm.cc",
    ],
    hdrs = [
        "capi.h",
        "common.h",
        "context.h",
        "context_interfaces.h",
        "filter.h",
        "vm.h",
    ],
    copts = [
        "-Wno-error=old-style-cast",
    ],
    external_deps = [
        "luajit",
    ],
    repository = "@envoy",
    deps = [
        "@envoy_api//envoy/extensions/filters/http/rider/v3alpha1:pkg_cc_proto",
        "@envoy//envoy/http:codes_interface",
        "@envoy//envoy/http:filter_interface",
        "@envoy//envoy/http:header_map_interface",
        "@envoy//envoy/stream_info:stream_info_interface",
        "@envoy//envoy/thread_local:thread_local_interface",
        "@envoy//envoy/upstream:cluster_manager_interface",
        "@envoy//source/common/buffer:buffer_lib",
        "@envoy//source/common/common:assert_lib",
        "@envoy//source/common/common:c_smart_ptr_lib",
        "@envoy//source/common/common:enum_to_int",
        "@envoy//source/common/config:datasource_lib",
        "@envoy//source/common/config:utility_lib",
        "@envoy//source/common/crypto:utility_lib",
        "@envoy//source/common/http:header_utility_lib",
        "@envoy//source/common/http:message_lib",
        "@envoy//source/common/http:utility_lib",
        "@envoy//source/common/protobuf:message_validator_lib",
        "@envoy//source/common/protobuf:utility_lib",
        "@envoy//source/common/singleton:const_singleton",
        "@envoy//source/extensions/filters/common/lua:wrappers_lib",
        "@envoy//source/extensions/filters/http/common:pass_through_filter_lib",
    ],
)

envoy_cc_extension(
    name = "config_lib",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    copts = [
        "-Wno-error=old-style-cast",
    ],
    repository = "@envoy",
    deps = [
        ":filter_lib",
        "//envoy/registry",
        "//source/extensions/filters/http/common:factory_base_lib",
        "//envoy/server:filter_config_interface",
        "//source/common/http:utility_lib",
    ],
)
