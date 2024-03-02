#!/bin/bash

# a script to generate compile_commands.json compilation db. this is crucial to make clangd work correctly
# TODO: this is a trivial task to do that does not require a whole python package to be installed do the fucking work.

selfdir="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
rootdir="$selfdir/.."
pushd rootdir

echo "install compiledb"
pip install compiledb

compiledb make -n

popd
