; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt < %s -passes=instcombine -S | FileCheck %s

target datalayout = "e-m:e-pf200:128:128:128:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-A200-P200-G200"

; Don't replace memcpy with a capability load/store if the size of the
; memcpy is less than the size of a capability.
define internal void @foo() unnamed_addr addrspace(200) {
; CHECK-LABEL: @foo(
; CHECK-NEXT:  start:
; CHECK-NEXT:    [[TMP0:%.*]] = alloca i96, align 16, addrspace(200)
; CHECK-NEXT:    [[_2:%.*]] = alloca [3 x i32], align 16, addrspace(200)
; CHECK-NEXT:    [[TMP1:%.*]] = call i96 @bar(i32 104)
; CHECK-NEXT:    store i96 [[TMP1]], ptr addrspace(200) [[TMP0]], align 16
; CHECK-NEXT:    call void @llvm.memcpy.p200.p200.i64(ptr addrspace(200) noundef nonnull align 16 dereferenceable(12) [[_2]], ptr addrspace(200) noundef nonnull align 16 dereferenceable(12) [[TMP0]], i64 12, i1 false)
; CHECK-NEXT:    call void @baz(ptr addrspace(200) nonnull [[_2]])
; CHECK-NEXT:    ret void
;
start:
  %0 = alloca i96, align 16, addrspace(200)
  %1 = alloca i96, align 16, addrspace(200)
  %_2 = alloca [3 x i32], align 16, addrspace(200)
  %_1 = alloca [3 x i32], align 16, addrspace(200)
  %2 = call i96 @bar(i32 104)
  store i96 %2, ptr addrspace(200) %1, align 16
  %3 = bitcast ptr addrspace(200) %_2 to ptr addrspace(200)
  %4 = bitcast ptr addrspace(200) %1 to ptr addrspace(200)
  call void @llvm.memcpy.p200.p200.i64(ptr addrspace(200) align 4 %3, ptr addrspace(200) align 16 %4, i64 12, i1 false)
  call void @baz(ptr addrspace(200) %3)
  ret void
}

declare i96 @bar(i32) addrspace(200)
declare void @baz(ptr addrspace(200)) addrspace(200)
declare void @llvm.memcpy.p200.p200.i64(ptr addrspace(200) noalias nocapture writeonly, ptr addrspace(200) noalias nocapture readonly, i64, i1 immarg) addrspace(200)
