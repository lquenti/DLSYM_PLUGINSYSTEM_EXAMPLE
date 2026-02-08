use std::ffi::{c_int, c_void};
use std::ptr;
use std::cell::RefCell;

// Note: Fully AI generated.
// But the idea is simple: Same logic as C++, and in the init we explicitly give away ownership to
// the main, and in the destroy we take ownership again, to RAII drop it

// 1. Define the Plugin V-Table layout to match header.h exactly
#[repr(C)]
pub struct Plugin {
    init: extern "C" fn() -> *mut c_void,
    bind: extern "C" fn(*mut c_void),
    destroy: extern "C" fn(*mut c_void),
    foo: extern "C" fn(c_int),
}

// 2. Define your actual State
struct PluginState {
    accumulator: i32,
}

// 3. Thread Local Storage to hold the context during the 'bind' -> 'foo' window
thread_local! {
    static ACTIVE_CONTEXT: RefCell<*mut PluginState> = RefCell::new(ptr::null_mut());
}

// --- Implementation of API Functions ---

extern "C" fn init() -> *mut c_void {
    // Create state on the heap and surrender ownership to C++ (Box::into_raw)
    let state = Box::new(PluginState { accumulator: 100 });
    println!("[Rust] State initialized.");
    Box::into_raw(state) as *mut c_void
}

extern "C" fn bind(ctx: *mut c_void) {
    // Store the pointer in TLS so 'foo' can find it later
    ACTIVE_CONTEXT.with(|c| {
        *c.borrow_mut() = ctx as *mut PluginState;
    });
}

extern "C" fn destroy(ctx: *mut c_void) {
    if ctx.is_null() { return; }
    // Take ownership back to drop it (free memory)
    unsafe {
        let _ = Box::from_raw(ctx as *mut PluginState);
    }
    println!("[Rust] State destroyed.");
}

extern "C" fn foo(val: c_int) {
    // Retrieve the state from TLS
    ACTIVE_CONTEXT.with(|c| {
        let state_ptr = *c.borrow();
        if state_ptr.is_null() {
            eprintln!("[Rust] Error: foo called without bind!");
            return;
        }

        // Safe to dereference because 'ArrowChainProxy' guarantees validity during this call
        unsafe {
            let state = &mut *state_ptr;
            state.accumulator += val;
            println!("[Rust] foo({}) called. New accumulator state: {}", val, state.accumulator);
        }
    });
}

// 4. The static instance of the v-table
static PLUGIN_API: Plugin = Plugin {
    init,
    bind,
    destroy,
    foo,
};

// 5. The entry point symbol that main.cc looks for
#[no_mangle]
pub extern "C" fn get_plugin() -> *const Plugin {
    &PLUGIN_API
}
