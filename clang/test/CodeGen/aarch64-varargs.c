// RUN: %clang_cc1 -triple arm64-linux-gnu -emit-llvm -o - %s | FileCheck --check-prefix=CHECK --check-prefix=CHECK-LE %s
// RUN: %clang_cc1 -triple aarch64_be-linux-gnu -emit-llvm -o - %s | FileCheck --check-prefix=CHECK --check-prefix=CHECK-BE %s

#include <stdarg.h>

// Obviously there's more than one way to implement va_arg. This test should at
// least prevent unintentional regressions caused by refactoring.

va_list the_list;

int simple_int(void) {
// CHECK-LABEL: define{{.*}} i32 @simple_int
  return va_arg(the_list, int);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 8
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK-BE: [[REG_ADDR_ALIGNED:%[0-9]+]] = getelementptr inbounds i8, ptr [[REG_ADDR]], i64 4
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 8
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK-BE: [[STACK_ALIGNED:%[a-z_0-9]*]] = getelementptr inbounds i8, ptr [[STACK]], i64 4
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK-LE: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
// CHECK-BE: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR_ALIGNED]], %[[VAARG_IN_REG]] ], [ [[STACK_ALIGNED]], %[[VAARG_ON_STACK]] ]
// CHECK: [[RESULT:%[a-z_0-9]+]] = load i32, ptr [[ADDR]]
// CHECK: ret i32 [[RESULT]]
}

__int128 aligned_int(void) {
// CHECK-LABEL: define{{.*}} i128 @aligned_int
  return va_arg(the_list, __int128);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[ALIGN_REGOFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 15
// CHECK: [[ALIGNED_REGOFFS:%[a-z_0-9]+]] = and i32 [[ALIGN_REGOFFS]], -16
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[ALIGNED_REGOFFS]], 16
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[ALIGNED_REGOFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[STACKINT:%[a-z_0-9]+]] = ptrtoint ptr [[STACK]] to i64
// CHECK: [[ALIGN_STACK:%[a-z_0-9]+]] = add i64 [[STACKINT]], 15
// CHECK: [[ALIGNED_STACK_INT:%[a-z_0-9]+]] = and i64 [[ALIGN_STACK]], -16
// CHECK: [[ALIGNED_STACK_PTR:%[a-z_0-9]+]] = inttoptr i64 [[ALIGNED_STACK_INT]] to ptr
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[ALIGNED_STACK_PTR]], i64 16
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[ALIGNED_STACK_PTR]], %[[VAARG_ON_STACK]] ]
// CHECK: [[RESULT:%[a-z_0-9]+]] = load i128, ptr [[ADDR]]
// CHECK: ret i128 [[RESULT]]
}

struct bigstruct {
  int a[10];
};

struct bigstruct simple_indirect(void) {
// CHECK-LABEL: define{{.*}} void @simple_indirect
  return va_arg(the_list, struct bigstruct);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK-NOT: and i32
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 8
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK-NOT: and i64
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 8
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
// CHECK: load ptr, ptr [[ADDR]]
}

struct aligned_bigstruct {
  float a;
  long double b;
};

struct aligned_bigstruct simple_aligned_indirect(void) {
// CHECK-LABEL: define{{.*}} void @simple_aligned_indirect
  return va_arg(the_list, struct aligned_bigstruct);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 8
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 8
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
// CHECK: load ptr, ptr [[ADDR]]
}

double simple_double(void) {
// CHECK-LABEL: define{{.*}} double @simple_double
  return va_arg(the_list, double);
// CHECK: [[VR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 4)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[VR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[VR_OFFS]], 16
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 4)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 2)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[VR_OFFS]]
// CHECK-BE: [[REG_ADDR_ALIGNED:%[a-z_0-9]*]] = getelementptr inbounds i8, ptr [[REG_ADDR]], i64 8
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 8
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK-LE: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
// CHECK-BE: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR_ALIGNED]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
// CHECK: [[RESULT:%[a-z_0-9]+]] = load double, ptr [[ADDR]]
// CHECK: ret double [[RESULT]]
}

struct hfa {
  float a, b;
};

