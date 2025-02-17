; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: %cheri_purecap_llc -o - -cheri-cap-table-abi=plt -mxcaptable=false %s | FileCheck %s

%struct.state = type { i32, i32, i32, i32, i32, i32, [1200 x i64], [1200 x i8], [512 x i8]}

@lclmem = external addrspace(200) global %struct.state, align 8
@gmtmem = external addrspace(200) global %struct.state, align 8

define ptr addrspace(200) @standard_load(i1 %arg) #0noinline nounwind optnone {
; CHECK-LABEL: standard_load:
; CHECK:       # %bb.0:
; CHECK-NEXT:    clcbi $c3, %captab20(lclmem)($c26)
; CHECK-NEXT:    cjr $c17
; CHECK-NEXT:    nop
  ret ptr addrspace(200) @lclmem
}

define ptr addrspace(200) @ternary(i1 %arg) noinline nounwind optnone {
; CHECK-LABEL: ternary:
; CHECK:       # %bb.0:
; CHECK-NEXT:    sll $1, $4, 0
; CHECK-NEXT:    andi $1, $1, 1
; CHECK-NEXT:    daddiu $2, $zero, %captab20(gmtmem)
; CHECK-NEXT:    dsll $2, $2, 4
; CHECK-NEXT:    cincoffset $c1, $c26, $2
; CHECK-NEXT:    daddiu $2, $zero, %captab20(lclmem)
; CHECK-NEXT:    dsll $2, $2, 4
; CHECK-NEXT:    cincoffset $c2, $c26, $2
; CHECK-NEXT:    cmovn $c1, $c2, $1
; CHECK-NEXT:    clc $c3, $zero, 0($c1)
; CHECK-NEXT:    cjr $c17
; CHECK-NEXT:    nop
  %cond = select i1 %arg, ptr addrspace(200) @lclmem, ptr addrspace(200) @gmtmem
  ret ptr addrspace(200) %cond
}
