; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --function-signature --scrub-attributes
; RUN: opt %s -passes=instcombine -S | FileCheck %s
; This previously asserted due to a missing cast in foldICmpUsingKnownBits()
target datalayout = "E-m:e-pf200:128:128:128:64-i8:8:32-i16:16:32-i64:64-n32:64-S128-A200-P200-G200"

@b = addrspace(200) global i64 0, align 8
@d = addrspace(200) global ptr addrspace(200) null, align 16
@c = addrspace(200) global i32 0, align 4

declare ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200), i64) addrspace(200)
define void @e() local_unnamed_addr addrspace(200) #1 {
; CHECK-LABEL: define {{[^@]+}}@e() local_unnamed_addr addrspace(200) {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP0:%.*]] = load i64, ptr addrspace(200) @b, align 8
; CHECK-NEXT:    [[TMP1:%.*]] = getelementptr i8, ptr addrspace(200) null, i64 [[TMP0]]
; CHECK-NEXT:    store ptr addrspace(200) [[TMP1]], ptr addrspace(200) @d, align 16
; CHECK-NEXT:    [[TOBOOL_NOT:%.*]] = icmp eq i64 [[TMP0]], 0
; CHECK-NEXT:    br i1 [[TOBOOL_NOT]], label [[IF_END:%.*]], label [[IF_THEN:%.*]]
; CHECK:       if.then:
; CHECK-NEXT:    br label [[IF_END]]
; CHECK:       if.end:
; CHECK-NEXT:    ret void
;
entry:
  %0 = load i64, ptr addrspace(200) @b, align 8
  %1 = getelementptr i8, ptr addrspace(200) null, i64 %0
  %2 = bitcast ptr addrspace(200) %1 to ptr addrspace(200)
  store ptr addrspace(200) %2, ptr addrspace(200) @d, align 16
  %tobool = icmp ne ptr addrspace(200) %2, null
  br i1 %tobool, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  ret void
}

define i1 @no_branches(i64 %tmp) local_unnamed_addr addrspace(200) {
; CHECK-LABEL: define {{[^@]+}}@no_branches
; CHECK-SAME: (i64 [[TMP:%.*]]) local_unnamed_addr addrspace(200) {
; CHECK-NEXT:  bb:
; CHECK-NEXT:    [[TMP6:%.*]] = icmp ne i64 [[TMP]], 0
; CHECK-NEXT:    ret i1 [[TMP6]]
;
bb:
  %tmp4 = getelementptr i8, ptr addrspace(200) null, i64 %tmp
  %tmp5 = bitcast ptr addrspace(200) %tmp4 to ptr addrspace(200)
  %tmp6 = icmp ne ptr addrspace(200) %tmp5, null
  ret i1 %tmp6
}

; These two can be folded to a constant return:

define i1 @known_bits_cap(i64 %arg) local_unnamed_addr addrspace(200) {
; CHECK-LABEL: define {{[^@]+}}@known_bits_cap
; CHECK-SAME: (i64 [[ARG:%.*]]) local_unnamed_addr addrspace(200) {
; CHECK-NEXT:  bb:
; CHECK-NEXT:    ret i1 true
;
bb:
  %greater_zero = add nuw nsw i64 %arg, 1
  %tmp4 = getelementptr i8, ptr addrspace(200) null, i64 %greater_zero
  %tmp5 = bitcast ptr addrspace(200) %tmp4 to ptr addrspace(200)
  %tmp6 = icmp ugt ptr addrspace(200) %tmp5, null
  ret i1 %tmp6
}

define i1 @known_bits_i64(i64 %arg) local_unnamed_addr addrspace(200) {
; CHECK-LABEL: define {{[^@]+}}@known_bits_i64
; CHECK-SAME: (i64 [[ARG:%.*]]) local_unnamed_addr addrspace(200) {
; CHECK-NEXT:  bb:
; CHECK-NEXT:    ret i1 true
;
bb:
  %greater_zero = add nuw nsw i64 %arg, 1
  %tmp6 = icmp ugt i64 %greater_zero, 0
  ret i1 %tmp6
}
