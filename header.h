#ifndef PLUGIN_API_HEADER
#define PLUGIN_API_HEADER

#ifdef __cplusplus
extern "C" {
#endif
struct Plugin {
  void (*foo)(int x);
};

Plugin *get_plugin();
#ifdef __cplusplus
}
#endif

#endif
