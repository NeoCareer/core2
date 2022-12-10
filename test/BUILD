load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "VectorTest",
    size = "small",
    srcs = ["VectorTest.cc"],
    deps = [
        "//:core2",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "FifoTest",
    srcs = ["FifoTest.cc"],
    deps = [
        "//:core2",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "SpinLockTest",
    srcs = ["SpinLockTest.cc"],
    deps = [
        "//:core2",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "TradingAllocatorTest",
    srcs = ["TradingAllocatorTest.cc"],
    deps = [
        "//:core2",
        "@com_google_googletest//:gtest_main",
    ],
)
