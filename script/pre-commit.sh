#!/bin/zsh

ROOT=$(git rev-parse --show-toplevel)

if [[ $? -ne 0 ]]; then
	echo "Git repo not found"
	exit 1
fi

if [[ -e $ROOT/bin/play.cc ]]; then
	rm $ROOT/bin/play.cc
fi

echo """#include \"core2/Core2.h\"

using namespace std;
using namespace core2;

// clang-format off
int main() {
  return 0;
}
// clang-format on""" >$ROOT/bin/play.cc

# Return 0 means no need to format
# Return 1 means need format cpp code
# Return 2 means need format bazel config
# Return 3 means need format shell script
format_type() {
	file=$1
	if [[ $file == *.cc || $file == *.c || $file == *.cpp || $file == *.h || $file == *.hpp ]]; then
		return 1
	fi

	if [[ $file == BUILD || $file == *.bazel || $file == WORKSPACE ]]; then
		return 2
	fi

	if [[ $file == *.sh ]]; then
		return 3
	fi

	return 0
}

for file in $(git diff-index --cached --diff-filter=d --name-only HEAD); do
	format_type ${file}
	type=$?
	filepath=$ROOT/${file}
	if [[ $type -eq 1 ]]; then
		# Format C/C++ code
		clang-format -i -style=file $filepath
	fi
	if [[ $type -eq 2 ]]; then
		buildifier $filepath
	fi

	if [[ $type -eq 3 ]]; then
		shfmt -l -w $filepath
	fi

	git add $filepath
done
