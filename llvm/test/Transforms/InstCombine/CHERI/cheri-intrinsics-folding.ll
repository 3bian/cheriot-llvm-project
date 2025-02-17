; RUN: %cheri_opt -S -passes=instcombine %s -o - | FileCheck %s
target datalayout = "pf200:128:128:128:64-A200-P200-G200"


declare i64 @check_fold(i64) addrspace(200)
declare ptr addrspace(200) @check_fold_i8ptr(ptr addrspace(200)) addrspace(200)

declare i64 @llvm.cheri.cap.base.get.i64(ptr addrspace(200)) addrspace(200)
declare ptr addrspace(200) @llvm.cheri.cap.base.set(ptr addrspace(200), i64) addrspace(200)
declare i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200)) addrspace(200)
declare i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200)) addrspace(200)
declare ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200), i64) addrspace(200)
declare ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200), i64) addrspace(200)
declare i64 @llvm.cheri.cap.length.get.i64(ptr addrspace(200)) addrspace(200)
declare ptr addrspace(200) @llvm.cheri.cap.length.set(ptr addrspace(200), i64) addrspace(200)
declare i64 @llvm.cheri.cap.perms.get.i64(ptr addrspace(200)) addrspace(200)
declare ptr addrspace(200) @llvm.cheri.cap.perms.and.i64(ptr addrspace(200), i64) addrspace(200)
declare i64 @llvm.cheri.cap.flags.get.i64(ptr addrspace(200)) addrspace(200)
declare i64 @llvm.cheri.cap.type.get.i64(ptr addrspace(200)) addrspace(200)
declare i1 @llvm.cheri.cap.sealed.get(ptr addrspace(200)) addrspace(200)
declare i1 @llvm.cheri.cap.tag.get(ptr addrspace(200)) addrspace(200)

define i64 @null_get_offset() addrspace(200) nounwind {
  %ret = tail call i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200) null)
  ret i64 %ret
  ; CHECK-LABEL: @null_get_offset()
  ; CHECK-NEXT: ret i64 0
}

define i64 @null_get_base() addrspace(200) nounwind {
  %ret = tail call i64 @llvm.cheri.cap.base.get.i64(ptr addrspace(200) null)
  ret i64 %ret
  ; CHECK-LABEL: @null_get_base()
  ; CHECK-NEXT: ret i64 0
}

define i64 @null_get_address() addrspace(200) nounwind {
  %ret = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) null)
  ret i64 %ret
  ; CHECK-LABEL: @null_get_address()
  ; CHECK-NEXT: ret i64 0
}

define i64 @null_get_perms() addrspace(200) nounwind {
  %ret = tail call i64 @llvm.cheri.cap.perms.get.i64(ptr addrspace(200) null)
  ret i64 %ret
  ; CHECK-LABEL: @null_get_perms()
  ; CHECK-NEXT: ret i64 0
}

define i64 @null_get_flags() addrspace(200) nounwind {
  %ret = tail call i64 @llvm.cheri.cap.flags.get.i64(ptr addrspace(200) null)
  ret i64 %ret
  ; CHECK-LABEL: @null_get_flags()
  ; CHECK-NEXT: ret i64 0
}

define i64 @null_get_sealed() addrspace(200) nounwind {
  %ret = tail call i1 @llvm.cheri.cap.sealed.get(ptr addrspace(200) null)
  %ret.ext = zext i1 %ret to i64
  ret i64 %ret.ext
  ; CHECK-LABEL: @null_get_sealed()
  ; CHECK-NEXT: ret i64 0
}

define i64 @null_get_tag() addrspace(200) nounwind {
  %ret = tail call i1 @llvm.cheri.cap.tag.get(ptr addrspace(200) null)
  %ret.ext = zext i1 %ret to i64
  ret i64 %ret.ext
  ; CHECK-LABEL: @null_get_tag()
  ; CHECK-NEXT: ret i64 0
}

