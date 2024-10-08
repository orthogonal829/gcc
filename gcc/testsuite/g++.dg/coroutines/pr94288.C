//  { dg-additional-options  "-w" }
//  { dg-skip-if "requires hosted libstdc++ for vector" { ! hostedlib } }

#include "coro.h"

#include <vector>

template <typename T> struct promise {
  T _value;
  coro::coroutine_handle<> _continuation = nullptr;

  struct final_awaitable {
    bool _has_continuation;
    final_awaitable(bool has_continuation)
        : _has_continuation(has_continuation) {}

    bool await_ready() const noexcept { return !_has_continuation; }

    template <typename Promise>
    coro::coroutine_handle<>
    await_suspend(coro::coroutine_handle<Promise> coro) noexcept {
      return coro.promise()._continuation;
    }

    void await_resume() noexcept {}
  };

  auto get_return_object() noexcept {
    return coro::coroutine_handle<promise>::from_promise(*this);
  }

  auto initial_suspend() noexcept { return coro::suspend_always(); }

  auto final_suspend() noexcept {
    return final_awaitable(_continuation != nullptr);
  }

  void return_value(T value) { _value = value; }

  void unhandled_exception() { /*std::terminate();*/ }

};

template <typename T> struct task {
  using promise_type = promise<T>;
  coro::coroutine_handle<promise<T>> _handle;

  task(coro::coroutine_handle<promise<T>> handle) : _handle(handle) {}

  bool await_ready() noexcept { return _handle.done(); }

  coro::coroutine_handle<>
  await_suspend(coro::coroutine_handle<> handle) noexcept {
    _handle.promise()._continuation = handle;
    return _handle;
  }

  T await_resume() noexcept { return _handle.promise()._value; }
};

task<std::vector<int>> foo()
{
  co_return std::vector<int>();
}

task<int> bar()
{
  while ((co_await foo()).empty()) {
  }
  co_return 0;
}