struct hfa simple_hfa(void) {
// CHECK-LABEL: define{{.*}} %struct.hfa @simple_hfa
  return va_arg(the_list, struct hfa);
// CHECK: [[VR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 4)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[VR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[VR_OFFS]], 32
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 4)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 2)
// CHECK: [[FIRST_REG:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[VR_OFFS]]
// CHECK-LE: [[EL_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[FIRST_REG]], i64 0
// CHECK-BE: [[EL_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[FIRST_REG]], i64 12
// CHECK: [[EL_TMPADDR:%[a-z_0-9]+]] = getelementptr inbounds [2 x float], ptr %[[TMP_HFA:[a-z_.0-9]+]], i64 0, i64 0
// CHECK: [[EL:%[a-z_0-9]+]] = load float, ptr [[EL_ADDR]]
// CHECK: store float [[EL]], ptr [[EL_TMPADDR]]
// CHECK-LE: [[EL_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[FIRST_REG]], i64 16
// CHECK-BE: [[EL_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[FIRST_REG]], i64 28
// CHECK: [[EL_TMPADDR:%[a-z_0-9]+]] = getelementptr inbounds [2 x float], ptr %[[TMP_HFA]], i64 0, i64 1
// CHECK: [[EL:%[a-z_0-9]+]] = load float, ptr [[EL_ADDR]]
// CHECK: store float [[EL]], ptr [[EL_TMPADDR]]
// CHECK: br label %[[VAARG_END:[a-z_.0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 8
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ %[[TMP_HFA]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
}

// Over and under alignment on fundamental types has no effect on parameter
// passing, so the code generated for va_arg should be the same as for
// non-aligned fundamental types.

typedef int underaligned_int __attribute__((packed,aligned(2)));
underaligned_int underaligned_int_test(void) {
// CHECK-LABEL: define{{.*}} i32 @underaligned_int_test()
  return va_arg(the_list, underaligned_int);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 8
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK-BE: [[REG_ADDR_ALIGNED:%[0-9]+]] = getelementptr inbounds i8, ptr [[REG_ADDR]], i64 4
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 8
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK-BE: [[STACK_ALIGNED:%[a-z_0-9]*]] = getelementptr inbounds i8, ptr [[STACK]], i64 4
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK-LE: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
// CHECK-BE: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR_ALIGNED]], %[[VAARG_IN_REG]] ], [ [[STACK_ALIGNED]], %[[VAARG_ON_STACK]] ]
// CHECK: [[RESULT:%[a-z_0-9]+]] = load i32, ptr [[ADDR]]
// CHECK: ret i32 [[RESULT]]
}

typedef int overaligned_int __attribute__((aligned(32)));
overaligned_int overaligned_int_test(void) {
// CHECK-LABEL: define{{.*}} i32 @overaligned_int_test()
  return va_arg(the_list, overaligned_int);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 8
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK-BE: [[REG_ADDR_ALIGNED:%[0-9]+]] = getelementptr inbounds i8, ptr [[REG_ADDR]], i64 4
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 8
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK-BE: [[STACK_ALIGNED:%[a-z_0-9]*]] = getelementptr inbounds i8, ptr [[STACK]], i64 4
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK-LE: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
// CHECK-BE: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR_ALIGNED]], %[[VAARG_IN_REG]] ], [ [[STACK_ALIGNED]], %[[VAARG_ON_STACK]] ]
// CHECK: [[RESULT:%[a-z_0-9]+]] = load i32, ptr [[ADDR]]
// CHECK: ret i32 [[RESULT]]
}

typedef long long underaligned_long_long  __attribute__((packed,aligned(2)));
underaligned_long_long underaligned_long_long_test(void) {
// CHECK-LABEL: define{{.*}} i64 @underaligned_long_long_test()
  return va_arg(the_list, underaligned_long_long);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 8
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 8
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
// CHECK: [[RESULT:%[a-z_0-9]+]] = load i64, ptr [[ADDR]]
// CHECK: ret i64 [[RESULT]]
}

typedef long long overaligned_long_long  __attribute__((aligned(32)));
overaligned_long_long overaligned_long_long_test(void) {
// CHECK-LABEL: define{{.*}} i64 @overaligned_long_long_test()
  return va_arg(the_list, overaligned_long_long);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 8
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 8
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
// CHECK: [[RESULT:%[a-z_0-9]+]] = load i64, ptr [[ADDR]]
// CHECK: ret i64 [[RESULT]]
}