; Note: we should not optimize gettype and getlength since those values may vary
; across implementations, and the optimization probably doesn't help much anyway
define i64 @null_get_length() addrspace(200) nounwind {
  %ret = tail call i64 @llvm.cheri.cap.length.get.i64(ptr addrspace(200) null)
  ret i64 %ret
  ; CHECK-LABEL: @null_get_length()
  ; CHECK-NEXT:  %ret = tail call i64 @llvm.cheri.cap.length.get.i64(ptr addrspace(200) null)
  ; CHECK-NEXT:  ret i64 %ret
}
define i64 @null_get_type() addrspace(200) nounwind {
  %ret = tail call i64 @llvm.cheri.cap.type.get.i64(ptr addrspace(200) null)
  ret i64 %ret
  ; CHECK-LABEL: @null_get_type()
  ; CHECK-NEXT:  %ret = tail call i64 @llvm.cheri.cap.type.get.i64(ptr addrspace(200) null)
  ; CHECK-NEXT:  ret i64 %ret
}



define void @infer_values_from_null_set_offset() addrspace(200) nounwind {
  ; CHECK-LABEL: @infer_values_from_null_set_offset()
  %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) null, i64 123456)

  %base = tail call i64 @llvm.cheri.cap.base.get.i64(ptr addrspace(200) nonnull %with_offset)
  %base_check = tail call i64 @check_fold(i64 %base)
  ; CHECK:  %base_check = tail call i64 @check_fold(i64 0)

  %offset = tail call i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200) nonnull %with_offset)
  %offset_check = tail call i64 @check_fold(i64 %offset)
  ; CHECK:  %offset_check = tail call i64 @check_fold(i64 123456)

  %address = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) nonnull %with_offset)
  %address_check = tail call i64 @check_fold(i64 %address)
  ; CHECK:  %address_check = tail call i64 @check_fold(i64 123456)

  %sealed = tail call i1 @llvm.cheri.cap.sealed.get(ptr addrspace(200) nonnull %with_offset)
  %sealed.ext = zext i1 %sealed to i64
  %sealed_check = tail call i64 @check_fold(i64 %sealed.ext)
  ; CHECK:  %sealed_check = tail call i64 @check_fold(i64 0)

  %perms = tail call i64 @llvm.cheri.cap.perms.get.i64(ptr addrspace(200) nonnull %with_offset)
  %perms_check = tail call i64 @check_fold(i64 %perms)
  ; CHECK:  %perms_check = tail call i64 @check_fold(i64 0)

  %flags = tail call i64 @llvm.cheri.cap.flags.get.i64(ptr addrspace(200) nonnull %with_offset)
  %flags_check = tail call i64 @check_fold(i64 %flags)
  ; CHECK:  %flags_check = tail call i64 @check_fold(i64 0)

  %tag = tail call i1 @llvm.cheri.cap.tag.get(ptr addrspace(200) nonnull %with_offset)
  %tag.ext = zext i1 %tag to i64
  %tag_check = tail call i64 @check_fold(i64 %tag.ext)
  ; CHECK:  %tag_check = tail call i64 @check_fold(i64 0)

  ; Length and type should not be optimized:
  %length = tail call i64 @llvm.cheri.cap.length.get.i64(ptr addrspace(200) nonnull %with_offset)
  %length_check = tail call i64 @check_fold(i64 %length)
  ; CHECK: %length = tail call i64 @llvm.cheri.cap.length.get.i64(ptr addrspace(200) nonnull getelementptr (i8, ptr addrspace(200) null, i64 123456))
  ; CHECK: %length_check = tail call i64 @check_fold(i64 %length)
  %type = tail call i64 @llvm.cheri.cap.type.get.i64(ptr addrspace(200) nonnull %with_offset)
  %type_check = tail call i64 @check_fold(i64 %type)
  ; CHECK:  %type = tail call i64 @llvm.cheri.cap.type.get.i64(ptr addrspace(200) nonnull getelementptr (i8, ptr addrspace(200) null, i64 123456))
  ; CHECK:  %type_check = tail call i64 @check_fold(i64 %type)

  ret void
  ; CHECK: ret void
}

