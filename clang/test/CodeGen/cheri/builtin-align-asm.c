// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --function-signature
// REQUIRES: mips-registered-target
// RUN: %cheri_purecap_cc1 -o - -O2 -emit-llvm %s | FileCheck %s
// RUN: %cheri_purecap_cc1 -o - -O2 -S %s | FileCheck %s -check-prefixes=ASM,PURECAP-ASM
// RUN: %cheri_cc1 -o - -O2 -S %s | FileCheck %s -check-prefixes=ASM,MIPS-ASM

// CHECK-LABEL: define {{[^@]+}}@is_aligned
// CHECK-SAME: (ptr addrspace(200) noundef [[PTR:%.*]], i64 noundef signext [[ALIGN:%.*]]) local_unnamed_addr addrspace(200) #[[ATTR0:[0-9]+]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[MASK:%.*]] = add i64 [[ALIGN]], -1
// CHECK-NEXT:    [[PTRADDR:%.*]] = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) [[PTR]])
// CHECK-NEXT:    [[SET_BITS:%.*]] = and i64 [[PTRADDR]], [[MASK]]
// CHECK-NEXT:    [[IS_ALIGNED:%.*]] = icmp eq i64 [[SET_BITS]], 0
// CHECK-NEXT:    ret i1 [[IS_ALIGNED]]
//
_Bool is_aligned(void *ptr, long align) {
  // ASM-LABEL: is_aligned:
  // PURECAP-ASM:      daddiu	$1, $4, -1
  // PURECAP-ASM-NEXT: cgetandaddr	$1, $c3, $1
  // PURECAP-ASM-NEXT: cjr	$c17
  // PURECAP-ASM-NEXT: sltiu	$2, $1, 1
  // MIPS-ASM:      daddiu	$1, $5, -1
  // MIPS-ASM-NEXT: and	$1, $1, $4
  // MIPS-ASM-NEXT: jr	$ra
  // MIPS-ASM-NEXT: sltiu	$2, $1, 1
  return __builtin_is_aligned(ptr, align);
}

// CHECK-LABEL: define {{[^@]+}}@align_up
// CHECK-SAME: (ptr addrspace(200) noundef [[PTR:%.*]], i64 noundef signext [[ALIGN:%.*]]) local_unnamed_addr addrspace(200) #[[ATTR2:[0-9]+]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[MASK:%.*]] = add i64 [[ALIGN]], -1
// CHECK-NEXT:    [[PTRADDR:%.*]] = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) [[PTR]])
// CHECK-NEXT:    [[OVER_BOUNDARY:%.*]] = add i64 [[MASK]], [[PTRADDR]]
// CHECK-NEXT:    [[INVERTED_MASK:%.*]] = sub i64 0, [[ALIGN]]
// CHECK-NEXT:    [[ALIGNED_INTPTR:%.*]] = and i64 [[OVER_BOUNDARY]], [[INVERTED_MASK]]
// CHECK-NEXT:    [[DIFF:%.*]] = sub i64 [[ALIGNED_INTPTR]], [[PTRADDR]]
// CHECK-NEXT:    [[ALIGNED_RESULT:%.*]] = getelementptr inbounds i8, ptr addrspace(200) [[PTR]], i64 [[DIFF]]
// CHECK-NEXT:    call void @llvm.assume(i1 true) [ "align"(ptr addrspace(200) [[ALIGNED_RESULT]], i64 [[ALIGN]]) ]
// CHECK-NEXT:    ret ptr addrspace(200) [[ALIGNED_RESULT]]
//
void* align_up(void *ptr, long align) {
  // ASM-LABEL: align_up:
  // PURECAP-ASM:      cgetaddr	$1, $c3
  // PURECAP-ASM-NEXT: daddu $2, $4, $1
  // PURECAP-ASM-NEXT: daddiu $2, $2, -1
  // PURECAP-ASM-NEXT: dnegu $3, $4
  // PURECAP-ASM-NEXT: and $2, $2, $3
  // PURECAP-ASM-NEXT: dsubu $1, $2, $1
  // PURECAP-ASM-NEXT: cjr	$c17
  // PURECAP-ASM-NEXT: cincoffset $c3, $c3, $1
  // MIPS-ASM:      daddu	$1, $5, $4
  // MIPS-ASM-NEXT: daddiu	$1, $1, -1
  // MIPS-ASM-NEXT: dnegu	$2, $5
  // MIPS-ASM-NEXT: jr	$ra
  // MIPS-ASM-NEXT: and	$2, $1, $2
  return __builtin_align_up(ptr, align);
}

// CHECK-LABEL: define {{[^@]+}}@align_down
// CHECK-SAME: (ptr addrspace(200) noundef [[PTR:%.*]], i64 noundef signext [[ALIGN:%.*]]) local_unnamed_addr addrspace(200) #[[ATTR2]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[PTRADDR:%.*]] = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) [[PTR]])
// CHECK-NEXT:    [[TMP0:%.*]] = add i64 [[ALIGN]], -1
// CHECK-NEXT:    [[TMP1:%.*]] = and i64 [[PTRADDR]], [[TMP0]]
// CHECK-NEXT:    [[DIFF:%.*]] = sub i64 0, [[TMP1]]
// CHECK-NEXT:    [[ALIGNED_RESULT:%.*]] = getelementptr inbounds i8, ptr addrspace(200) [[PTR]], i64 [[DIFF]]
// CHECK-NEXT:    call void @llvm.assume(i1 true) [ "align"(ptr addrspace(200) [[ALIGNED_RESULT]], i64 [[ALIGN]]) ]
// CHECK-NEXT:    ret ptr addrspace(200) [[ALIGNED_RESULT]]
//
void* align_down(void *ptr, long align) {
  // ASM-LABEL: align_down:
  // PURECAP-ASM:      dnegu	$1, $4
  // PURECAP-ASM-NEXT: cjr	$c17
  // PURECAP-ASM-NEXT: candaddr	$c3, $c3, $1
  // MIPS-ASM:      dnegu	$1, $5
  // MIPS-ASM-NEXT: jr	$ra
  // MIPS-ASM-NEXT: and	$2, $4, $1
  return __builtin_align_down(ptr, align);
}
