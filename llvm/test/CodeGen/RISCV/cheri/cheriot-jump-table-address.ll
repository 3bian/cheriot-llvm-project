; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py UTC_ARGS: --version 2
; RUN: llc --filetype=asm --mcpu=cheriot --mtriple=riscv32-unknown-unknown -target-abi cheriot  %s -mattr=+xcheri,+cap-mode -o - | FileCheck %s

target datalayout = "e-m:e-pf200:64:64:64:32-p:32:32-i64:64-n32-S128-A200-P200-G200"
target triple = "riscv32-unknown-unknown"

define [2 x i32] @foo(i32 %searched) addrspace(200) {
; CHECK-LABEL: foo:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    cincoffset csp, csp, -16
; CHECK-NEXT:    .cfi_def_cfa_offset 16
; CHECK-NEXT:    csc cra, 8(csp) # 8-byte Folded Spill
; CHECK-NEXT:    .cfi_offset ra, -8
; CHECK-NEXT:    addi a0, a0, 1
; CHECK-NEXT:    li a1, 4
; CHECK-NEXT:    bltu a1, a0, .LBB0_2
; CHECK-NEXT:  # %bb.1: # %entry
; CHECK-NEXT:    slli a0, a0, 2
; CHECK-NEXT:  .LBB0_3: # %entry
; CHECK-NEXT:    # Label of block must be emitted
; CHECK-NEXT:    auipcc ca1, %cheriot_compartment_hi(.LJTI0_0)
; CHECK-NEXT:    cincoffset ca1, ca1, %cheriot_compartment_lo_i(.LBB0_3)
; CHECK-NEXT:    cincoffset ca0, ca1, a0
; CHECK-NEXT:    clw a0, 0(ca0)
; CHECK-NEXT:  .LBB0_4: # %entry
; CHECK-NEXT:    # Label of block must be emitted
; CHECK-NEXT:    auipcc ca1, %cheriot_compartment_hi(.Lfoo$jump_table_base)
; CHECK-NEXT:    cincoffset ca1, ca1, %cheriot_compartment_lo_i(.LBB0_4)
; CHECK-NEXT:    cincoffset ca0, ca1, a0
; CHECK-NEXT:    cjr ca0
; CHECK-NEXT:  .LBB0_2: # %cleanup
; CHECK-NEXT:    li a0, 0
; CHECK-NEXT:    li a1, 0
; CHECK-NEXT:    clc cra, 8(csp) # 8-byte Folded Reload
; CHECK-NEXT:    cincoffset csp, csp, 16
; CHECK-NEXT:    cret
entry:
  switch i32 %searched, label %sw.epilog [
    i32 -1, label %cleanup
    i32 0, label %sw.bb5
    i32 1, label %sw.bb13
    i32 2, label %sw.bb21
    i32 3, label %sw.bb29
  ]

sw.bb5:                                           ; preds = %entry
  br label %cleanup

sw.bb13:                                          ; preds = %entry
  br label %cleanup

sw.bb21:                                          ; preds = %entry
  br label %cleanup

sw.bb29:                                          ; preds = %entry
  br label %cleanup

sw.epilog:                                        ; preds = %entry
  br label %cleanup

cleanup:                                          ; preds = %sw.epilog, %sw.bb29, %sw.bb21, %sw.bb13, %sw.bb5, %entry
  %call330.pn = phi [2 x i32] [ zeroinitializer, %sw.epilog ], [ zeroinitializer, %sw.bb5 ], [ zeroinitializer, %sw.bb13 ], [ zeroinitializer, %sw.bb21 ], [ zeroinitializer, %sw.bb29 ], [ [i32 -1, i32 1], %entry ]
  ret [2 x i32] zeroinitializer
}
