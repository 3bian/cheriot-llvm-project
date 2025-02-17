; RUN: llc < %s -mtriple=armv7-none-linux-gnueabi | FileCheck %s
; Test that we correctly use registers and align elements when using va_arg

%struct_t = type { double, double, double }
@static_val = constant %struct_t { double 1.0, double 2.0, double 3.0 }

declare void @llvm.va_start.p0(ptr) nounwind
declare void @llvm.va_end.p0(ptr) nounwind

; CHECK-LABEL: test_byval_8_bytes_alignment:
define void @test_byval_8_bytes_alignment(i32 %i, ...) {
entry:
; CHECK: sub       sp, sp, #16
; CHECK: add       r0, sp, #4
; CHECK: stmib     sp, {r1, r2, r3}
  %g = alloca ptr
  call void @llvm.va_start.p0(ptr %g)

; CHECK: add	[[REG:(r[0-9]+)|(lr)]], {{(r[0-9]+)|(lr)}}, #7
; CHECK: bic	[[REG]], [[REG]], #7
  %0 = va_arg ptr %g, double
  call void @llvm.va_end.p0(ptr %g)

  ret void
}

; CHECK-LABEL: main:
; CHECK: movw [[BASE:r[0-9]+]], :lower16:static_val
; CHECK: movt [[BASE]], :upper16:static_val
; ldm is not formed when the coalescer failed to coalesce everything.
; CHECK: ldrd    r2, [[TMP:r[0-9]+]], [[[BASE]]]
; CHECK: movw r0, #555
define i32 @main() {
entry:
  call void (i32, ...) @test_byval_8_bytes_alignment(i32 555, ptr byval(%struct_t) @static_val)
  ret i32 0
}

declare void @f(double);

; CHECK-LABEL:     test_byval_8_bytes_alignment_fixed_arg:
; CHECK-NOT:   str     r1
; CHECK-DAG:   str     r3, [sp, #12]
; CHECK-DAG:   str     r2, [sp, #8]
; CHECK-NOT:   str     r1
define void @test_byval_8_bytes_alignment_fixed_arg(i32 %n1, ptr byval(%struct_t) %val) nounwind {
entry:
  %0 = load double, ptr %val
  call void (double) @f(double %0)
  ret void
}

; CHECK-LABEL: main_fixed_arg:
; CHECK: movw [[BASE:r[0-9]+]], :lower16:static_val
; CHECK: movt [[BASE]], :upper16:static_val
; ldm is not formed when the coalescer failed to coalesce everything.
; CHECK: ldrd     r2, [[TMP:r[0-9]+]], [[[BASE]]]
; CHECK: movw r0, #555
define i32 @main_fixed_arg() {
entry:
  call void (i32, ptr) @test_byval_8_bytes_alignment_fixed_arg(i32 555, ptr byval(%struct_t) @static_val)
  ret i32 0
}
