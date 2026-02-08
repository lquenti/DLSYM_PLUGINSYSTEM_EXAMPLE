#include "header.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct CPlugin {
  int id;
  char name[65];
} CPlugin;

static _Thread_local CPlugin *g_instance = NULL;

void *plugin_init() {
  static int counter = 0;
  CPlugin *p = malloc(sizeof(CPlugin));
  p->id = ++counter;
  snprintf(p->name, 65, "Instance_%d", p->id);
  printf("[CPlugin   %s] Constructed\n", p->name);
  return p;
}

void plugin_destroy(void *ctx) {
  CPlugin *p = (CPlugin *)ctx;
  printf("[CPlugin   %s] Destructed\n", p->name);
  free(p);
}

void plugin_bind(void *ctx) {
  g_instance = (CPlugin *)ctx;
}

void plugin_foo(int x) {
    printf("[CPlugin   %s] Squared: %d\n", g_instance->name, x * x);
}

static struct Plugin plugin_api = {
  .init = plugin_init,
  .bind = plugin_bind,
  .destroy = plugin_destroy,

  // Add any other methods
  .foo = plugin_foo
};

struct Plugin *get_plugin() {
  return &plugin_api;
}