define void @infer_values_from_null_set_address() addrspace(200) nounwind {
  ; CHECK-LABEL: @infer_values_from_null_set_address()
  %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) null, i64 123456)

  %base = tail call i64 @llvm.cheri.cap.base.get.i64(ptr addrspace(200) nonnull %with_offset)
  %base_check = tail call i64 @check_fold(i64 %base)
  ; CHECK:  %base_check = tail call i64 @check_fold(i64 0)

  %offset = tail call i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200) nonnull %with_offset)
  %offset_check = tail call i64 @check_fold(i64 %offset)
  ; CHECK:  %offset_check = tail call i64 @check_fold(i64 123456)

  %address = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) nonnull %with_offset)
  %address_check = tail call i64 @check_fold(i64 %address)
  ; CHECK:  %address_check = tail call i64 @check_fold(i64 123456)

  %sealed = tail call i1 @llvm.cheri.cap.sealed.get(ptr addrspace(200) nonnull %with_offset)
  %sealed.ext = zext i1 %sealed to i64
  %sealed_check = tail call i64 @check_fold(i64 %sealed.ext)
  ; CHECK:  %sealed_check = tail call i64 @check_fold(i64 0)

  %perms = tail call i64 @llvm.cheri.cap.perms.get.i64(ptr addrspace(200) nonnull %with_offset)
  %perms_check = tail call i64 @check_fold(i64 %perms)
  ; CHECK:  %perms_check = tail call i64 @check_fold(i64 0)

  %flags = tail call i64 @llvm.cheri.cap.flags.get.i64(ptr addrspace(200) nonnull %with_offset)
  %flags_check = tail call i64 @check_fold(i64 %flags)
  ; CHECK:  %flags_check = tail call i64 @check_fold(i64 0)

  %tag = tail call i1 @llvm.cheri.cap.tag.get(ptr addrspace(200) nonnull %with_offset)
  %tag.ext = zext i1 %tag to i64
  %tag_check = tail call i64 @check_fold(i64 %tag.ext)
  ; CHECK:  %tag_check = tail call i64 @check_fold(i64 0)

  ; Length and type should not be optimized:
  %length = tail call i64 @llvm.cheri.cap.length.get.i64(ptr addrspace(200) nonnull %with_offset)
  %length_check = tail call i64 @check_fold(i64 %length)
  ; CHECK: %length = tail call i64 @llvm.cheri.cap.length.get.i64(ptr addrspace(200) nonnull getelementptr (i8, ptr addrspace(200) null, i64 123456))
  ; CHECK: %length_check = tail call i64 @check_fold(i64 %length)
  %type = tail call i64 @llvm.cheri.cap.type.get.i64(ptr addrspace(200) nonnull %with_offset)
  %type_check = tail call i64 @check_fold(i64 %type)
  ; CHECK:  %type = tail call i64 @llvm.cheri.cap.type.get.i64(ptr addrspace(200) nonnull getelementptr (i8, ptr addrspace(200) null, i64 123456))
  ; CHECK:  %type_check = tail call i64 @check_fold(i64 %type)

  ret void
  ; CHECK: ret void
}

