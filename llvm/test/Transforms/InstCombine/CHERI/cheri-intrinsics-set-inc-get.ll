; RUN: opt -S -passes=instcombine %s -o - | FileCheck %s
target datalayout = "pf200:128:128:128:64-A200-P200-G200"

declare i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200)) addrspace(200)
declare ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200), i64) addrspace(200)
declare i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200)) addrspace(200)
declare ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200), i64) addrspace(200)

define i64 @fold_set_and_inc_offset_get_offset(ptr addrspace(200) %arg) addrspace(200) {
  %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) %arg, i64 42)
  %inc_offset = getelementptr i8, ptr addrspace(200) %with_offset, i64 100
  %ret = tail call i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200) %inc_offset)
  ret i64 %ret
  ; CHECK-LABEL: @fold_set_and_inc_offset_get_offset(ptr addrspace(200) %arg)
  ; CHECK: ret i64 142
}

define i64 @no_fold_set_and_inc_offset_get_addr(ptr addrspace(200) %arg) addrspace(200) {
  ; a getaddr can no be inferred from a setoffset since we don't know the base:
  ; However the offset increment can be folded into a single setoffset
  %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) %arg, i64 42)
  %inc_offset = getelementptr i8, ptr addrspace(200) %with_offset, i64 100
  %ret = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) %inc_offset)
  ret i64 %ret
  ; The %with_offset will be removed by dead code elimination later in the pipeline
  ; CHECK-LABEL: @no_fold_set_and_inc_offset_get_addr(ptr addrspace(200) %arg)
  ; CHECK: %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) %arg, i64 142)
  ; CHECK: %ret = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) %with_offset)
  ; CHECK: ret i64 %ret
}

define i64 @fold_set_and_multiple_inc_offset_get_offset(ptr addrspace(200) %arg) addrspace(200) {
  %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) %arg, i64 42)
  %inc_offset = getelementptr i8, ptr addrspace(200) %with_offset, i64 100
  %inc_offset2 = getelementptr i8, ptr addrspace(200) %inc_offset, i64 100
  %ret = tail call i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200) %inc_offset2)
  ret i64 %ret
  ; CHECK-LABEL: @fold_set_and_multiple_inc_offset_get_offset(ptr addrspace(200) %arg)
  ; CHECK: ret i64 242
}

define i64 @fold_set_addr_and_inc_offset_get_offset(ptr addrspace(200) %arg) addrspace(200) {
  ; a getoffset can no be inferred from a setaddr since we don't know the base:
  ; But we acn fold the inc-offset into the setaddr:
  %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) %arg, i64 42)
  %inc_offset = getelementptr i8, ptr addrspace(200) %with_offset, i64 100
  %ret = tail call i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200) %inc_offset)
  ret i64 %ret
  ; CHECK-LABEL: @fold_set_addr_and_inc_offset_get_offset(ptr addrspace(200) %arg)
  ; CHECK: %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) %arg, i64 142)
  ; CHECK: %ret = tail call i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: ret i64 %ret
}

define i64 @fold_set_addr_and_inc_offset_get_addr(ptr addrspace(200) %arg) addrspace(200) {
  ; a getoffset can no be inferred from a setaddr since we don't know the base:
  %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) %arg, i64 42)
  %inc_offset = getelementptr i8, ptr addrspace(200) %with_offset, i64 100
  %ret = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) %inc_offset)
  ret i64 %ret
  ; CHECK-LABEL: @fold_set_addr_and_inc_offset_get_addr(ptr addrspace(200) %arg)
  ; CHECK: ret i64 142
}

define i64 @fold_set_addr_and_multiple_inc_offset_get_addr(ptr addrspace(200) %arg) addrspace(200) {
  %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) %arg, i64 42)
  %inc_offset = getelementptr i8, ptr addrspace(200) %with_offset, i64 100
  %inc_offset2 = getelementptr i8, ptr addrspace(200) %inc_offset, i64 100
  %ret = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) %inc_offset2)
  ret i64 %ret
  ; CHECK-LABEL: @fold_set_addr_and_multiple_inc_offset_get_addr(ptr addrspace(200) %arg)
  ; CHECK: ret i64 242
}
