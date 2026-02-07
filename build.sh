#!/bin/bash
set -ex
g++ -ldl -std=c++20 main.cc -o main
g++ -fPIC -shared -std=c++20 plugin.cc -o cc_plugin.so