; Only the getaddr should be inferred, all other values are unknown if we call setaddr on an unknown value:
define void @infer_values_from_arg_set_address(ptr addrspace(200) %arg) addrspace(200) nounwind {
  ; CHECK-LABEL: @infer_values_from_arg_set_address(ptr addrspace(200)
  %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) %arg, i64 123456)

  %base = tail call i64 @llvm.cheri.cap.base.get.i64(ptr addrspace(200) nonnull %with_offset)
  %base_check = tail call i64 @check_fold(i64 %base)
  ; CHECK: %base = tail call i64 @llvm.cheri.cap.base.get.i64(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %base_check = tail call i64 @check_fold(i64 %base)

  %length = tail call i64 @llvm.cheri.cap.length.get.i64(ptr addrspace(200) nonnull %with_offset)
  %length_check = tail call i64 @check_fold(i64 %length)
  ; CHECK: %length = tail call i64 @llvm.cheri.cap.length.get.i64(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %length_check = tail call i64 @check_fold(i64 %length)

  %offset = tail call i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200) nonnull %with_offset)
  %offset_check = tail call i64 @check_fold(i64 %offset)
  ; CHECK: %offset = tail call i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %offset_check = tail call i64 @check_fold(i64 %offset)

  %address = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) nonnull %with_offset)
  %address_check = tail call i64 @check_fold(i64 %address)
  ; CHECK-NOT: @llvm.cheri.cap.address.get.i64
  ; CHECK: %address_check = tail call i64 @check_fold(i64 123456)

  %type = tail call i64 @llvm.cheri.cap.type.get.i64(ptr addrspace(200) nonnull %with_offset)
  %type_check = tail call i64 @check_fold(i64 %type)
  ; CHECK: %type = tail call i64 @llvm.cheri.cap.type.get.i64(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %type_check = tail call i64 @check_fold(i64 %type)

  %sealed = tail call i1 @llvm.cheri.cap.sealed.get(ptr addrspace(200) nonnull %with_offset)
  %sealed.ext = zext i1 %sealed to i64
  %sealed_check = tail call i64 @check_fold(i64 %sealed.ext)
  ; CHECK: %sealed = tail call i1 @llvm.cheri.cap.sealed.get(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %sealed.ext = zext i1 %sealed to i64
  ; CHECK: %sealed_check = tail call i64 @check_fold(i64 %sealed.ext)

  %perms = tail call i64 @llvm.cheri.cap.perms.get.i64(ptr addrspace(200) nonnull %with_offset)
  %perms_check = tail call i64 @check_fold(i64 %perms)
  ; CHECK: %perms = tail call i64 @llvm.cheri.cap.perms.get.i64(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %perms_check = tail call i64 @check_fold(i64 %perms)

  %flags = tail call i64 @llvm.cheri.cap.flags.get.i64(ptr addrspace(200) nonnull %with_offset)
  %flags_check = tail call i64 @check_fold(i64 %flags)
  ; CHECK: %flags = tail call i64 @llvm.cheri.cap.flags.get.i64(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %flags_check = tail call i64 @check_fold(i64 %flags)

  %tag = tail call i1 @llvm.cheri.cap.tag.get(ptr addrspace(200) nonnull %with_offset)
  %tag.ext = zext i1 %tag to i64
  %tag_check = tail call i64 @check_fold(i64 %tag.ext)
  ; CHECK: %tag = tail call i1 @llvm.cheri.cap.tag.get(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %tag.ext = zext i1 %tag to i64
  ; CHECK: %tag_check = tail call i64 @check_fold(i64 %tag.ext)

  ret void
  ; CHECK: ret void
}

; Only the getoffset should be inferred, all other values are unknown if we call setoffset on an unknown value:
define void @infer_values_from_arg_set_offset(ptr addrspace(200) %arg) addrspace(200) nounwind {
  ; CHECK-LABEL: @infer_values_from_arg_set_offset(ptr addrspace(200)
  %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) %arg, i64 123456)

  %base = tail call i64 @llvm.cheri.cap.base.get.i64(ptr addrspace(200) nonnull %with_offset)
  %base_check = tail call i64 @check_fold(i64 %base)
  ; CHECK: %base = tail call i64 @llvm.cheri.cap.base.get.i64(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %base_check = tail call i64 @check_fold(i64 %base)

  %length = tail call i64 @llvm.cheri.cap.length.get.i64(ptr addrspace(200) nonnull %with_offset)
  %length_check = tail call i64 @check_fold(i64 %length)
  ; CHECK: %length = tail call i64 @llvm.cheri.cap.length.get.i64(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %length_check = tail call i64 @check_fold(i64 %length)

  %offset = tail call i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200) nonnull %with_offset)
  %offset_check = tail call i64 @check_fold(i64 %offset)
  ; CHECK-NOT: @llvm.cheri.cap.offset.get.i64
  ; CHECK: %offset_check = tail call i64 @check_fold(i64 123456)

  %address = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) nonnull %with_offset)
  %address_check = tail call i64 @check_fold(i64 %address)
  ; CHECK: %address = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %address_check = tail call i64 @check_fold(i64 %address)

  %type = tail call i64 @llvm.cheri.cap.type.get.i64(ptr addrspace(200) nonnull %with_offset)
  %type_check = tail call i64 @check_fold(i64 %type)
  ; CHECK: %type = tail call i64 @llvm.cheri.cap.type.get.i64(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %type_check = tail call i64 @check_fold(i64 %type)

  %sealed = tail call i1 @llvm.cheri.cap.sealed.get(ptr addrspace(200) nonnull %with_offset)
  %sealed.ext = zext i1 %sealed to i64
  %sealed_check = tail call i64 @check_fold(i64 %sealed.ext)
  ; CHECK: %sealed = tail call i1 @llvm.cheri.cap.sealed.get(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %sealed.ext = zext i1 %sealed to i64
  ; CHECK: %sealed_check = tail call i64 @check_fold(i64 %sealed.ext)

  %perms = tail call i64 @llvm.cheri.cap.perms.get.i64(ptr addrspace(200) nonnull %with_offset)
  %perms_check = tail call i64 @check_fold(i64 %perms)
  ; CHECK: %perms = tail call i64 @llvm.cheri.cap.perms.get.i64(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %perms_check = tail call i64 @check_fold(i64 %perms)

  %flags = tail call i64 @llvm.cheri.cap.flags.get.i64(ptr addrspace(200) nonnull %with_offset)
  %flags_check = tail call i64 @check_fold(i64 %flags)
  ; CHECK: %flags = tail call i64 @llvm.cheri.cap.flags.get.i64(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %flags_check = tail call i64 @check_fold(i64 %flags)

  %tag = tail call i1 @llvm.cheri.cap.tag.get(ptr addrspace(200) nonnull %with_offset)
  %tag.ext = zext i1 %tag to i64
  %tag_check = tail call i64 @check_fold(i64 %tag.ext)
  ; CHECK: %tag = tail call i1 @llvm.cheri.cap.tag.get(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %tag.ext = zext i1 %tag to i64
  ; CHECK: %tag_check = tail call i64 @check_fold(i64 %tag.ext)

  ret void
  ; CHECK: ret void
}


define i64 @fold_set_offset_arg(ptr addrspace(200) %arg) addrspace(200) nounwind {
  %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) %arg, i64 42)
  %ret = tail call i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200) nonnull %with_offset)
  ret i64 %ret
  ; CHECK-LABEL: @fold_set_offset_arg(ptr addrspace(200) %arg)
  ; CHECK: ret i64 42
}

define i64 @no_fold_set_offset_get_address(ptr addrspace(200) %arg) addrspace(200) nounwind {
  %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) %arg, i64 42)
  %ret = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) nonnull %with_offset)
  ret i64 %ret
  ; Since the base is not know we shouldn't fold anything here:
  ; CHECK-LABEL: @no_fold_set_offset_get_address(ptr addrspace(200) %arg)
  ; CHECK: %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) %arg, i64 42)
  ; CHECK: %ret = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: ret i64 %ret
}

