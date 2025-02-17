; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: %cheri_llc %s -o - | FileCheck %s
; ModuleID = 'brtest.c'
target datalayout = "E-p:64:64:64-i1:8:8-i8:8:32-i16:16:32-i32:32:32-i64:64:64-f32:32:32-f64:64:64-f128:128:128-v64:64:64-n32"
target triple = "cheri-unknown-freebsd"

; Check whether we're optimising a branch on a condition flag to a BC2F.

define void @isValid(ptr addrspace(200) %x) nounwind {
; CHECK-LABEL: isValid:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    daddiu $sp, $sp, -16
; CHECK-NEXT:    sd $ra, 8($sp) # 8-byte Folded Spill
; CHECK-NEXT:    cbtu $c3, .LBB0_2
; CHECK-NEXT:    nop
; CHECK-NEXT:  # %bb.1: # %if.then
; CHECK-NEXT:    jal f1
; CHECK-NEXT:    nop
; CHECK-NEXT:  .LBB0_2: # %if.end
; CHECK-NEXT:    jal f2
; CHECK-NEXT:    nop
; CHECK-NEXT:    ld $ra, 8($sp) # 8-byte Folded Reload
; CHECK-NEXT:    jr $ra
; CHECK-NEXT:    daddiu $sp, $sp, 16
entry:
  %0 = tail call i1 @llvm.cheri.cap.tag.get(ptr addrspace(200) %x)
  br i1 %0, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  tail call void @f1() nounwind
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  tail call void @f2() nounwind
  ret void
}

declare i1 @llvm.cheri.cap.tag.get(ptr addrspace(200))

declare void @f1()

declare void @f2()
