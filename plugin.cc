#include "header.h"
#include <iostream>
#include <string>

class PluginImplementation {
  int id;
  std::string name;
  public:
  PluginImplementation() {
    static int counter = 0;
    id = ++counter;
    name = "Instance_" + std::to_string(id);
    std::cout << "[Plugin " << name << "] Constructed\n";
  }

  ~PluginImplementation() {
    std::cout << "[Plugin " << name << "] Destructed\n";
  }

  void foo(int x) {
    std::cout << "[Plugin " << name << "] Squared: " << x * x << std::endl;
  }
};

static thread_local PluginImplementation* g_instance = nullptr;

static Plugin plugin_api = {
  .init = []() -> void* { return new PluginImplementation();},
  .bind = [](void* ctx) {g_instance = static_cast<PluginImplementation*>(ctx);},
  .destroy = [](void* ctx) {delete static_cast<PluginImplementation*>(ctx);},

  .foo = [](auto... args) { g_instance->foo(args...); },
};

extern "C" {
  Plugin *get_plugin() {
    return &plugin_api;
  }
}
