; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: %cheri_purecap_llc -O0 %s -o - | FileCheck %s

define void @zero(ptr addrspace(200) nocapture %out) local_unnamed_addr nounwind {
; CHECK-LABEL: zero:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    csc $cnull, $zero, 0($c3)
; CHECK-NEXT:    daddiu $1, $zero, 0
; CHECK-NEXT:    csb $zero, $zero, 30($c3)
; CHECK-NEXT:    csh $zero, $zero, 28($c3)
; CHECK-NEXT:    addiu $1, $zero, 0
; CHECK-NEXT:    csw $zero, $zero, 24($c3)
; CHECK-NEXT:    csd $zero, $zero, 16($c3)
; CHECK-NEXT:    cjr $c17
; CHECK-NEXT:    nop
entry:
; Check that the zero memset is expanded to capability stores and a final overlapping store.
; Note: no unaligned store anymore since CHERI doesn't support it -> three stores instead
  call void @llvm.memset.p200.i64(ptr addrspace(200) align 32 %out, i8 0, i64 31, i1 false)
  ret void
}

declare void @llvm.memset.p200.i64(ptr addrspace(200) nocapture writeonly, i8, i64, i1 immarg) #0