define i64 @fold_set_address_get_address(ptr addrspace(200) %arg) addrspace(200) nounwind {
  %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) %arg, i64 42)
  %ret = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) nonnull %with_offset)
  ret i64 %ret
  ; The resulting address will always be 42 but we don't know if it will be tagged or not after this operation
  ; CHECK-LABEL: @fold_set_address_get_address(ptr addrspace(200) %arg)
  ; CHECK: ret i64 42
}

define i64 @no_fold_set_offset_get_tag(ptr addrspace(200) %arg) addrspace(200) nounwind {
  %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) %arg, i64 42)
  %ret = tail call i1 @llvm.cheri.cap.tag.get(ptr addrspace(200) nonnull %with_offset)
  %ret.ext = zext i1 %ret to i64
  ret i64 %ret.ext
  ; Since the value might be unrepresentable after setting the address we can't infer the tag:
  ; CHECK-LABEL: @no_fold_set_offset_get_tag(ptr addrspace(200) %arg)
  ; CHECK: %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) %arg, i64 42)
  ; CHECK: %ret = tail call i1 @llvm.cheri.cap.tag.get(ptr addrspace(200) nonnull %with_offset)
  ; CHECK: %ret.ext = zext i1 %ret to i64
  ; CHECK: ret i64 %ret.ext
}

define i64 @fold_null_inc_offset() addrspace(200) nounwind {
  %inc_offset = getelementptr inbounds i8, ptr addrspace(200) null, i64 100
  %ret = tail call i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200) %inc_offset)
  ret i64 %ret
  ; CHECK-LABEL: @fold_null_inc_offset()
  ; CHECK: ret i64 100
}

define i64 @fold_null_inc_offset_get_address() addrspace(200) nounwind {
  %inc_offset = getelementptr inbounds i8, ptr addrspace(200) null, i64 100
  %ret = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) %inc_offset)
  ret i64 %ret
  ; CHECK-LABEL: @fold_null_inc_offset_get_address()
  ; CHECK: ret i64 100
}

define i64 @fold_null_set_address_get_offset() addrspace(200) nounwind {
  %setaddr = tail call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) null, i64 100)
  %ret = tail call i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200) %setaddr)
  ret i64 %ret
  ; CHECK-LABEL: @fold_null_set_address_get_offset()
  ; CHECK: ret i64 100
}

define i64 @fold_null_set_address_offset_get_address() addrspace(200) nounwind {
  %setaddr = tail call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) null, i64 100)
  %ret = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) %setaddr)
  ret i64 %ret
  ; CHECK-LABEL: @fold_null_set_address_offset_get_address()
  ; CHECK: ret i64 100
}

