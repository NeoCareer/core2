#!/bin/zsh

ROOT=$(git rev-parse --show-toplevel)

if [[ $? -ne 0 ]]; then
	echo "Git repo not found"
	exit 1
fi

if ! [[ -e $ROOT/.git/hooks ]]; then
	mkdir $ROOT/.git/hooks
fi

if [[ -e $ROOT/.git/hooks/pre-commit ]]; then
	rm $ROOT/.git/hooks/pre-commit
fi

if ! [[ -e $ROOT/.git/hooks/pre-commit ]]; then
	cp $ROOT/script/pre-commit.sh $ROOT/.git/hooks/pre-commit
fi
