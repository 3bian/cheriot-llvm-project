// RUN: %clang_cc1 -triple armv7-apple-ios -emit-llvm -o - %s | FileCheck %s
struct Empty {};

Empty emptyvar;

int take_args(int a, ...) {
  __builtin_va_list l;
  __builtin_va_start(l, a);
// CHECK: call void @llvm.va_start.p0

  emptyvar = __builtin_va_arg(l, Empty);
// CHECK: load ptr, ptr

  // It's conceivable that EMPTY_PTR may not actually be a valid pointer
  // (e.g. it's at the very bottom of the stack and the next page is
  // invalid). This doesn't matter provided it's never loaded (there's no
  // well-defined way to tell), but it becomes a problem if we do try to use it.
// CHECK-NOT: load %struct.Empty, ptr {{%[a-zA-Z0-9._]+}}

  int i = __builtin_va_arg(l, int);
// CHECK: load i32, ptr

  __builtin_va_end(l);
  return i;
}
