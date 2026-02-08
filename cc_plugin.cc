#include "header.h"
#include <iostream>
#include <string>

class CppPlugin {
  int id;
  std::string name;
  public:
  CppPlugin() {
    static int counter{0};
    id = ++counter;
    name = "Instance_" + std::to_string(id);
    std::cout << "[CppPlugin " << name << "] Constructed\n";
  }

  ~CppPlugin() {
    std::cout << "[CppPlugin " << name << "] Destructed\n";
  }

  void foo(int x) {
    std::cout << "[CppPlugin " << name << "] Squared: " << x * x << std::endl;
  }
};

static thread_local CppPlugin* g_instance = nullptr;

static Plugin plugin_api = {
  .init = []() -> void* { return new CppPlugin();},
  .bind = [](void* ctx) {g_instance = static_cast<CppPlugin *>(ctx);},
  .destroy = [](void* ctx) {delete static_cast<CppPlugin *>(ctx);},

  // Add any other methods
  .foo = [](auto... args) { g_instance->foo(args...); },
};

extern "C" {
  Plugin *get_plugin() {
    return &plugin_api;
  }
}