define i64 @fold_base_get_inttoptr() addrspace(200) nounwind {
  %ret = tail call i64 @llvm.cheri.cap.base.get.i64(ptr addrspace(200) inttoptr (i64 100 to ptr addrspace(200)))
  ret i64 %ret
  ; CHECK-LABEL: @fold_base_get_inttoptr()
  ; CHECK: ret i64 0
}

define i64 @fold_tag_get_inttoptr() addrspace(200) nounwind {
  %ret = tail call i1 @llvm.cheri.cap.tag.get(ptr addrspace(200) inttoptr (i64 100 to ptr addrspace(200)))
  %ret.ext = zext i1 %ret to i64
  ret i64 %ret.ext
  ; CHECK-LABEL: @fold_tag_get_inttoptr()
  ; CHECK: ret i64 0
}

define i64 @fold_offset_get_inttoptr() addrspace(200) nounwind {
  %ret = tail call i64 @llvm.cheri.cap.offset.get.i64(ptr addrspace(200) inttoptr (i64 100 to ptr addrspace(200)))
  ret i64 %ret
  ; CHECK-LABEL: @fold_offset_get_inttoptr()
  ; CHECK: ret i64 100
}

define i64 @fold_address_get_inttoptr() addrspace(200) nounwind {
  %ret = tail call i64 @llvm.cheri.cap.address.get.i64(ptr addrspace(200) inttoptr (i64 100 to ptr addrspace(200)))
  ret i64 %ret
  ; CHECK-LABEL: @fold_address_get_inttoptr()
  ; CHECK: ret i64 100
}

; No folding for gettype and getlength:
define i64 @fold_type_get_inttoptr() addrspace(200) nounwind {
  %ret = tail call i64 @llvm.cheri.cap.type.get.i64(ptr addrspace(200) inttoptr (i64 100 to ptr addrspace(200)))
  ret i64 %ret
  ; CHECK-LABEL: @fold_type_get_inttoptr()
  ; CHECK-NEXT: %ret = tail call i64 @llvm.cheri.cap.type.get.i64(ptr addrspace(200) nonnull inttoptr (i64 100 to ptr addrspace(200)))
  ; CHECK-NEXT: ret i64 %ret
}

define i64 @fold_length_get_inttoptr() addrspace(200) nounwind {
  %ret = tail call i64 @llvm.cheri.cap.length.get.i64(ptr addrspace(200) inttoptr (i64 100 to ptr addrspace(200)))
  ret i64 %ret
  ; CHECK-LABEL: @fold_length_get_inttoptr()
  ; CHECK-NEXT: %ret = tail call i64 @llvm.cheri.cap.length.get.i64(ptr addrspace(200) nonnull inttoptr (i64 100 to ptr addrspace(200)))
  ; CHECK-NEXT: ret i64 %ret
}

define ptr addrspace(200) @fold_set_offset_inc_offset(ptr addrspace(200) %arg) addrspace(200) nounwind {
  %with_offset = tail call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) %arg, i64 42)
  %inc_offset = getelementptr i8, ptr addrspace(200) %with_offset, i64 100
  %inc_offset2 = getelementptr i8, ptr addrspace(200) %inc_offset, i64 100
  ret ptr addrspace(200) %inc_offset2
  ; CHECK-LABEL: @fold_set_offset_inc_offset(ptr addrspace(200) %arg)
  ; CHECK: call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) %arg, i64 242)
}

define ptr addrspace(200) @fold_set_inc_gep_sequence() local_unnamed_addr addrspace(200) nounwind {
entry:
  %set = tail call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) null, i64 100)
  %gep1 = getelementptr inbounds i8, ptr addrspace(200) %set, i64 -4
  %gep2 = getelementptr inbounds i8, ptr addrspace(200) %gep1, i64 -4
  %gep3 = getelementptr inbounds i8, ptr addrspace(200) %gep2, i64 -2
  %inc = getelementptr i8, ptr addrspace(200) %gep3, i64 -10

  ret ptr addrspace(200) %inc
  ; CHECK-LABEL: @fold_set_inc_gep_sequence()
  ; CHECK-NEXT: entry:
  ; CHECK-NEXT: ret ptr addrspace(200) getelementptr (i8, ptr addrspace(200) null, i64 80)
}