typedef __int128 underaligned_int128  __attribute__((packed,aligned(2)));
underaligned_int128 underaligned_int128_test(void) {
// CHECK-LABEL: define{{.*}} i128 @underaligned_int128_test()
  return va_arg(the_list, underaligned_int128);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[ALIGN_REGOFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 15
// CHECK: [[ALIGNED_REGOFFS:%[a-z_0-9]+]] = and i32 [[ALIGN_REGOFFS]], -16
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[ALIGNED_REGOFFS]], 16
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[ALIGNED_REGOFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[STACKINT:%[a-z_0-9]+]] = ptrtoint ptr [[STACK]] to i64
// CHECK: [[ALIGN_STACK:%[a-z_0-9]+]] = add i64 [[STACKINT]], 15
// CHECK: [[ALIGNED_STACK_INT:%[a-z_0-9]+]] = and i64 [[ALIGN_STACK]], -16
// CHECK: [[ALIGNED_STACK_PTR:%[a-z_0-9]+]] = inttoptr i64 [[ALIGNED_STACK_INT]] to ptr
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[ALIGNED_STACK_PTR]], i64 16
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[ALIGNED_STACK_PTR]], %[[VAARG_ON_STACK]] ]
// CHECK: [[RESULT:%[a-z_0-9]+]] = load i128, ptr [[ADDR]]
// CHECK: ret i128 [[RESULT]]
}

typedef __int128 overaligned_int128  __attribute__((aligned(32)));
overaligned_int128 overaligned_int128_test(void) {
// CHECK-LABEL: define{{.*}} i128 @overaligned_int128_test()
  return va_arg(the_list, overaligned_int128);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[ALIGN_REGOFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 15
// CHECK: [[ALIGNED_REGOFFS:%[a-z_0-9]+]] = and i32 [[ALIGN_REGOFFS]], -16
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[ALIGNED_REGOFFS]], 16
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[ALIGNED_REGOFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[STACKINT:%[a-z_0-9]+]] = ptrtoint ptr [[STACK]] to i64
// CHECK: [[ALIGN_STACK:%[a-z_0-9]+]] = add i64 [[STACKINT]], 15
// CHECK: [[ALIGNED_STACK_INT:%[a-z_0-9]+]] = and i64 [[ALIGN_STACK]], -16
// CHECK: [[ALIGNED_STACK_PTR:%[a-z_0-9]+]] = inttoptr i64 [[ALIGNED_STACK_INT]] to ptr
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[ALIGNED_STACK_PTR]], i64 16
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[ALIGNED_STACK_PTR]], %[[VAARG_ON_STACK]] ]
// CHECK: [[RESULT:%[a-z_0-9]+]] = load i128, ptr [[ADDR]]
// CHECK: ret i128 [[RESULT]]
}

// The way that attributes applied to a struct change parameter passing is a
// little strange, in that the alignment due to attributes is used when
// calculating the size of the struct, but the alignment is based only on the
// alignment of the members (which can be affected by attributes). What this
// means is:
//  * The only effect of the aligned attribute on a struct is to increase its
//    size if the alignment is greater than the member alignment.
//  * The packed attribute is considered as applying to the members, so it will
//    affect the alignment.
// Additionally the alignment can't go below 8 or above 16, so it's only
// __int128 that can be affected by a change in alignment.

typedef struct __attribute__((packed,aligned(2))) {
  int val;
} underaligned_int_struct;
underaligned_int_struct underaligned_int_struct_test(void) {
// CHECK-LE-LABEL: define{{.*}} i32 @underaligned_int_struct_test()
// CHECK-BE-LABEL: define{{.*}} i64 @underaligned_int_struct_test()
  return va_arg(the_list, underaligned_int_struct);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 8
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 8
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
}

typedef struct __attribute__((aligned(16))) {
  int val;
} overaligned_int_struct;
overaligned_int_struct overaligned_int_struct_test(void) {
// CHECK-LABEL: define{{.*}} i128 @overaligned_int_struct_test()
  return va_arg(the_list, overaligned_int_struct);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 16
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 16
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
}

