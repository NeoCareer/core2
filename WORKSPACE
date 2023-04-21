workspace(name = "com_neoclear_core2")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_googletest",
    sha256 = "983a7f2f4cc2a4d75d94ee06300c46a657291fba965e355d11ab3b6965a7b0e5",
    strip_prefix = "googletest-b796f7d44681514f58a683a3a71ff17c94edb0c1",
    urls = ["https://github.com/google/googletest/archive/b796f7d44681514f58a683a3a71ff17c94edb0c1.zip"],
)

http_archive(
    name = "bazel_skylib",
    sha256 = "b8a1527901774180afc798aeb28c4634bdccf19c4d98e7bdd1ce79d1fe9aaad7",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.4.1/bazel-skylib-1.4.1.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.4.1/bazel-skylib-1.4.1.tar.gz",
    ],
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

http_archive(
    name = "com_google_absl",
    strip_prefix = "abseil-cpp-a69b0ae5cdba53a45617afc408618a3e1ac244de",
    sha256 = "d091c4da2c1d51f52e7d37fb4a6c6e8be3cc4f5ddf9f1de50754be2f6c992818",
    urls = ["https://github.com/abseil/abseil-cpp/archive/a69b0ae5cdba53a45617afc408618a3e1ac244de.zip"],
)

benchmark_version = "1.7.1"

http_archive(
    name = "com_github_google_benchmark",
    sha256 = "6430e4092653380d9dc4ccb45a1e2dc9259d581f4866dc0759713126056bc1d7",
    strip_prefix = "benchmark-%s" % benchmark_version,
    urls = ["https://github.com/google/benchmark/archive/refs/tags/v%s.tar.gz" % benchmark_version],
)