define ptr addrspace(200) @fold_set_addr_inc_gep_sequence() local_unnamed_addr addrspace(200) nounwind {
entry:
  %set = tail call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) null, i64 100)
  %gep1 = getelementptr inbounds i8, ptr addrspace(200) %set, i64 -4
  %gep2 = getelementptr inbounds i8, ptr addrspace(200) %gep1, i64 -4
  %gep3 = getelementptr inbounds i8, ptr addrspace(200) %gep2, i64 -2
  %inc = getelementptr i8, ptr addrspace(200) %gep3, i64 -10
 
  ret ptr addrspace(200) %inc
  ; CHECK-LABEL: @fold_set_addr_inc_gep_sequence()
  ; CHECK-NEXT: entry:
  ; CHECK-NEXT: ret ptr addrspace(200) getelementptr (i8, ptr addrspace(200) null, i64 80)
}

define ptr addrspace(200) @fold_set_inc_gep_sequence_arg(ptr addrspace(200) %arg) local_unnamed_addr addrspace(200) nounwind {
  %set = tail call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) %arg, i64 100)
  %gep1 = getelementptr inbounds i8, ptr addrspace(200) %set, i64 -4
  %gep2 = getelementptr inbounds i8, ptr addrspace(200) %gep1, i64 -4
  %gep3 = getelementptr inbounds i8, ptr addrspace(200) %gep2, i64 -2
  %inc = getelementptr i8, ptr addrspace(200) %gep3, i64 -10
  ret ptr addrspace(200) %inc
  ; CHECK-LABEL: @fold_set_inc_gep_sequence_arg(ptr addrspace(200) %arg)
  ; CHECK-NEXT: tail call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) %arg, i64 80)
}

define ptr addrspace(200) @fold_set_addr_inc_gep_sequence_arg(ptr addrspace(200) %arg) local_unnamed_addr addrspace(200) nounwind {
  %set = tail call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) %arg, i64 100)
  %gep1 = getelementptr inbounds i8, ptr addrspace(200) %set, i64 -4
  %gep2 = getelementptr inbounds i8, ptr addrspace(200) %gep1, i64 -4
  %gep3 = getelementptr inbounds i8, ptr addrspace(200) %gep2, i64 -2
  %inc = getelementptr i8, ptr addrspace(200) %gep3, i64 -10
  ret ptr addrspace(200) %inc
  ; CHECK-LABEL: @fold_set_addr_inc_gep_sequence_arg(ptr addrspace(200) %arg)
  ; CHECK-NEXT: %set = tail call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) %arg, i64 80)
  ; CHECK-NEXT: ret ptr addrspace(200) %set
}

define ptr addrspace(200) @fold_inc_gep_sequence_null() local_unnamed_addr addrspace(200) nounwind {
  %src = getelementptr inbounds i8, ptr addrspace(200) null, i64 100
  %gep1 = getelementptr inbounds i8, ptr addrspace(200) %src, i64 -4
  %gep2 = getelementptr inbounds i8, ptr addrspace(200) %gep1, i64 -4
  %inc = getelementptr i8, ptr addrspace(200) %gep2, i64 -10
  ret ptr addrspace(200) %inc
  ; CHECK-LABEL: @fold_inc_gep_sequence_null()
  ; CHECK-NEXT: ret ptr addrspace(200) getelementptr (i8, ptr addrspace(200) null, i64 82)
}

define ptr addrspace(200) @fold_inc_gep_sequence_arg(ptr addrspace(200) %arg) local_unnamed_addr addrspace(200) nounwind {
  %inc1 = getelementptr i8, ptr addrspace(200) %arg, i64 100
  %gep1 = getelementptr inbounds i8, ptr addrspace(200) %inc1, i64 -4
  %gep2 = getelementptr inbounds i8, ptr addrspace(200) %gep1, i64 -4
  %inc2 = getelementptr i8, ptr addrspace(200) %gep2, i64 -10
  ret ptr addrspace(200) %inc2
  ; CHECK-LABEL: @fold_inc_gep_sequence_arg(ptr addrspace(200) %arg)
  ; CHECK-NEXT: %inc2 = getelementptr i8, ptr addrspace(200) %arg, i64 82
  ; CHECK-NEXT: ret ptr addrspace(200) %inc2
}

