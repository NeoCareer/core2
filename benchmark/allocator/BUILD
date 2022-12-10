load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "trading",
    srcs = ["trading.cc"],
    deps = [
        "//:core2",
        "@com_github_google_benchmark//:benchmark_main",
        "@com_google_absl//absl/strings:str_format",
    ],
)
