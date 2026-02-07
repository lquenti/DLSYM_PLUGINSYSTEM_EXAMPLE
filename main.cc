#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#include"header.h"

template <typename T, auto fn>
using c_unique_ptr = std::unique_ptr<T, decltype([](T* ptr) { fn(ptr); })>;

// deleters should return void
using LibHandle = c_unique_ptr<void, [](void *handle){printf("duck\n");dlclose(handle);}>;

class PluginInstance {
  LibHandle m_lib;

  // Raw pointer as we can't take ownership (for example, if they put it in static storage)
  Plugin *m_plugin;

  PluginInstance(LibHandle l, Plugin *p) : m_lib(std::move(l)), m_plugin(p) {}

public:
  PluginInstance(const PluginInstance &) = delete;
  PluginInstance &operator=(const PluginInstance &) = delete;
  PluginInstance(PluginInstance &&) = default;
  PluginInstance &operator=(PluginInstance &&) = default;

  static std::optional<PluginInstance> Load(const char *path) {
    LibHandle lib(dlopen(path, RTLD_LAZY));
    if (!lib) {
      std::cerr << "dlopen failed with: " << dlerror() << std::endl;
      return std::nullopt;
    }
    dlerror(); // clear

    auto *get_plugin_func = reinterpret_cast<Plugin *(*)(void)>(dlsym(lib.get(), "get_plugin"));
    char* error = dlerror();
    if (error || !get_plugin_func) {
        std::cerr << "  ERROR: Symbol missing: " << (error ? error : "NULL") << "\n";
        return std::nullopt;
    }

    return PluginInstance(std::move(lib), get_plugin_func());
  }

  Plugin *operator->() const {
    return m_plugin;
  }

  explicit operator bool() const {
    return m_plugin != nullptr;
  }
};

int main(int argc, char **argv) {
  assert(argc > 1);
  std::vector<PluginInstance> plugins;
  for (int i{1}; i<argc; ++i) {
    std::cout << "Loading " << argv[i] << "..." << std::endl;

    auto loaded = PluginInstance::Load(argv[i]);
    if (!loaded) {
      return EXIT_FAILURE;
    }
    plugins.push_back(std::move(*loaded));
  }

  for (const auto &p: plugins) {
    p->foo(4);
  }

  printf("Before clear\n");
  plugins.clear();
  printf("After clear\n");
  return EXIT_SUCCESS;
}