define ptr addrspace(200) @fold_gep_incoffset(ptr addrspace(200) %arg) local_unnamed_addr addrspace(200) nounwind {
  ; CHECK-LABEL: @fold_gep_incoffset(ptr addrspace(200) %arg)
  ; CHECK: %gep = getelementptr i8, ptr addrspace(200) %arg, i64 96
  ; CHECK: ret ptr addrspace(200) %gep
  %inc = getelementptr i8, ptr addrspace(200) %arg, i64 100
  %gep = getelementptr inbounds i8, ptr addrspace(200) %inc, i64 -4
  ret ptr addrspace(200) %gep
}

; TODO: Order of GEP vs incoffset should not matter:
define ptr addrspace(200) @fold_gep_incoffset2(ptr addrspace(200) %arg) local_unnamed_addr addrspace(200) nounwind {
  ; CHECK-LABEL: @fold_gep_incoffset2(ptr addrspace(200) %arg)
  ; CHECK-NOT: tail call ptr addrspace(200) @llvm.cheri.cap.offset.increment.i64(ptr addrspace(200) %arg, i64 96)
  %gep = getelementptr inbounds i8, ptr addrspace(200) %arg, i64 -4
  %inc = getelementptr i8, ptr addrspace(200) %gep, i64 100
  ret ptr addrspace(200) %inc
}

define ptr addrspace(200) @fold_null_setaddr_zero() addrspace(200) nounwind {
  %arg = tail call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) null, i64 0)
  %ret = call ptr addrspace(200) @check_fold_i8ptr(ptr addrspace(200) %arg)
  ret ptr addrspace(200) %ret
  ; CHECK-LABEL: @fold_null_setaddr_zero()
  ; CHECK-NEXT: %ret = call ptr addrspace(200) @check_fold_i8ptr(ptr addrspace(200) null)
  ; CHECK-NEXT: ret ptr addrspace(200) %ret
}

define ptr addrspace(200) @fold_null_setoffest_zero() addrspace(200) nounwind {
  %arg = tail call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) null, i64 0)
  %ret = call ptr addrspace(200) @check_fold_i8ptr(ptr addrspace(200) %arg)
  ret ptr addrspace(200) %ret
  ; CHECK-LABEL: @fold_null_setoffest_zero()
  ; CHECK-NEXT: %ret = call ptr addrspace(200) @check_fold_i8ptr(ptr addrspace(200) null)
  ; CHECK-NEXT: ret ptr addrspace(200) %ret
}

define ptr addrspace(200) @fold_null_incoffset_zero() addrspace(200) nounwind {
  %arg = getelementptr i8, ptr addrspace(200) null, i64 0
  %ret = call ptr addrspace(200) @check_fold_i8ptr(ptr addrspace(200) %arg)
  ret ptr addrspace(200) %ret
  ; CHECK-LABEL: @fold_null_incoffset_zero()
  ; CHECK-NEXT: %ret = call ptr addrspace(200) @check_fold_i8ptr(ptr addrspace(200) null)
  ; CHECK-NEXT: ret ptr addrspace(200) %ret
}

define ptr addrspace(200) @fold_incoffset_zero(ptr addrspace(200) %arg) addrspace(200) nounwind {
  %inc0 = getelementptr i8, ptr addrspace(200) %arg, i64 0
  %ret = call ptr addrspace(200) @check_fold_i8ptr(ptr addrspace(200) %inc0)
  ret ptr addrspace(200) %ret
  ; CHECK-LABEL: @fold_incoffset_zero(ptr addrspace(200) %arg)
  ; CHECK-NEXT: %ret = call ptr addrspace(200) @check_fold_i8ptr(ptr addrspace(200) %arg)
  ; CHECK-NEXT: ret ptr addrspace(200) %ret
}

define ptr addrspace(200) @fold_setoffset_null_zero_to_null() addrspace(200) nounwind {
  %ret = call ptr addrspace(200) @llvm.cheri.cap.offset.set.i64(ptr addrspace(200) null, i64 0)
  ret ptr addrspace(200) %ret
  ; CHECK-LABEL: @fold_setoffset_null_zero_to_null()
  ; CHECK-NEXT: ret ptr addrspace(200) null
}

define ptr addrspace(200) @fold_setaddr_null_zero_to_null() addrspace(200) nounwind {
  %ret = call ptr addrspace(200) @llvm.cheri.cap.address.set.i64(ptr addrspace(200) null, i64 0)
  ret ptr addrspace(200) %ret
  ; CHECK-LABEL: @fold_setaddr_null_zero_to_null()
  ; CHECK-NEXT: ret ptr addrspace(200) null
}
