#!/bin/bash
set -ex
g++ -ldl -std=c++20 main.cc -o main
g++ -fPIC -shared -std=c++20 cc_plugin.cc -o cc_plugin.so
gcc -fPIC -shared -std=c11 c_plugin.c -o c_plugin.so
