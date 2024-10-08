// { dg-do run { target c++11 } }
// { dg-skip-if "requires hosted libstdc++ for cassert" { ! hostedlib } }

#include <cassert>

template<typename F>
void call(const F& f) { f(); }

int main() {
  call([] () -> void {});
  //call([] () mutable -> void {}); // { dg-error: "`f' does not have const `operator()'" }

  int i = -1;
  call([&i] () -> void { i = 0; });
  assert(i == 0);
  //call([i] () -> void { i = 0; }); // { dg-error: "assignment to non-reference capture in const lambda" }

  return 0;
}

