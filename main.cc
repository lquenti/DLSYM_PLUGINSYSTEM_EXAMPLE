#include <cassert>
#include <cstdlib>
#include <dlfcn.h>
#include <iostream>
#include <memory>

#include"header.h"


int main(int argc, char **argv) {
  assert(argc > 1);
  for (int i{1}; i<argc; ++i) {
    assert(argv[i]); // If null, then we silently look at ourselves
    std::cout << "loading " << argv[i] << std::endl;

    auto *handle{dlopen(argv[i], RTLD_LAZY)};
    if (!handle) {
      std::cerr << "dlopen failed with: " << dlerror() << std::endl;
      return EXIT_FAILURE;
    }

    auto *get_plugin{reinterpret_cast<Plugin *(*)(void)>(dlsym(handle, "get_plugin"))};
    if (!get_plugin) {
      std::cerr << "dlsym (get_plugin) failed with: " << dlerror() << std::endl;
      dlclose(handle);
      return EXIT_FAILURE;
    }

    Plugin *plugin{get_plugin()};
    plugin->foo(4);
    dlclose(handle);
  }
}
