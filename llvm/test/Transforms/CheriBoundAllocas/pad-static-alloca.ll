; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: %riscv64_cheri_purecap_opt -instsimplify -cheri-bound-allocas -S < %s | FileCheck %s
target datalayout = "E-m:m-pf200:128:128:128:64-i8:8:32-i16:16:32-i64:64-n32:64-S128-A200-P200-G200"

declare void @keep_live(ptr addrspace(200)) local_unnamed_addr addrspace(200)

define void @small() local_unnamed_addr addrspace(200) {
; CHECK-LABEL: @small(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP0:%.*]] = alloca [10 x i8], align 1, addrspace(200)
; CHECK-NEXT:    [[TMP1:%.*]] = call ptr addrspace(200) @llvm.cheri.bounded.stack.cap.i64(ptr addrspace(200) [[TMP0]], i64 10)
; CHECK-NEXT:    call void @keep_live(ptr addrspace(200) nonnull [[TMP1]])
; CHECK-NEXT:    ret void
;
entry:
  %0 = alloca [10 x i8], align 1, addrspace(200)
  %ptr = getelementptr inbounds [10 x i8], ptr addrspace(200) %0, i64 0, i64 0
  call void @keep_live(ptr addrspace(200) nonnull %ptr)
  ret void
}

define void @pad_large() local_unnamed_addr addrspace(200) {
; CHECK-LABEL: @pad_large(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP0:%.*]] = alloca { [16388 x i8], [28 x i8] }, align 32, addrspace(200)
; CHECK-NEXT:    [[TMP1:%.*]] = call ptr addrspace(200) @llvm.cheri.bounded.stack.cap.i64(ptr addrspace(200) [[TMP0]], i64 16416)
; CHECK-NEXT:    call void @keep_live(ptr addrspace(200) nonnull [[TMP1]])
; CHECK-NEXT:    ret void
;
entry:
  %0 = alloca [16388 x i8], align 1, addrspace(200)
  %ptr = getelementptr inbounds [16388 x i8], ptr addrspace(200) %0, i64 0, i64 0
  call void @keep_live(ptr addrspace(200) nonnull %ptr)
  ret void
}

define void @nopad_large() local_unnamed_addr addrspace(200) {
; CHECK-LABEL: @nopad_large(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[TMP0:%.*]] = alloca [16388 x i8], align 32, addrspace(200)
; CHECK-NEXT:    store volatile i8 0, ptr addrspace(200) [[TMP0]], align 1
; CHECK-NEXT:    ret void
;
entry:
  %0 = alloca [16388 x i8], align 1, addrspace(200)
  %ptr = getelementptr inbounds [16388 x i8], ptr addrspace(200) %0, i64 0, i64 0
  store volatile i8 0, ptr addrspace(200) %ptr, align 1
  ret void
}

;; A large alloca with a struct type - incorrect handling of the new alloca type
;; resulted in invalid IR after catching up with API changes when merging to LLVM 14.

%struct.snmp_pdu = type { [100 x %struct.asn_oid], i32 }
%struct.asn_oid = type { [28 x i32] }

@snmp_discover_engine_resp = addrspace(200) global %struct.snmp_pdu zeroinitializer, align 4
declare void @snmp_pdu_free(%struct.snmp_pdu addrspace(200)* noundef) addrspace(200)

define dso_local void @snmp_discover_engine(ptr addrspace(200) noalias sret(%struct.snmp_pdu) align 4 %agg.result) addrspace(200) #0 {
; CHECK-LABEL: @snmp_discover_engine(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[BYVAL_TEMP:%.*]] = alloca { [[STRUCT_SNMP_PDU:%.*]], [12 x i8] }, align 16, addrspace(200)
; CHECK-NEXT:    [[TMP0:%.*]] = call ptr addrspace(200) @llvm.cheri.bounded.stack.cap.i64(ptr addrspace(200) [[BYVAL_TEMP]], i64 11216)
; CHECK-NEXT:    call void @snmp_pdu_free(ptr addrspace(200) [[TMP0]])
; CHECK-NEXT:    ret void
;
entry:
  %byval-temp = alloca %struct.snmp_pdu, align 4, addrspace(200)
  call void @snmp_pdu_free(ptr addrspace(200) %byval-temp)
  ret void
}

declare void @llvm.memcpy.p200.p200.i64(ptr addrspace(200) noalias nocapture writeonly, ptr addrspace(200) noalias nocapture readonly, i64, i1 immarg) addrspace(200) #1
