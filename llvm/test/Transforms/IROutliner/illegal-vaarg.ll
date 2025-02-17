; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt -S -passes=verify,iroutliner -ir-outlining-no-cost -no-ir-sim-intrinsics < %s | FileCheck %s

; This test ensures that we do not outline vararg instructions or intrinsics, as
; they may cause inconsistencies when outlining.

declare void @llvm.va_start(ptr)
declare void @llvm.va_copy(ptr, ptr)
declare void @llvm.va_end(ptr)

define i32 @func1(i32 %a, double %b, ptr %v, ...) nounwind {
; CHECK-LABEL: @func1(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP_LOC:%.*]] = alloca i32, align 4
; CHECK-NEXT:    [[A_ADDR:%.*]] = alloca i32, align 4
; CHECK-NEXT:    [[B_ADDR:%.*]] = alloca double, align 8
; CHECK-NEXT:    [[AP:%.*]] = alloca ptr, align 4
; CHECK-NEXT:    [[C:%.*]] = alloca i32, align 4
; CHECK-NEXT:    call void @outlined_ir_func_0(i32 [[A:%.*]], ptr [[A_ADDR]], double [[B:%.*]], ptr [[B_ADDR]])
; CHECK-NEXT:    call void @llvm.va_start.p0(ptr [[AP]])
; CHECK-NEXT:    [[TMP0:%.*]] = va_arg ptr [[AP]], i32
; CHECK-NEXT:    call void @llvm.va_copy.p0.p0(ptr [[V:%.*]], ptr [[AP]])
; CHECK-NEXT:    call void @llvm.va_end.p0(ptr [[AP]])
; CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 -1, ptr [[TMP_LOC]])
; CHECK-NEXT:    call void @outlined_ir_func_1(i32 [[TMP0]], ptr [[C]], ptr [[TMP_LOC]])
; CHECK-NEXT:    [[TMP_RELOAD:%.*]] = load i32, ptr [[TMP_LOC]], align 4
; CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 -1, ptr [[TMP_LOC]])
; CHECK-NEXT:    ret i32 [[TMP_RELOAD]]
;
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca double, align 8
  %ap = alloca ptr, align 4
  %c = alloca i32, align 4
  store i32 %a, ptr %a.addr, align 4
  store double %b, ptr %b.addr, align 8
  call void @llvm.va_start(ptr %ap)
  %0 = va_arg ptr %ap, i32
  call void @llvm.va_copy(ptr %v, ptr %ap)
  call void @llvm.va_end(ptr %ap)
  store i32 %0, ptr %c, align 4
  %tmp = load i32, ptr %c, align 4
  ret i32 %tmp
}

define i32 @func2(i32 %a, double %b, ptr %v, ...) nounwind {
; CHECK-LABEL: @func2(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP_LOC:%.*]] = alloca i32, align 4
; CHECK-NEXT:    [[A_ADDR:%.*]] = alloca i32, align 4
; CHECK-NEXT:    [[B_ADDR:%.*]] = alloca double, align 8
; CHECK-NEXT:    [[AP:%.*]] = alloca ptr, align 4
; CHECK-NEXT:    [[C:%.*]] = alloca i32, align 4
; CHECK-NEXT:    call void @outlined_ir_func_0(i32 [[A:%.*]], ptr [[A_ADDR]], double [[B:%.*]], ptr [[B_ADDR]])
; CHECK-NEXT:    call void @llvm.va_start.p0(ptr [[AP]])
; CHECK-NEXT:    [[TMP0:%.*]] = va_arg ptr [[AP]], i32
; CHECK-NEXT:    call void @llvm.va_copy.p0.p0(ptr [[V:%.*]], ptr [[AP]])
; CHECK-NEXT:    call void @llvm.va_end.p0(ptr [[AP]])
; CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 -1, ptr [[TMP_LOC]])
; CHECK-NEXT:    call void @outlined_ir_func_1(i32 [[TMP0]], ptr [[C]], ptr [[TMP_LOC]])
; CHECK-NEXT:    [[TMP_RELOAD:%.*]] = load i32, ptr [[TMP_LOC]], align 4
; CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 -1, ptr [[TMP_LOC]])
; CHECK-NEXT:    ret i32 [[TMP_RELOAD]]
;
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca double, align 8
  %ap = alloca ptr, align 4
  %c = alloca i32, align 4
  store i32 %a, ptr %a.addr, align 4
  store double %b, ptr %b.addr, align 8
  call void @llvm.va_start(ptr %ap)
  %0 = va_arg ptr %ap, i32
  call void @llvm.va_copy(ptr %v, ptr %ap)
  call void @llvm.va_end(ptr %ap)
  store i32 %0, ptr %c, align 4
  %tmp = load i32, ptr %c, align 4
  ret i32 %tmp
}
