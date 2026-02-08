# TODOs
- [ ] Write blog post about it

Notes to self
- no global state because then you cant have 2 instances per .so
  - obvious solution for this would be passing ctx to each function => Annoying as it has to wrap each function to make it usably callable from main, and ugly API for plugin
    - Next step would be to have a `fuse_get_ctx`/`PyThread_Get` pattern of thread local ptr that gets set before => still annoying
      - Final solution: Explain ScopedBinder and automatic `operator->` chaining
- design goal: no need to change main.cc when evolving the API