typedef struct __attribute__((packed,aligned(2))) {
  long long val;
} underaligned_long_long_struct;
underaligned_long_long_struct underaligned_long_long_struct_test(void) {
// CHECK-LABEL: define{{.*}} i64 @underaligned_long_long_struct_test()
  return va_arg(the_list, underaligned_long_long_struct);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 8
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 8
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
}

typedef struct __attribute__((aligned(16))) {
  long long val;
} overaligned_long_long_struct;
overaligned_long_long_struct overaligned_long_long_struct_test(void) {
// CHECK-LABEL: define{{.*}} i128 @overaligned_long_long_struct_test()
  return va_arg(the_list, overaligned_long_long_struct);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 16
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 16
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
}

typedef struct __attribute__((packed,aligned(2))) {
  __int128 val;
} underaligned_int128_struct;
underaligned_int128_struct underaligned_int128_struct_test(void) {
// CHECK-LABEL: define{{.*}} [2 x i64] @underaligned_int128_struct_test()
  return va_arg(the_list, underaligned_int128_struct);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 16
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 16
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
}

// Overaligning to 32 bytes causes it to be passed indirectly via a pointer
typedef struct __attribute__((aligned(32))) {
  __int128 val;
} overaligned_int128_struct;
overaligned_int128_struct overaligned_int128_struct_test(void) {
// CHECK-LABEL: define{{.*}} void @overaligned_int128_struct_test(ptr noalias sret(%struct.overaligned_int128_struct) align 32 %agg.result)
  return va_arg(the_list, overaligned_int128_struct);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 8
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 8
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
}

// Overaligning or underaligning a struct member changes both its alignment and
// size when passed as an argument.

typedef struct {
  int val __attribute__((packed,aligned(2)));
} underaligned_int_struct_member;
underaligned_int_struct_member underaligned_int_struct_member_test(void) {
// CHECK-LE-LABEL: define{{.*}} i32 @underaligned_int_struct_member_test()
// CHECK-BE-LABEL: define{{.*}} i64 @underaligned_int_struct_member_test()
  return va_arg(the_list, underaligned_int_struct_member);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 8
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 8
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
}

typedef struct {
  int val __attribute__((aligned(16)));
} overaligned_int_struct_member;
overaligned_int_struct_member overaligned_int_struct_member_test(void) {
// CHECK-LABEL: define{{.*}} i128 @overaligned_int_struct_member_test()
  return va_arg(the_list, overaligned_int_struct_member);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[ALIGN_REGOFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 15
// CHECK: [[ALIGNED_REGOFFS:%[a-z_0-9]+]] = and i32 [[ALIGN_REGOFFS]], -16
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[ALIGNED_REGOFFS]], 16
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[ALIGNED_REGOFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[STACKINT:%[a-z_0-9]+]] = ptrtoint ptr [[STACK]] to i64
// CHECK: [[ALIGN_STACK:%[a-z_0-9]+]] = add i64 [[STACKINT]], 15
// CHECK: [[ALIGNED_STACK_INT:%[a-z_0-9]+]] = and i64 [[ALIGN_STACK]], -16
// CHECK: [[ALIGNED_STACK_PTR:%[a-z_0-9]+]] = inttoptr i64 [[ALIGNED_STACK_INT]] to ptr
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[ALIGNED_STACK_PTR]], i64 16
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[ALIGNED_STACK_PTR]], %[[VAARG_ON_STACK]] ]
}

typedef struct {
  long long val __attribute__((packed,aligned(2)));
} underaligned_long_long_struct_member;
underaligned_long_long_struct_member underaligned_long_long_struct_member_test(void) {
// CHECK-LABEL: define{{.*}} i64 @underaligned_long_long_struct_member_test()
  return va_arg(the_list, underaligned_long_long_struct_member);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 8
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 8
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
}

