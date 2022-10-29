#!/bin/bash

clang-format -i -style=file **/*.h **/*.cc
buildifier **/BUILD **/BUILD.bazel WORKSPACE
