#!/bin/zsh

ROOT=$(git rev-parse --show-toplevel)

if [[ $? -ne 0 ]]; then
	echo "Git repo not found"
	exit 1
fi

find $ROOT -name "*.h" -or -name "*.cc" -or "*.sh" -or "BUILD" | xargs sed -i -e '$a\'

clang-format -i -style=file $ROOT/**/*.h $ROOT/**/*.cc
buildifier $ROOT/**/BUILD $ROOT/**/BUILD.bazel $ROOT/WORKSPACE
shfmt -l -w $ROOT/**/*.sh
