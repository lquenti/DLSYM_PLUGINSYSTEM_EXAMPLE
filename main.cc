#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "header.h"

// https://www.learncpp.com/cpp-tutorial/non-type-template-parameters/
template <typename T, auto fn>
using c_unique_ptr = std::unique_ptr<T, decltype([](T* ptr) { fn(ptr); })>;

// deleters should return void
using LibHandle = c_unique_ptr<void, [](void *handle){dlclose(handle);}>;

// TODO write comment explaining reasoning
class ArrowChainProxy {
  Plugin* m_api;
  public:
  ArrowChainProxy(Plugin* api, void *ctx) : m_api(api) {
    if (m_api && m_api->bind) {
      m_api->bind(ctx);
    }
  }

  ~ArrowChainProxy() {
    if (m_api && m_api->bind) {
      m_api->bind(nullptr);
    }
  }

  Plugin* operator->() {
    return m_api;
  }
};

class PluginInstance {
  LibHandle m_lib;

  Plugin *m_plugin;  // raw ptr since we dont have ownership
  void *m_ctx;

  PluginInstance(LibHandle l, Plugin *p, void *ctx) : m_lib(std::move(l)), m_plugin(p), m_ctx(ctx) {}

  public:
  PluginInstance(const PluginInstance &) = delete;
  PluginInstance &operator=(const PluginInstance &) = delete;

  PluginInstance(PluginInstance &&other) noexcept
        : m_lib(std::move(other.m_lib)),
          m_plugin(std::exchange(other.m_plugin, nullptr)),
          m_ctx(std::exchange(other.m_ctx, nullptr)) {}

  PluginInstance &operator=(PluginInstance &&other) noexcept {
    if (this != &other) {
      std::swap(m_lib, other.m_lib);
      std::swap(m_plugin, other.m_plugin);
      std::swap(m_ctx, other.m_ctx);
    }
    return *this;
  }

  ~PluginInstance() {
    if (m_plugin) {
      m_plugin->destroy(m_ctx);
    }
  }

  static std::optional<PluginInstance> Load(const char *path) {
    LibHandle lib(dlopen(path, RTLD_LAZY));
    if (!lib) {
      std::cerr << "dlopen failed with: " << dlerror() << std::endl;
      return std::nullopt;
    }
    dlerror(); // clear

    auto *get_plugin_func = reinterpret_cast<Plugin *(*)(void)>(dlsym(lib.get(), "get_plugin"));
    char* error = dlerror(); // capture it before, since it clears afterwards
    if (error || !get_plugin_func) {
      std::cerr << "  ERROR: Symbol missing: " << (error ? error : "NULL") << "\n";
      return std::nullopt;
    }

    Plugin* api = get_plugin_func();
    if (!validate_plugin(api)) {
      // It already threw errors
      return std::nullopt;
    }

    // Initialize the specific state for this instance
    void *ctx = api->init();

    return PluginInstance(std::move(lib), api, ctx);
  }

  ArrowChainProxy operator->() const {
    return ArrowChainProxy(m_plugin, m_ctx);
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
