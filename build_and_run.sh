#!/bin/bash
set -ex
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
pushd $SCRIPT_DIR
g++ -ldl -std=c++20 main.cc -o main
g++ -fPIC -shared -std=c++20 cc_plugin.cc -o cc_plugin.so
gcc -fPIC -shared -std=c11 c_plugin.c -o c_plugin.so
rustc --crate-type cdylib rust_plugin.rs -o rust_plugin.so
./main ./c_plugin.so ./cc_plugin.so ./rust_plugin.so
popd
