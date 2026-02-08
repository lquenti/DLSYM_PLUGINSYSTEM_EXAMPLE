#ifndef PLUGIN_API_HEADER
#define PLUGIN_API_HEADER

#include <assert.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

struct Plugin {
  // Magic functions
  void *(*init)();
  void (*bind)(void *ctx); // Required to set thread local ctx storage...
  void (*destroy)(void *ctx);

  void (*foo)(int x);
};

void *get_ctx();

Plugin *get_plugin();

// Will be called by main, but feel free to also use for testing
static inline int validate_plugin(const struct Plugin *p) {
  if (!p) {
    fprintf(stderr, "[validate_plugin] Plugin API table is NULL\n");
    return 0;
  }
  int valid = 1;
  if (!p->init) { fprintf(stderr, "[validate_plugin] Missing symbol: 'init'\n"); valid = 0; }
  if (!p->bind) { fprintf(stderr, "[validate_plugin] Missing symbol: 'bind'\n"); valid = 0; }
  if (!p->destroy) { fprintf(stderr, "[validate_plugin] Missing symbol: 'destroy'\n"); valid = 0; }
  if (!p->foo) { fprintf(stderr, "[validate_plugin] Missing symbol: 'foo'\n"); valid = 0; }
  return valid;
}

#ifdef __cplusplus
}
#endif

#endif
