workspace(name = "com_neoclear_core2")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_googletest",
    strip_prefix = "googletest-609281088cfefc76f9d0ce82e1ff6c30cc3591e5",
    urls = ["https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip"],
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
    urls = ["https://github.com/abseil/abseil-cpp/archive/a69b0ae5cdba53a45617afc408618a3e1ac244de.zip"],
)

benchmark_version = "1.6.1"

http_archive(
    name = "com_github_google_benchmark",
    sha256 = "6132883bc8c9b0df5375b16ab520fac1a85dc9e4cf5be59480448ece74b278d4",
    strip_prefix = "benchmark-%s" % benchmark_version,
    urls = ["https://github.com/google/benchmark/archive/refs/tags/v%s.tar.gz" % benchmark_version],
)
