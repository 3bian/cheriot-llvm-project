target datalayout = "E-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v128:128:128-n32:64"
target triple = "powerpc64-unknown-linux-gnu"
; RUN: llc -verify-machineinstrs < %s | FileCheck %s

define i32 @intvaarg(i32 %a, ...) nounwind {
entry:
  %va = alloca ptr, align 8
  call void @llvm.va_start.p0(ptr %va)
  %0 = va_arg ptr %va, i32
  %sub = sub nsw i32 %a, %0
  ret i32 %sub
}

declare void @llvm.va_start.p0(ptr) nounwind

; CHECK: @intvaarg
; Make sure that the va pointer is incremented by 8 (not 4).
; CHECK: addi{{.*}}, 1, 64 