typedef struct {
  long long val __attribute__((aligned(16)));
} overaligned_long_long_struct_member;
overaligned_long_long_struct_member overaligned_long_long_struct_member_test(void) {
// CHECK-LABEL: define{{.*}} i128 @overaligned_long_long_struct_member_test()
  return va_arg(the_list, overaligned_long_long_struct_member);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[ALIGN_REGOFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 15
// CHECK: [[ALIGNED_REGOFFS:%[a-z_0-9]+]] = and i32 [[ALIGN_REGOFFS]], -16
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[ALIGNED_REGOFFS]], 16
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[ALIGNED_REGOFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[STACKINT:%[a-z_0-9]+]] = ptrtoint ptr [[STACK]] to i64
// CHECK: [[ALIGN_STACK:%[a-z_0-9]+]] = add i64 [[STACKINT]], 15
// CHECK: [[ALIGNED_STACK_INT:%[a-z_0-9]+]] = and i64 [[ALIGN_STACK]], -16
// CHECK: [[ALIGNED_STACK_PTR:%[a-z_0-9]+]] = inttoptr i64 [[ALIGNED_STACK_INT]] to ptr
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[ALIGNED_STACK_PTR]], i64 16
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[ALIGNED_STACK_PTR]], %[[VAARG_ON_STACK]] ]
}

typedef struct {
  __int128 val __attribute__((packed,aligned(2)));
} underaligned_int128_struct_member;
underaligned_int128_struct_member underaligned_int128_struct_member_test(void) {
// CHECK-LABEL: define{{.*}} [2 x i64] @underaligned_int128_struct_member_test()
  return va_arg(the_list, underaligned_int128_struct_member);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 16
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 16
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
}

// Overaligning to 32 bytes causes it to be passed indirectly via a pointer
typedef struct {
  __int128 val __attribute__((aligned(32)));
} overaligned_int128_struct_member;
overaligned_int128_struct_member overaligned_int128_struct_member_test(void) {
// CHECK-LABEL: define{{.*}} void @overaligned_int128_struct_member_test(ptr noalias sret(%struct.overaligned_int128_struct_member) align 32 %agg.result)
  return va_arg(the_list, overaligned_int128_struct_member);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load i32, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[EARLY_ONSTACK:%[a-z_0-9]+]] = icmp sge i32 [[GR_OFFS]], 0
// CHECK: br i1 [[EARLY_ONSTACK]], label %[[VAARG_ON_STACK:[a-z_.0-9]+]], label %[[VAARG_MAYBE_REG:[a-z_.0-9]+]]

// CHECK: [[VAARG_MAYBE_REG]]
// CHECK: [[NEW_REG_OFFS:%[a-z_0-9]+]] = add i32 [[GR_OFFS]], 8
// CHECK: store i32 [[NEW_REG_OFFS]], ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 3)
// CHECK: [[INREG:%[a-z_0-9]+]] = icmp sle i32 [[NEW_REG_OFFS]], 0
// CHECK: br i1 [[INREG]], label %[[VAARG_IN_REG:[a-z_.0-9]+]], label %[[VAARG_ON_STACK]]

// CHECK: [[VAARG_IN_REG]]
// CHECK: [[REG_TOP:%[a-z_0-9]+]] = load ptr, ptr getelementptr inbounds (%struct.__va_list, ptr @the_list, i32 0, i32 1)
// CHECK: [[REG_ADDR:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[REG_TOP]], i32 [[GR_OFFS]]
// CHECK: br label %[[VAARG_END:[a-z._0-9]+]]

// CHECK: [[VAARG_ON_STACK]]
// CHECK: [[STACK:%[a-z_0-9]+]] = load ptr, ptr @the_list
// CHECK: [[NEW_STACK:%[a-z_0-9]+]] = getelementptr inbounds i8, ptr [[STACK]], i64 8
// CHECK: store ptr [[NEW_STACK]], ptr @the_list
// CHECK: br label %[[VAARG_END]]

// CHECK: [[VAARG_END]]
// CHECK: [[ADDR:%[a-z._0-9]+]] = phi ptr [ [[REG_ADDR]], %[[VAARG_IN_REG]] ], [ [[STACK]], %[[VAARG_ON_STACK]] ]
}

void check_start(int n, ...) {
// CHECK-LABEL: define{{.*}} void @check_start(i32 noundef %n, ...)

  va_list the_list;
  va_start(the_list, n);
// CHECK: [[THE_LIST:%[a-z_0-9]+]] = alloca %struct.__va_list
// CHECK: call void @llvm.va_start.p0(ptr [[THE_LIST]])
}

typedef struct {} empty;
empty empty_record_test(void) {
// CHECK-LABEL: define{{.*}} void @empty_record_test()
  return va_arg(the_list, empty);
// CHECK: [[GR_OFFS:%[a-z_0-9]+]] = load ptr, ptr @the_list
}
