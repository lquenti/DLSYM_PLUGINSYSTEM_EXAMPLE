#include"header.h"
#include <iostream>

void foo_impl(int x) {
  std::cout << "squared: " << x*x << std::endl;
}

extern "C" {
  Plugin *get_plugin() {
    static Plugin plugin;
    plugin.foo = foo_impl;
    return &plugin;
  }
}
