//  { dg-do run }
//  { dg-skip-if "requires hosted libstdc++ for cstdlib abort" { ! hostedlib } }

// Test yielding an int.

// We will use the promise to contain this to avoid having to include
// additional C++ headers.

// Check that we resolve the correct overload for the yield_value method.

#include "../coro.h"

struct coro1 {
  struct promise_type;
  using handle_type = coro::coroutine_handle<coro1::promise_type>;
  handle_type handle;
  coro1 () : handle(0) {}
  coro1 (handle_type _handle)
    : handle(_handle) {
        PRINT("Created coro1 object from handle");
  }
  coro1 (const coro1 &) = delete; // no copying
  coro1 (coro1 &&s) : handle(s.handle) {
    s.handle = nullptr;
    PRINT("coro1 mv ctor ");
  }
  coro1 &operator = (coro1 &&s) {
    handle = s.handle;
    s.handle = nullptr;
    PRINT("coro1 op=  ");
    return *this;
  }
  ~coro1() {
    PRINT("Destroyed coro1");
    if ( handle )
      handle.destroy();
  }

  struct suspend_never_prt {
    bool await_ready() const noexcept { return true; }
    void await_suspend(handle_type) const noexcept { PRINT ("susp-never-susp"); }
    void await_resume() const noexcept { PRINT ("susp-never-resume");}
  };

  /* NOTE: this has a DTOR to test that pathway.  */
  struct  suspend_always_prt {
    bool await_ready() const noexcept { return false; }
    void await_suspend(handle_type) const noexcept { PRINT ("susp-always-susp"); }
    void await_resume() const noexcept { PRINT ("susp-always-resume"); }
    ~suspend_always_prt() { PRINT ("susp-always-DTOR"); }
  };

  struct promise_type {
  int value;
  promise_type() {  PRINT ("Created Promise"); }
  ~promise_type() { PRINT ("Destroyed Promise"); }

  auto get_return_object () {
    PRINT ("get_return_object: handle from promise");
    return handle_type::from_promise (*this);
  }
  auto initial_suspend () {
    PRINT ("get initial_suspend (always)");
    return suspend_always_prt{};
  }
  auto final_suspend () noexcept {
    PRINT ("get final_suspend (always)");
    return suspend_always_prt{};
  }
  void return_value (int v) {
    PRINTF ("return_value () %d\n",v);
    value = v;
  }
  auto yield_value (int v) {
    PRINTF ("yield_value () %d and suspend always\n",v);
    value = v;
    return suspend_always_prt{};
  }
  /* Some non-matching overloads.  */
  auto yield_value (suspend_always_prt s, int x) {
    return s;
  }
  auto yield_value (void) {
    return 42;
  }
  int get_value (void) { return value; }
  void unhandled_exception() { PRINT ("** unhandled exception"); }
  };
};

struct coro1
f () noexcept
{
  PRINT ("f: about to yield 42");
  co_yield 42;

  PRINT ("f: about to return 6174");
  co_return 6174;
}

int main ()
{
  PRINT ("main: create coro1");
  struct coro1 x = f ();
  PRINT ("main: got coro1 - resuming (1)");
  if (x.handle.done())
    abort();
  x.handle.resume();
  PRINT ("main: after resume (1)");
  int y = x.handle.promise().get_value();
  if ( y != 42 )
    abort ();
  PRINT ("main: apparently got 42");
  PRINT ("main: got coro1 - resuming (2)");
  if (x.handle.done())
    abort();
  x.handle.resume();
  PRINT ("main: after resume (2)");
  y = x.handle.promise().get_value();
  if ( y != 6174 )
    abort ();
  PRINT ("main: apparently got 6174");
  if (!x.handle.done())
    {
      PRINT ("main: apparently not done...");
      abort ();
    }
  PRINT ("main: returning");
  return 0;
}
