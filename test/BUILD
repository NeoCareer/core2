load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "VectorTest",
    size = "small",
    srcs = ["VectorTest.cc"],
    deps = [
        "//lib:corelib",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "FifoTest",
    srcs = ["FifoTest.cc"],
    deps = [
        "//lib:corelib",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "SpinLockTest",
    srcs = ["SpinLockTest.cc"],
    deps = [
        "//lib:corelib",
        "@com_google_googletest//:gtest_main",
    ],
)
